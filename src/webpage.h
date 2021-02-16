/*
 * Copyright 2020-2021 Aaron Dewes <aaron.dewes@web.de>
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

#ifndef WEBPAGE_H
#define WEBPAGE_H

#include "tabwidget.h"

#include <QList>
#include <QNetworkRequest>
#include <QWebEnginePage>

class WebPage;
class JavaScriptEndorphinObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *currentEngine READ currentEngine NOTIFY currentEngineChanged)

signals:
    void currentEngineChanged();

public:
    JavaScriptEndorphinObject(QObject *parent = nullptr, WebPage *page = nullptr);

public slots:
    QString translate(const QString &string);
    QObject *currentEngine() const;
    QString searchUrl(const QString &string) const;
    void addSearchProvider(const QString &url);
    QString getSetting(const QString &name, const QString &group);
    int setSetting(const QString &name, const QString &group, const int &value);
    int setSetting(const QString &name, const QString &group, const QString &value);
    int setSetting(const QString &name, const QString &group, const bool value);

private:
    WebPage *m_page;
};

class WebPageLinkedResource
{
public:
    QString rel;
    QString type;
    QUrl href;
    QString title;
};

class OpenSearchEngine;
class QNetworkReply;

class WebPage : public QWebEnginePage
{
    Q_OBJECT

signals:
    void aboutToLoadUrl(const QUrl &url);

public:
    WebPage(QWebEngineProfile *profile, QObject *parent = 0);
    ~WebPage();

    void loadSettings();

//    QList<WebPageLinkedResource> linkedResources(const QString &relation = QString());

    static QString userAgent();
    static void setUserAgent(const QString &userAgent);

protected:
    QString userAgentForUrl(const QUrl &url) const;
    /*
        bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request,
                                     NavigationType type);
    */
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame);
    QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type);

protected:
    static QString s_userAgent;
    TabWidget::OpenUrlIn m_openTargetBlankLinksIn;
    QUrl m_requestedUrl;

private:
    QNetworkRequest lastRequest;
    QWebEnginePage::NavigationType lastRequestType;

};

#endif // WEBPAGE_H

