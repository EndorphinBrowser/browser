/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#include <qwebpage.h>

#include <tabwidget.h>

class QNetworkReply;
class WebPluginFactory;
class WebPage : public QWebPage
{
    Q_OBJECT

signals:
    void aboutToLoadUrl(const QUrl &url);

public:
    WebPage(QObject *parent = 0);
    void loadSettings();

    WebPluginFactory *webPluginFactory();

protected:
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request,
                                 NavigationType type);
    QObject *createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
    QWebPage *createWindow(QWebPage::WebWindowType type);

protected slots:
    void handleUnsupportedContent(QNetworkReply *reply);

protected:
    static WebPluginFactory *s_webPluginFactory;
    TabWidget::OpenUrlIn m_openTargetBlankLinksIn;

    QUrl m_requestedUrl;
};

#endif // WEBPAGE_H

