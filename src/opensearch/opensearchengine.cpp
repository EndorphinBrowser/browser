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
    , m_suggestionsRequest(0)
    , m_suggestionsReply(0)
    , m_iconRequest(0)
    , m_iconReply(0)
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

QUrl OpenSearchEngine::iconUrl() const
{
    return m_iconUrl;
}

void OpenSearchEngine::setIconUrl(const QUrl &iconUrl)
{
    m_iconUrl = iconUrl;

    loadIcon();
}

void OpenSearchEngine::loadIcon()
{
    if (!m_networkAccessManager || !m_iconUrl.isValid())
        return;

    if (m_iconRequest) {
        m_iconReply->abort();
        delete m_iconRequest;
    }

    m_iconRequest = new QNetworkRequest(m_iconUrl);
    m_iconReply = m_networkAccessManager->get(*m_iconRequest);
    connect(m_iconReply, SIGNAL(finished()), this, SLOT(iconObtained()));
}

void OpenSearchEngine::iconObtained()
{
    QByteArray response = m_iconReply->readAll();

    m_iconReply->close();
    delete m_iconRequest;
    m_iconRequest = 0;

    if (response.isEmpty())
        return;

    m_icon.loadFromData(response);

    emit iconChanged();
}

QPixmap OpenSearchEngine::icon() const
{
    if (m_icon.isNull())
        return QPixmap(QLatin1String(":defaulticon.png")).scaled(16, 16);

    return m_icon;
}

void OpenSearchEngine::setIcon(const QPixmap &icon)
{
    m_icon = icon;
}

bool OpenSearchEngine::isValid() const
{
    return (!m_name.isEmpty()
            && !m_description.isEmpty()
            && !m_searchUrl.isEmpty());
}

bool OpenSearchEngine::operator==(const OpenSearchEngine &other) const
{
    return (m_name == other.name()
            && m_searchUrl == other.searchUrl());
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

    if (m_networkAccessManager)
        delete m_networkAccessManager;

    m_networkAccessManager = manager;

    // Once we have set new network access manager, we can acually load the icon.
    if (!m_iconUrl.isEmpty() && m_icon.isNull())
        loadIcon();
}

void OpenSearchEngine::requestSuggestions(const QString &searchTerm)
{
    if (searchTerm.isEmpty() || !providesSuggestions())
        return;

    Q_ASSERT(m_networkAccessManager);

    if (!m_networkAccessManager)
        return;

    if (m_suggestionsRequest) {
        m_suggestionsReply->abort();
        delete m_suggestionsRequest;
    }

    m_suggestionsRequest = new QNetworkRequest(suggestionsUrl(searchTerm));
    m_suggestionsReply = m_networkAccessManager->get(*m_suggestionsRequest);
    connect(m_suggestionsReply, SIGNAL(finished()), this, SLOT(suggestionsObtained()));
}

void OpenSearchEngine::suggestionsObtained()
{
    QString response(QString::fromUtf8(m_suggestionsReply->readAll()));
    response = response.trimmed();

    m_suggestionsReply->close();
    delete m_suggestionsRequest;
    m_suggestionsRequest = 0;

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
