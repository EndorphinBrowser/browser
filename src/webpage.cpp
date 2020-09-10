/*
 * Copyright 2020 Aaron Dewes <aaron.dewes@web.de>
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
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
#include "opensearchengine.h"
#include "opensearchmanager.h"
#include "tabwidget.h"
#include "toolbarsearch.h"
#include "webview.h"

#include <qbuffer.h>
#include <qdesktopservices.h>
#include <qmessagebox.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsettings.h>
#include <QUiLoader>

#include <QWebEnginePage>

QString WebPage::s_userAgent;

WebPage::WebPage(QObject *parent)
    : QWebEnginePage(parent)
    , m_openTargetBlankLinksIn(TabWidget::NewWindow)
{
    connect(this, SIGNAL(unsupportedContent(QNetworkReply *)),
            this, SLOT(handleUnsupportedContent(QNetworkReply *)));
    loadSettings();
}

WebPage::~WebPage()
{
}
/*
QList<WebPageLinkedResource> WebPage::linkedResources(const QString &relation)
{
    QList<WebPageLinkedResource> resources;

    QUrl baseUrl = mainFrame()->baseUrl();

    QWebElementCollection linkElements = mainFrame()->findAllElements(QLatin1String("html > head > link"));

    foreach (const QWebElement &linkElement, linkElements) {
        QString rel = linkElement.attribute(QLatin1String("rel"));
        QString href = linkElement.attribute(QLatin1String("href"));
        QString type = linkElement.attribute(QLatin1String("type"));
        QString title = linkElement.attribute(QLatin1String("title"));

        if (href.isEmpty() || type.isEmpty())
            continue;
        if (!relation.isEmpty() && rel != relation)
            continue;

        WebPageLinkedResource resource;
        resource.rel = rel;
        resource.type = type;
        resource.href = baseUrl.resolved(QUrl::fromEncoded(href.toUtf8()));
        resource.title = title;

        resources.append(resource);
    }

    return resources;
}
*/
QString WebPage::userAgent()
{
    return s_userAgent;
}

void WebPage::setUserAgent(const QString &userAgent)
{
    if (userAgent == s_userAgent)
        return;

    QSettings settings;
    if (userAgent.isEmpty()) {
        settings.remove(QLatin1String("userAgent"));
    } else {
        settings.setValue(QLatin1String("userAgent"), userAgent);
    }

    s_userAgent = userAgent;
}

QString WebPage::userAgentForUrl(const QUrl &url) const
{

    if (s_userAgent.isEmpty())
        //s_userAgent = QWebEnginePage::userAgentForUrl(url);
        s_userAgent = "";
    return s_userAgent;
}

/*
bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request,
                                      NavigationType type)
*/
bool WebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    /*
        lastRequest = request;
        lastRequestType = type;
    */
    QString scheme = url.scheme();
    if (scheme == QLatin1String("mailto")
            || scheme == QLatin1String("ftp")) {
        BrowserApplication::instance()->askDesktopToOpenUrl(url);
        return false;
    }
    /*
        if (type == QWebEnginePage::NavigationTypeFormResubmitted) {
            QMessageBox::StandardButton button = QMessageBox::warning(view(), tr("Resending POST request"),
                                 tr("In order to display the site, the request along with all the data must be sent once again, "
                                    "which may lead to some unexpected behaviour of the site e.g. the same action might be "
                                    "performed once again. Do you want to continue anyway?"), QMessageBox::Yes | QMessageBox::No);
            if (button != QMessageBox::Yes)
                return false;
        }
    */
    TabWidget::OpenUrlIn openIn =  isMainFrame ? TabWidget::CurrentTab : TabWidget::NewWindow;
    openIn = TabWidget::modifyWithUserBehavior(openIn);

    // handle the case where we want to do something different then
    // what QWebEnginePage would do
    if (openIn == TabWidget::NewSelectedTab
            || openIn == TabWidget::NewNotSelectedTab
            || (openIn == TabWidget::NewWindow)) {
        if (WebView *webView = qobject_cast<WebView*>(view())) {
            TabWidget *tabWidget = webView->tabWidget();
            if (tabWidget) {
                WebView *newView = tabWidget->getView(openIn, webView);
                QWebEnginePage *page = nullptr;
                if (newView)
                    page = newView->page();
                if (page)
                    page->load(url);
            }
        }
        return false;
    }

    bool accepted = QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
    if (accepted &&  isMainFrame) {
        m_requestedUrl = url;
        emit aboutToLoadUrl(url);
    }

    return accepted;
}

void WebPage::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("tabs"));
    m_openTargetBlankLinksIn = (TabWidget::OpenUrlIn)settings.value(QLatin1String("openTargetBlankLinksIn"),
                               TabWidget::NewSelectedTab).toInt();
    settings.endGroup();
    s_userAgent = settings.value(QLatin1String("userAgent")).toString();
}

QWebEnginePage *WebPage::createWindow(QWebEnginePage::WebWindowType type)
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
    return nullptr;
}

// The chromium guys have documented many examples of incompatibilities that
// different browsers have when they mime sniff.
// http://src.chromium.org/viewvc/chrome/trunk/src/net/base/mime_sniffer.cc
//
// All WebKit ports should share a common set of rules to sniff content.
// By having this here we are yet another browser that has different behavior :(
// But sadly QtWebKit does no sniffing at all so we are forced to do something.
static bool contentSniff(const QByteArray &data)
{
    if (data.contains("<!doctype")
            || data.contains("<script")
            || data.contains("<html")
            || data.contains("<!--")
            || data.contains("<head")
            || data.contains("<iframe")
            || data.contains("<h1")
            || data.contains("<div")
            || data.contains("<font")
            || data.contains("<table")
            || data.contains("<a")
            || data.contains("<style")
            || data.contains("<title")
            || data.contains("<b")
            || data.contains("<body")
            || data.contains("<br")
            || data.contains("<p"))
        return true;
    return false;
}

void WebPage::handleUnsupportedContent(QNetworkReply *reply)
{
    if (!reply)
        return;

    QUrl replyUrl = reply->url();

    if (replyUrl.scheme() == QLatin1String("abp"))
        return;

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
    if (replyUrl.isEmpty() || replyUrl != m_requestedUrl)
        return;

    if (reply->header(QNetworkRequest::ContentTypeHeader).toString().isEmpty()) {
        // do evil
        QByteArray data = reply->readAll();
        if (contentSniff(data)) {
            setHtml(QLatin1String(data), replyUrl);
            return;
        }
    }

    // Generate translated not found error page with an image
    QFile notFoundErrorFile(QLatin1String(":/notfound.html"));
    if (!notFoundErrorFile.open(QIODevice::ReadOnly))
        return;
    QString title = tr("Error loading page: %1").arg(QString::fromUtf8(replyUrl.toEncoded()));
    QString html = QLatin1String(notFoundErrorFile.readAll());
    QPixmap pixmap = qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning, nullptr, view()).pixmap(QSize(32, 32));
    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    if (pixmap.save(&imageBuffer, "PNG")) {
        html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"),
                     QLatin1String(imageBuffer.buffer().toBase64()));
    }
    html = html.arg(title,
                    reply->errorString(),
                    tr("When connecting to: %1.").arg(QString::fromUtf8(replyUrl.toEncoded())),
                    tr("Check the address for errors such as <b>ww</b>.example.com instead of <b>www</b>.example.com"),
                    tr("If the address is correct, try checking the network connection."),
                    tr("If your computer or network is protected by a firewall or proxy, make sure that the browser is permitted to access the network."));
    setHtml(html, replyUrl);
    // Don't put error pages to the history.
//    BrowserApplication::instance()->historyManager()->removeHistoryEntry(replyUrl, notFoundFrame->title());
}

