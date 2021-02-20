/*
 * Copyright 2020-2021 Aaron Dewes <aaron.dewes@web.de>
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

#include <QBuffer>
#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

#include <QWebEnginePage>
#include <QWebChannel>
#include <QPrintDialog>
#include <QPrinter>
#include <QFile>
#include <QTextStream>

QString WebPage::s_userAgent;

Q_DECLARE_METATYPE(OpenSearchEngine*)
JavaScriptEndorphinObject::JavaScriptEndorphinObject(QObject *parent, WebPage *page)
    : QObject(parent)
    , m_page(page)
{
    static const char *translations[] = {
        QT_TR_NOOP("Welcome to Endorphin!"),
        QT_TR_NOOP("Endorphin Start"),
        QT_TR_NOOP("Search!"),
        QT_TR_NOOP("Search the web with"),
        QT_TR_NOOP("Search results provided by"),
        QT_TR_NOOP("About Endorphin")
    };
    Q_UNUSED(translations);

    qRegisterMetaType<OpenSearchEngine*>("OpenSearchEngine*");
}

void JavaScriptEndorphinObject::addSearchProvider(const QString &url)
{
    ToolbarSearch::openSearchManager()->addEngine(QUrl(url));
}

QString JavaScriptEndorphinObject::getSetting(const QString &name, const QString &group)
{
    if (m_page->url().toString() != "qrc:/settings.html")
        return nullptr;
    QSettings settings;
    settings.beginGroup(group);
    return settings.value(name).toString();
    settings.endGroup();
}

int JavaScriptEndorphinObject::setSetting(const QString &name, const QString &group, const QString &value)
{
    if (m_page->url().toString() != "qrc:/settings.html")
        return 1;
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(name, value);
    settings.endGroup();
    return 0;
}

int JavaScriptEndorphinObject::setSetting(const QString &name, const QString &group, const int &value)
{
    if (m_page->url().toString() != "qrc:/settings.html")
        return 1;
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(name, value);
    settings.endGroup();
    return 0;
}

int JavaScriptEndorphinObject::setSetting(const QString &name, const QString &group, const bool value)
{
    if (m_page->url().toString() != "qrc:/settings.html")
        return 1;
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(name, value);
    settings.endGroup();
    return 0;
}

QString JavaScriptEndorphinObject::translate(const QString &string)
{
    QString translatedString = tr(string.toUtf8().constData());

    // If the translation is the same as the original string
    // it could not be translated.  In that case
    // try to translate using the QApplication domain
    if (translatedString != string)
        return translatedString;
    else
        return qApp->tr(string.toUtf8().constData());
}

QObject *JavaScriptEndorphinObject::currentEngine() const
{
    return ToolbarSearch::openSearchManager()->currentEngine();
}

QString JavaScriptEndorphinObject::searchUrl(const QString &string) const
{
    return QString::fromUtf8(ToolbarSearch::openSearchManager()->currentEngine()->searchUrl(string).toEncoded());
}

WebPage::WebPage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
    , m_openTargetBlankLinksIn(TabWidget::NewWindow)
{
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("endorphin", new JavaScriptEndorphinObject(this, this));
    setWebChannel(channel);
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

    QWebElementCollection linkElements = mainFrame()->findAllElements(QStringLiteral("html > head > link"));

    Q_FOREACH (const QWebElement &linkElement, linkElements) {
        QString rel = linkElement.attribute(QStringLiteral("rel"));
        QString href = linkElement.attribute(QStringLiteral("href"));
        QString type = linkElement.attribute(QStringLiteral("type"));
        QString title = linkElement.attribute(QStringLiteral("title"));

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
        settings.remove(QStringLiteral("userAgent"));
    } else {
        settings.setValue(QStringLiteral("userAgent"), userAgent);
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
    if (scheme == QStringLiteral("mailto")
            || scheme == QStringLiteral("ftp")) {
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
        Q_EMIT aboutToLoadUrl(url);
    }

    return accepted;
}

void WebPage::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("tabs"));
    m_openTargetBlankLinksIn = (TabWidget::OpenUrlIn)settings.value(QStringLiteral("openTargetBlankLinksIn"),
                               TabWidget::NewSelectedTab).toInt();
    settings.endGroup();
    s_userAgent = settings.value(QStringLiteral("userAgent")).toString();
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
