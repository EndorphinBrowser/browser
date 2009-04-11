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

#include "webpage.h"

#include "browserapplication.h"
#include "downloadmanager.h"
#include "historymanager.h"
#include "networkaccessmanager.h"
#include "tabwidget.h"
#include "webpluginfactory.h"
#include "webview.h"

#include <qbuffer.h>
#include <qdesktopservices.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsettings.h>
#include <qwebframe.h>

WebPluginFactory *WebPage::s_webPluginFactory = 0;

WebPage::WebPage(QObject *parent)
    : QWebPage(parent)
    , m_openTargetBlankLinksIn(TabWidget::NewWindow)
{
    setPluginFactory(webPluginFactory());
    setNetworkAccessManager(BrowserApplication::networkAccessManager());
    connect(this, SIGNAL(unsupportedContent(QNetworkReply *)),
            this, SLOT(handleUnsupportedContent(QNetworkReply *)));
    loadSettings();
}

WebPluginFactory *WebPage::webPluginFactory()
{
    if (!s_webPluginFactory)
        s_webPluginFactory = new WebPluginFactory(BrowserApplication::instance());
    return s_webPluginFactory;
}

bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request,
                                      NavigationType type)
{
    QString scheme = request.url().scheme();
    if (scheme == QLatin1String("mailto")
        || scheme == QLatin1String("ftp")) {
        QDesktopServices::openUrl(request.url());
        return false;
    }

    TabWidget::OpenUrlIn openIn = frame ? TabWidget::CurrentTab : TabWidget::NewWindow;
    // If the user just clicked on the back or forward button on the toolbar
    if (type == QWebPage::NavigationTypeBackOrForward) {
        BrowserApplication::instance()->setEventMouseButtons(qApp->mouseButtons());
        BrowserApplication::instance()->setEventKeyboardModifiers(qApp->keyboardModifiers());
    }
    openIn = TabWidget::modifyWithUserBehavior(openIn);

    // handle the case where we want to do something different then
    // what qwebpage would do
    if (openIn == TabWidget::NewSelectedTab
        || openIn == TabWidget::NewNotSelectedTab
        || (frame && openIn == TabWidget::NewWindow)) {
        if (WebView *webView = qobject_cast<WebView*>(view())) {
            TabWidget *tabWidget = webView->tabWidget();
            if (tabWidget) {
                WebView *newView = tabWidget->getView(openIn, webView);
                QWebPage *page = 0;
                if (newView)
                    page = newView->page();
                if (page && page->mainFrame())
                    page->mainFrame()->load(request);
            }
        }
        return false;
    }

    bool accepted = QWebPage::acceptNavigationRequest(frame, request, type);
    if (accepted && frame == mainFrame())
        emit aboutToLoadUrl(request.url());

    return accepted;
}

void WebPage::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("tabs"));
    m_openTargetBlankLinksIn = (TabWidget::OpenUrlIn)settings.value(QLatin1String("openTargetBlankLinksIn"),
                                                                    TabWidget::NewWindow).toInt();
}

QWebPage *WebPage::createWindow(QWebPage::WebWindowType type)
{
    Q_UNUSED(type);
    if (WebView *webView = qobject_cast<WebView*>(view())) {
        TabWidget *tabWidget = webView->tabWidget();
        if (tabWidget) {
            TabWidget::OpenUrlIn openIn = m_openTargetBlankLinksIn;
            openIn = TabWidget::modifyWithUserBehavior(openIn);
            return tabWidget->getView(openIn, webView)->page();
        }
    }
    return 0;
}

QObject *WebPage::createPlugin(const QString &classId, const QUrl &url,
                               const QStringList &paramNames, const QStringList &paramValues)
{
    Q_UNUSED(classId);
    Q_UNUSED(url);
    Q_UNUSED(paramNames);
    Q_UNUSED(paramValues);
#if !defined(QT_NO_UITOOLS)
    QUiLoader loader;
    return loader.createWidget(classId, view());
#else
    return 0;
#endif
}

void WebPage::handleUnsupportedContent(QNetworkReply *reply)
{
    if (!reply)
        return;

    QUrl replyUrl = reply->url();

    switch (reply->error()) {
    case QNetworkReply::NoError:
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            BrowserApplication::downloadManager()->handleUnsupportedContent(reply);
            return;
        }
        break;
    case QNetworkReply::ProtocolUnknownError: {
        QSettings settings;
        settings.beginGroup(QLatin1String("WebView"));
        QStringList externalSchemes = settings.value(QLatin1String("externalSchemes")).toStringList();
        if (externalSchemes.contains(replyUrl.scheme())) {
            QDesktopServices::openUrl(replyUrl);
            return;
        }
        break;
    }
    default:
        break;
    }

    // Find the frame that has the unsupported content
    QWebFrame *notFoundFrame = 0;
    QList<QWebFrame*> frames;
    frames.append(mainFrame());
    while (!frames.isEmpty()) {
        QWebFrame *frame = frames.takeFirst();
        if (replyUrl == frame->url()) {
            notFoundFrame = frame;
            break;
        }
        frames.append(frame->childFrames());
    }
    if (!notFoundFrame)
        return;

    // Generate translated not found error page with an image
    QFile notFoundErrorFile(QLatin1String(":/notfound.html"));
    if (!notFoundErrorFile.open(QIODevice::ReadOnly))
        return;
    QString title = tr("Error loading page: %1").arg(QString::fromUtf8(replyUrl.toEncoded()));
    QString html = QLatin1String(notFoundErrorFile.readAll());
    QPixmap pixmap = qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view()).pixmap(QSize(32, 32));
    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    if (pixmap.save(&imageBuffer, "PNG")) {
        html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"),
                     QLatin1String(imageBuffer.buffer().toBase64()));
    }
    html = QString(html)
                   .arg(title)
                   .arg(reply->errorString())
                   .arg(tr("When connecting to: %1.").arg(QString::fromUtf8(replyUrl.toEncoded())))
                   .arg(tr("Check the address for errors such as <b>ww</b>.arora-browser.org instead of <b>www</b>.arora-browser.org"))
                   .arg(tr("If the address is correct, try checking the network connection."))
                   .arg(tr("If your computer or network is protected by a firewall or proxy, make sure that the browser is permitted to access the network."));
    notFoundFrame->setHtml(html, replyUrl);
    // Don't put error pages to the history.
    BrowserApplication::instance()->historyManager()->removeHistoryEntry(replyUrl, notFoundFrame->title());
}

