/**
 * Copyright (c) 2009, Benjamin C. Meyer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "googlesuggest.h"

#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qstringlist.h>
#include <qxmlstream.h>

#include <qdebug.h>

GoogleSuggest::GoogleSuggest(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(0)
{
}

void GoogleSuggest::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    m_networkAccessManager = manager;
}

void GoogleSuggest::suggest(const QString &searchText)
{
    Q_ASSERT(m_networkAccessManager);
    if (!m_networkAccessManager) {
        emit error(QLatin1String("QNetworkAccessManager is not set."));
        return;
    }

    if (searchText.isEmpty()) {
        emit suggestions(QStringList(), searchText);
        return;
    }

    QUrl url(QLatin1String("http://www.google.com/complete/search"));
    url.addEncodedQueryItem(QUrl::toPercentEncoding(QLatin1String("q")),
                            QUrl::toPercentEncoding(searchText));
    url.addQueryItem(QLatin1String("output"), QLatin1String("toolbar"));
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::User, searchText);
    QNetworkReply *reply = m_networkAccessManager->get(request);
    connect(reply, SIGNAL(finished()),
            this, SLOT(finished()));
}

void GoogleSuggest::finished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        return;
    }

    QStringList suggestionsList;
    QByteArray result = reply->readAll();
    QXmlStreamReader xml(result);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("suggestion")) {
                QStringRef str = xml.attributes().value(QLatin1String("data"));
                suggestionsList << str.toString();
            }
        }
    }
    QString searchText = reply->request().attribute(QNetworkRequest::User).toString();
    emit suggestions(suggestionsList, searchText);
    reply->deleteLater();
}

