/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright 2008 Jason A. Donenfeld <Jason@zx2c4.com>
 * Copyright 2008 Ariya Hidayat <ariya.hidayat@gmail.com>
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

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "webview.h"

#include "bookmarks.h"
#include "browserapplication.h"
#include "browsermainwindow.h"
#include "downloadmanager.h"
#include "opensearchengine.h"
#include "opensearchengineaction.h"
#include "opensearchmanager.h"
#include "toolbarsearch.h"
#include "webpage.h"

#include <qclipboard.h>
#include <qevent.h>
#include <qmenubar.h>
#include <qwebframe.h>

#include <qdebug.h>

WebView::WebView(QWidget *parent)
    : QWebView(parent)
    , m_progress(0)
    , m_currentZoom(100)
    , m_page(new WebPage(this))
{
    setPage(m_page);
    connect(page(), SIGNAL(statusBarMessage(const QString&)),
            SLOT(setStatusBarText(const QString&)));
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(setProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished()));
    connect(page(), SIGNAL(aboutToLoadUrl(const QUrl &)),
            this, SIGNAL(urlChanged(const QUrl &)));
    connect(page(), SIGNAL(downloadRequested(const QNetworkRequest &)),
            this, SLOT(downloadRequested(const QNetworkRequest &)));
#if QT_VERSION >= 0x040500
    connect(BrowserApplication::instance(), SIGNAL(zoomTextOnlyChanged(bool)),
            this, SLOT(applyZoom()));
#endif
    page()->setForwardUnsupportedContent(true);
    setAcceptDrops(true);

    // the zoom values (in percent) are chosen to be like in Mozilla Firefox 3
    m_zoomLevels << 30 << 50 << 67 << 80 << 90;
    m_zoomLevels << 100;
    m_zoomLevels << 110 << 120 << 133 << 150 << 170 << 200 << 240 << 300;
}

#if 1 // soon to be #if QT_VERSION <= 0x040600
#include <qdir.h>
QUrl WebView::guessUrlFromString(const QString &string)
{
    // Check the most common case of a valid url with scheme and host first
    QUrl url = QUrl::fromEncoded(string.toUtf8(), QUrl::TolerantMode);
    if (url.isValid() && !url.scheme().isEmpty() && !url.host().isEmpty())
        return url;

    // Absolute files that exists
    if (QDir::isAbsolutePath(string) && QFile::exists(string))
        return QUrl::fromLocalFile(string);

    // If the string is missing the scheme or the scheme is not valid prepend a scheme
    QString scheme = url.scheme();
    if (scheme.isEmpty() || scheme.contains(QLatin1Char('.')) || scheme == QLatin1String("localhost")) {
        QString urlString = string.trimmed();
        // Do not do anything for strings such as "foo", only "foo.com"
        int dotIndex = urlString.indexOf(QLatin1Char('.'));
        if (dotIndex != -1 || urlString.startsWith(QLatin1String("localhost"))) {
            const QString hostscheme = urlString.left(dotIndex).toLower();
            QByteArray scheme = (hostscheme == QLatin1String("ftp")) ? "ftp" : "http";
            urlString = QLatin1String(scheme) + QLatin1String("://") + urlString;
        }
        url = QUrl::fromEncoded(urlString.toUtf8(), QUrl::TolerantMode);
    }

    if (url.isValid())
        return url;

    return QUrl();
}
#endif

TabWidget *WebView::tabWidget() const
{
    QObject *widget = this->parent();
    while (widget) {
        if (TabWidget *tw = qobject_cast<TabWidget*>(widget))
            return tw;
        widget = widget->parent();
    }
    return 0;
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    if (!r.linkUrl().isEmpty()) {
        QAction *newWindowAction = menu->addAction(tr("Open in New &Window"), this, SLOT(openActionUrlInNewWindow()));
        newWindowAction->setData(r.linkUrl());
        QAction *newTabAction = menu->addAction(tr("Open in New &Tab"), this, SLOT(openActionUrlInNewTab()));
        newTabAction->setData(r.linkUrl());
        menu->addSeparator();
        menu->addAction(tr("Save Lin&k"), this, SLOT(downloadLinkToDisk()));
        menu->addAction(tr("&Bookmark This Link"), this, SLOT(bookmarkLink()))->setData(r.linkUrl());
        menu->addSeparator();
        if (!page()->selectedText().isEmpty())
            menu->addAction(pageAction(QWebPage::Copy));
        menu->addAction(tr("&Copy Link Location"), this, SLOT(copyLinkToClipboard()));
        if (page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
            menu->addAction(pageAction(QWebPage::InspectElement));
    }

    if (!r.imageUrl().isEmpty()) {
        if (!menu->isEmpty())
            menu->addSeparator();
        QAction *newWindowAction = menu->addAction(tr("Open Image in New &Window"), this, SLOT(openActionUrlInNewWindow()));
        newWindowAction->setData(r.imageUrl());
        QAction *newTabAction = menu->addAction(tr("Open Image in New &Tab"), this, SLOT(openActionUrlInNewTab()));
        newTabAction->setData(r.imageUrl());
        menu->addSeparator();
        menu->addAction(tr("&Save Image"), this, SLOT(downloadImageToDisk()));
        menu->addAction(tr("&Copy Image"), this, SLOT(copyImageToClipboard()));
        menu->addAction(tr("C&opy Image Location"), this, SLOT(copyImageLocationToClipboard()))->setData(r.imageUrl().toString());
    }

    if (!page()->selectedText().isEmpty()) {
        if (menu->isEmpty()) {
            menu->addAction(pageAction(QWebPage::Copy));
        } else {
            menu->addSeparator();
        }
        QMenu *searchMenu = menu->addMenu(tr("Search with..."));

        QList<QString> list = ToolbarSearch::openSearchManager()->allEnginesNames();
        for (int i = 0; i < list.count(); ++i) {
            QString name = list.at(i);
            OpenSearchEngine *engine = ToolbarSearch::openSearchManager()->engine(name);
            QAction *action = new OpenSearchEngineAction(engine, searchMenu);
            searchMenu->addAction(action);
            action->setData(name);
        }

        connect(searchMenu, SIGNAL(triggered(QAction *)), this, SLOT(searchRequested(QAction *)));
    }

#if QT_VERSION >= 0x040500
    if (menu->isEmpty()) {
        delete menu;
        menu = page()->createStandardContextMenu();
    }
#endif

    if (!menu->isEmpty()) {
        if (BrowserMainWindow::parentWindow(tabWidget())->menuBar()->isHidden()) {
            menu->addSeparator();
            menu->addAction(BrowserMainWindow::parentWindow(tabWidget())->showMenuBarAction());
        }

        menu->exec(mapToGlobal(event->pos()));
        delete menu;
        return;
    }
    delete menu;

    QWebView::contextMenuEvent(event);
}

void WebView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        m_currentZoom = m_currentZoom + numSteps * 10;
        applyZoom();
        event->accept();
        return;
    }
    QWebView::wheelEvent(event);
}

void WebView::resizeEvent(QResizeEvent *event)
{
    int offset = event->size().height() - event->oldSize().height();
    int currentValue = page()->mainFrame()->scrollBarValue(Qt::Vertical);
    setUpdatesEnabled(false);
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, currentValue - offset);
    setUpdatesEnabled(true);
    QWebView::resizeEvent(event);
}

void WebView::downloadLinkToDisk()
{
    pageAction(QWebPage::DownloadLinkToDisk)->trigger();
}

void WebView::copyLinkToClipboard()
{
    pageAction(QWebPage::CopyLinkToClipboard)->trigger();
}

void WebView::openActionUrlInNewTab()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QWebPage *page = tabWidget()->getView(TabWidget::NewNotSelectedTab, this)->page();
        QNetworkRequest request(action->data().toUrl());
        request.setRawHeader("Referer", url().toEncoded());
        page->mainFrame()->load(request);
    }
}

void WebView::openActionUrlInNewWindow()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QWebPage *page = tabWidget()->getView(TabWidget::NewWindow, this)->page();
        QNetworkRequest request(action->data().toUrl());
        request.setRawHeader("Referer", url().toEncoded());
        page->mainFrame()->load(request);
    }
}

void WebView::openImageInNewWindow()
{
    pageAction(QWebPage::OpenImageInNewWindow)->trigger();
}

void WebView::downloadImageToDisk()
{
    pageAction(QWebPage::DownloadImageToDisk)->trigger();
}

void WebView::copyImageToClipboard()
{
    pageAction(QWebPage::CopyImageToClipboard)->trigger();
}

void WebView::copyImageLocationToClipboard()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        BrowserApplication::clipboard()->setText(action->data().toString());
    }
}

void WebView::bookmarkLink()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        AddBookmarkDialog dialog;
        dialog.setUrl(QString::fromUtf8(action->data().toUrl().toEncoded()));
        dialog.exec();
    }
}

void WebView::searchRequested(QAction *action)
{
    QString searchText = page()->selectedText();

    if (searchText.isEmpty())
        return;

    QVariant index = action->data();

    if (index.canConvert<QString>()) {
        OpenSearchEngine *engine = ToolbarSearch::openSearchManager()->engine(index.toString());
        emit search(engine->searchUrl(searchText), TabWidget::NewSelectedTab);
    }
}

void WebView::setProgress(int progress)
{
    m_progress = progress;
}

int WebView::levelForZoom(int zoom)
{
    int i;

    i = m_zoomLevels.indexOf(zoom);
    if (i >= 0)
        return i;

    for (i = 0 ; i < m_zoomLevels.count(); ++i)
        if (zoom <= m_zoomLevels[i])
            break;

    if (i == m_zoomLevels.count())
        return i - 1;
    if (i == 0)
        return i;

    if (zoom - m_zoomLevels[i-1] > m_zoomLevels[i] - zoom)
        return i;
    else
        return i-1;
}

void WebView::applyZoom()
{
#if QT_VERSION >= 0x040500
    setZoomFactor(qreal(m_currentZoom) / 100.0);
#else
    setTextSizeMultiplier(qreal(m_currentZoom) / 100.0);
#endif
}

void WebView::zoomIn()
{
    int i = levelForZoom(m_currentZoom);

    if (i < m_zoomLevels.count() - 1)
        m_currentZoom = m_zoomLevels[i + 1];
    applyZoom();
}

void WebView::zoomOut()
{
    int i = levelForZoom(m_currentZoom);

    if (i > 0)
        m_currentZoom = m_zoomLevels[i - 1];
    applyZoom();
}

void WebView::resetZoom()
{
    m_currentZoom = 100;
    applyZoom();
}

void WebView::loadFinished()
{
    if (100 != m_progress) {
        qWarning() << "Received finished signal while progress is still:" << progress()
                   << "Url:" << url();
    }
    m_progress = 0;
}

void WebView::loadUrl(const QUrl &url, const QString &title)
{
    if (url.scheme() == QLatin1String("javascript")) {
        QString scriptSource = url.toString().mid(11);
        QVariant result = page()->mainFrame()->evaluateJavaScript(scriptSource);
        if (result.canConvert(QVariant::String)) {
            QString newHtml = result.toString();
            setHtml(newHtml);
        }
        return;
    }
    m_initialUrl = url;
    if (!title.isEmpty())
        emit titleChanged(tr("Loading..."));
    else
        emit titleChanged(title);
    load(url);
}

QString WebView::lastStatusBarText() const
{
    return m_statusBarText;
}

QUrl WebView::url() const
{
    QUrl url = QWebView::url();
    if (!url.isEmpty())
        return url;

    return m_initialUrl;
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    BrowserApplication::instance()->setEventMouseButtons(event->buttons());
    BrowserApplication::instance()->setEventKeyboardModifiers(event->modifiers());

    switch (event->button()) {
    case Qt::XButton1:
        pageAction(WebPage::Back)->trigger();
        break;
    case Qt::XButton2:
        pageAction(WebPage::Forward)->trigger();
        break;
    default:
        QWebView::mousePressEvent(event);
        break;
    }
}

void WebView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void WebView::dragMoveEvent(QDragMoveEvent *event)
{
    if (!event->mimeData()->urls().isEmpty()) {
        event->acceptProposedAction();
    } else {
        QUrl url(event->mimeData()->text());
        if (url.isValid())
            event->acceptProposedAction();
    }
    if (!event->isAccepted())
        QWebView::dragMoveEvent(event);
}

void WebView::dropEvent(QDropEvent *event)
{
    QWebView::dropEvent(event);
    if (!event->isAccepted()
        && event->possibleActions() & Qt::CopyAction) {

        QUrl url;
        if (!event->mimeData()->urls().isEmpty())
            url = event->mimeData()->urls().first();
        if (!url.isValid())
            url = event->mimeData()->text();
        if (url.isValid()) {
            loadUrl(url);
            event->acceptProposedAction();
        }
    }
}

void WebView::mouseReleaseEvent(QMouseEvent *event)
{
    const bool isAccepted = event->isAccepted();
    m_page->event(event);
    if (!event->isAccepted()
        && (BrowserApplication::instance()->eventMouseButtons() & Qt::MidButton)) {
        QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
        if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
            BrowserApplication::instance()->setEventMouseButtons(Qt::NoButton);
            loadUrl(url);
        }
    }
    event->setAccepted(isAccepted);
}

void WebView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Back:
        pageAction(WebPage::Back)->trigger();
        event->accept();
        break;
    case Qt::Key_Forward:
        pageAction(WebPage::Forward)->trigger();
        event->accept();
        break;
    case Qt::Key_Stop:
        pageAction(WebPage::Stop)->trigger();
        event->accept();
        break;
    case Qt::Key_Refresh:
        pageAction(WebPage::Reload)->trigger();
        event->accept();
        break;
    default:
        QWebView::keyPressEvent(event);
    }
}

void WebView::setStatusBarText(const QString &string)
{
    m_statusBarText = string;
}

void WebView::downloadRequested(const QNetworkRequest &request)
{
    BrowserApplication::downloadManager()->download(request);
}

