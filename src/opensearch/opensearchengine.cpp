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

#include <qnetworkaccessmanager.h>
#include <qnetworkrequest.h>
#include <qnetworkreply.h>
#include <qregexp.h>
#include <qwebframe.h>
#include <qwebpage.h>

OpenSearchEngine::OpenSearchEngine(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(0)
    , m_suggestionsReply(0)
{
}

QString OpenSearchEngine::parseTemplate(const QString &searchTerm, QString templ) const
{
    templ.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)searchTerms\\??\\}")),
            searchTerm);
    templ.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)count\\??\\}")),
            QLatin1String("20"));
    templ.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)startIndex\\??\\}")),
            QLatin1String("0")); //Use Index Offset
    templ.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)startPage\\??\\}")),
            QLatin1String("0")); // Use Page Offset
    templ.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)language\\??\\}")),
            QLatin1String("en")); // Be Better here
    templ.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)inputEncoding\\??\\}")),
            QLatin1String("UTF-8")); // Be better here
    templ.replace(QRegExp(QLatin1String("\\{([^\\}]*:|)outputEncoding\\??\\}")),
            QLatin1String("UTF-8")); // Be better here

    // Strip unknown paramters
    templ.replace(QRegExp(QLatin1String("\\{[^\\}]*\\?\\}")), QLatin1String(""));

    return templ;
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

QString OpenSearchEngine::searchUrl() const
{
    return m_searchUrl;
}

QUrl OpenSearchEngine::searchUrl(const QString &searchTerm) const
{
    if (m_searchUrl.isEmpty())
        return QUrl();

    QUrl retVal = parseTemplate(searchTerm, m_searchUrl);

    QHash<QString, QString>::const_iterator end = m_searchParameters.constEnd();
    QHash<QString, QString>::const_iterator i = m_searchParameters.constBegin();
    for (; i != end; ++i)
        retVal.addQueryItem(i.key(), parseTemplate(searchTerm, i.value()));

    return retVal;
}

void OpenSearchEngine::setSearchUrl(const QString &searchUrl)
{
    m_searchUrl = searchUrl;
}

bool OpenSearchEngine::providesSuggestions() const
{
    return !m_suggestionsUrl.isEmpty();
}

QString OpenSearchEngine::suggestionsUrl() const
{
    return m_suggestionsUrl;
}

QUrl OpenSearchEngine::suggestionsUrl(const QString &searchTerm) const
{
    if (m_suggestionsUrl.isEmpty())
        return QUrl();

    QUrl retVal = parseTemplate(searchTerm, m_suggestionsUrl);

    QHash<QString, QString>::const_iterator end = m_suggestionsParameters.constEnd();
    QHash<QString, QString>::const_iterator i = m_suggestionsParameters.constBegin();
    for (; i != end; ++i)
        retVal.addQueryItem(i.key(), parseTemplate(searchTerm, i.value()));

    return retVal;
}

void OpenSearchEngine::setSuggestionsUrl(const QString &suggestionsUrl)
{
    m_suggestionsUrl = suggestionsUrl;
}

QHash<QString, QString> OpenSearchEngine::searchParameters() const
{
    return m_searchParameters;
}

void OpenSearchEngine::setSearchParameters(const QHash<QString, QString> &searchParameters)
{
    m_searchParameters = searchParameters;
}

QHash<QString, QString> OpenSearchEngine::suggestionsParameters() const
{
    return m_suggestionsParameters;
}

void OpenSearchEngine::setSuggestionsParameters(const QHash<QString, QString> &suggestionsParameters)
{
    m_suggestionsParameters = suggestionsParameters;
}

QUrl OpenSearchEngine::imageUrl() const
{
    return m_imageUrl;
}

void OpenSearchEngine::setImageUrl(const QUrl &imageUrl)
{
    m_imageUrl = imageUrl;

    loadImage();
}

void OpenSearchEngine::loadImage()
{
    if (!m_networkAccessManager || !m_imageUrl.isValid())
        return;

    QNetworkReply *reply = m_networkAccessManager->get(QNetworkRequest(m_imageUrl));
    connect(reply, SIGNAL(finished()), this, SLOT(imageObtained()));
}

void OpenSearchEngine::imageObtained()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

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
    if (m_image.isNull())
        return QImage(QLatin1String(":defaulticon.png"));

    return m_image;
}

void OpenSearchEngine::setImage(const QImage &image)
{
    m_image = image;
}

bool OpenSearchEngine::isValid() const
{
    return (!m_name.isEmpty() && !m_searchUrl.isEmpty());
}

bool OpenSearchEngine::operator==(const OpenSearchEngine &other) const
{
    return (m_name == other.name() && m_searchUrl == other.searchUrl());
}

bool OpenSearchEngine::operator<(const OpenSearchEngine &other) const
{
    return (m_name < other.name());
}

QNetworkAccessManager *OpenSearchEngine::networkAccessManager() const
{
    return m_networkAccessManager;
}

void OpenSearchEngine::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    if (!manager)
        return;

    m_networkAccessManager = manager;

    // Once we have set new network access manager, we can actually load the image.
    if (!m_imageUrl.isEmpty() && m_image.isNull())
        loadImage();
}

void OpenSearchEngine::requestSuggestions(const QString &searchTerm)
{
    if (searchTerm.isEmpty() || !providesSuggestions())
        return;

    Q_ASSERT(m_networkAccessManager);

    if (!m_networkAccessManager)
        return;

    if (m_suggestionsReply) {
        m_suggestionsReply->abort();
        delete m_suggestionsReply;
        m_suggestionsReply = 0;
    }

    m_suggestionsReply = m_networkAccessManager->get(QNetworkRequest(suggestionsUrl(searchTerm)));
    connect(m_suggestionsReply, SIGNAL(finished()), this, SLOT(suggestionsObtained()));
}

void OpenSearchEngine::suggestionsObtained()
{
    QString response(QString::fromUtf8(m_suggestionsReply->readAll()));
    response = response.trimmed();

    m_suggestionsReply->close();
    delete m_suggestionsReply;
    m_suggestionsReply = 0;

    if (response.isEmpty())
        return;

    if (!response.startsWith(QLatin1Char('[')) || !response.endsWith(QLatin1Char(']')))
        return;

    // Evaluate the JSON response using QtWebKit's JavaScript engine.
    QVariantList responseParts = QWebPage().mainFrame()->evaluateJavaScript(response).toList();

    if (responseParts.count() < 2 || !responseParts.at(1).canConvert<QStringList>())
        return;

    QStringList suggestionsList = responseParts.at(1).toStringList();
    emit suggestions(suggestionsList);
}
