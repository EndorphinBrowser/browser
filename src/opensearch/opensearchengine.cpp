/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
 * Copyright 2009 Christian Franke <cfchris6@ts2server.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "opensearchengine.h"

#include "browserapplication.h"
#include "languagemanager.h"
#include "networkaccessmanager.h"

#include <qbuffer.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkrequest.h>
#include <qnetworkreply.h>
#include <qregexp.h>
#include <qscriptengine.h>
#include <qscriptvalue.h>
#include <qstringlist.h>

OpenSearchEngine::OpenSearchEngine(QObject *parent)
    : QObject(parent)
    , m_suggestionsReply(0)
    , m_scriptEngine(0)
{
}

OpenSearchEngine::~OpenSearchEngine()
{
    if (m_scriptEngine)
        m_scriptEngine->deleteLater();
}

QString OpenSearchEngine::parseTemplate(const QString &searchTerm, const QString &searchTemplate)
{
    QString language = BrowserApplication::languageManager()->currentLanguage();
    // Simple conversion to RFC 3066.
    language = language.replace(QLatin1Char('_'), QLatin1Char('-'));

    QString result = searchTemplate;
    result.replace(QLatin1String("{count}"), QLatin1String("20"));
    result.replace(QLatin1String("{startIndex}"), QLatin1String("0"));
    result.replace(QLatin1String("{startPage}"), QLatin1String("0"));
    result.replace(QLatin1String("{language}"), language);
    result.replace(QLatin1String("{inputEncoding}"), QLatin1String("UTF-8"));
    result.replace(QLatin1String("{outputEncoding}"), QLatin1String("UTF-8"));
    result.replace(QLatin1String("{searchTerms}"), searchTerm);
    result.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)source\\??\\}")), QCoreApplication::applicationName());

    return result;
}

QString OpenSearchEngine::name() const
{
    return m_name;
}

void OpenSearchEngine::setName(const QString &name)
{
    m_name = name;
}

QString OpenSearchEngine::description() const
{
    return m_description;
}

void OpenSearchEngine::setDescription(const QString &description)
{
    m_description = description;
}

QString OpenSearchEngine::searchUrlTemplate() const
{
    return m_searchUrlTemplate;
}

void OpenSearchEngine::setSearchUrlTemplate(const QString &searchUrlTemplate)
{
    m_searchUrlTemplate = searchUrlTemplate;
}

QUrl OpenSearchEngine::searchUrl(const QString &searchTerm) const
{
    if (m_searchUrlTemplate.isEmpty())
        return QUrl();

    QUrl retVal = QUrl::fromEncoded(parseTemplate(searchTerm, m_searchUrlTemplate).toUtf8());

    Parameters::const_iterator end = m_searchParameters.constEnd();
    Parameters::const_iterator i = m_searchParameters.constBegin();
    for (; i != end; ++i)
        retVal.addQueryItem(i->first, parseTemplate(searchTerm, i->second));

    return retVal;
}

bool OpenSearchEngine::providesSuggestions() const
{
    return !m_suggestionsUrlTemplate.isEmpty();
}

QString OpenSearchEngine::suggestionsUrlTemplate() const
{
    return m_suggestionsUrlTemplate;
}

void OpenSearchEngine::setSuggestionsUrlTemplate(const QString &suggestionsUrlTemplate)
{
    m_suggestionsUrlTemplate = suggestionsUrlTemplate;
}

QUrl OpenSearchEngine::suggestionsUrl(const QString &searchTerm) const
{
    if (m_suggestionsUrlTemplate.isEmpty())
        return QUrl();

    QUrl retVal = QUrl::fromEncoded(parseTemplate(searchTerm, m_suggestionsUrlTemplate).toUtf8());

    Parameters::const_iterator end = m_suggestionsParameters.constEnd();
    Parameters::const_iterator i = m_suggestionsParameters.constBegin();
    for (; i != end; ++i)
        retVal.addQueryItem(i->first, parseTemplate(searchTerm, i->second));

    return retVal;
}

OpenSearchEngine::Parameters OpenSearchEngine::searchParameters() const
{
    return m_searchParameters;
}

void OpenSearchEngine::setSearchParameters(const Parameters &searchParameters)
{
    m_searchParameters = searchParameters;
}

OpenSearchEngine::Parameters OpenSearchEngine::suggestionsParameters() const
{
    return m_suggestionsParameters;
}

void OpenSearchEngine::setSuggestionsParameters(const Parameters &suggestionsParameters)
{
    m_suggestionsParameters = suggestionsParameters;
}

QString OpenSearchEngine::imageUrl() const
{
    return m_imageUrl;
}

void OpenSearchEngine::setImageUrl(const QString &imageUrl)
{
    m_imageUrl = imageUrl;
}

void OpenSearchEngine::loadImage() const
{
    if (!networkAccessManager() || m_imageUrl.isEmpty())
        return;

    QNetworkReply *reply = networkAccessManager()->get(QNetworkRequest(QUrl::fromEncoded(m_imageUrl.toUtf8())));
    connect(reply, SIGNAL(finished()), this, SLOT(imageObtained()));
}

void OpenSearchEngine::imageObtained()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
        return;

    QByteArray response = reply->readAll();

    reply->close();
    reply->deleteLater();

    if (response.isEmpty())
        return;

    m_image.loadFromData(response);

    emit imageChanged();
}

QImage OpenSearchEngine::image() const
{
    if (m_image.isNull() && networkAccessManager() && !m_imageUrl.isEmpty())
        loadImage();
    return m_image;
}

void OpenSearchEngine::setImage(const QImage &image)
{
    if (m_imageUrl.isEmpty()) {
        QBuffer imageBuffer;
        imageBuffer.open(QBuffer::ReadWrite);
        if (image.save(&imageBuffer, "PNG")) {
            m_imageUrl = QString(QLatin1String("data:image/png;base64,%1"))
                         .arg(QLatin1String(imageBuffer.buffer().toBase64()));
        }
    }

    m_image = image;
}

bool OpenSearchEngine::isValid() const
{
    return (!m_name.isEmpty() && !m_searchUrlTemplate.isEmpty());
}

bool OpenSearchEngine::operator==(const OpenSearchEngine &other) const
{
    return (m_name == other.m_name
            && m_description == other.m_description
            && m_imageUrl == other.m_imageUrl
            && m_searchUrlTemplate == other.m_searchUrlTemplate
            && m_suggestionsUrlTemplate == other.m_suggestionsUrlTemplate
            && m_searchParameters == other.m_searchParameters
            && m_suggestionsParameters == other.m_suggestionsParameters);
}

bool OpenSearchEngine::operator<(const OpenSearchEngine &other) const
{
    return (m_name < other.m_name);
}

void OpenSearchEngine::requestSuggestions(const QString &searchTerm)
{
    if (searchTerm.isEmpty() || !providesSuggestions())
        return;

    Q_ASSERT(networkAccessManager());

    if (!networkAccessManager())
        return;

    if (m_suggestionsReply) {
        m_suggestionsReply->abort();
        m_suggestionsReply->deleteLater();
        m_suggestionsReply = 0;
    }

    m_suggestionsReply = networkAccessManager()->get(QNetworkRequest(suggestionsUrl(searchTerm)));
    connect(m_suggestionsReply, SIGNAL(finished()), this, SLOT(suggestionsObtained()));
}

void OpenSearchEngine::suggestionsObtained()
{
    QString response(QString::fromUtf8(m_suggestionsReply->readAll()));
    response = response.trimmed();

    m_suggestionsReply->close();
    m_suggestionsReply->deleteLater();
    m_suggestionsReply = 0;

    if (response.isEmpty())
        return;

    if (!response.startsWith(QLatin1Char('[')) || !response.endsWith(QLatin1Char(']')))
        return;

    if (!m_scriptEngine)
        m_scriptEngine = new QScriptEngine();

    // Evaluate the JSON response using QtScript.
    if (!m_scriptEngine->canEvaluate(response))
        return;

    QScriptValue responseParts = m_scriptEngine->evaluate(response);

    if (!responseParts.property(1).isArray())
        return;

    QStringList suggestionsList;
    qScriptValueToSequence(responseParts.property(1), suggestionsList);

    emit suggestions(suggestionsList);
}

QNetworkAccessManager *OpenSearchEngine::networkAccessManager()
{
    return BrowserApplication::networkAccessManager();
}

