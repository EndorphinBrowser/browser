/*
 * Copyright 2020 Aaron Dewes <aaron.dewes@web.de>
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

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "webview.h"

#include "addbookmarkdialog.h"
#include "bookmarksmanager.h"
#include "browserapplication.h"
#include "browsermainwindow.h"
#include "downloadmanager.h"
#include "opensearchengine.h"
#include "opensearchengineaction.h"
#include "opensearchmanager.h"
#include "toolbarsearch.h"
#include "webpage.h"
#include "permissionbar.h"

#include <qclipboard.h>
#include <qdebug.h>
#include <qevent.h>
#include <qmenubar.h>
#include <qtimer.h>

#include <qinputdialog.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qtooltip.h>

#include <QMimeData>
#include <QUrlQuery>

#include <qdebug.h>

#include <QWebEngineContextMenuData>
#include <QWebEngineCertificateError>

WebView::WebView(QWidget* parent)
    : QWebEngineView(parent)
    , m_progress(0)
    , m_page(0)
{
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(setProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
    connect(this, &QWebEngineView::renderProcessTerminated,
            [=](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
        const char *status = "";
        switch (termStatus) {
        case QWebEnginePage::NormalTerminationStatus:
            status = "(normal exit)";
            break;
        case QWebEnginePage::AbnormalTerminationStatus:
            status = "(abnormal exit)";
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            status = "(crashed)";
            break;
        case QWebEnginePage::KilledTerminationStatus:
            status = "(killed)";
            break;
        }

        qInfo() << "Render process exited with code" << statusCode << status;
        QTimer::singleShot(0, [this] { reload(); });
    });
}

void WebView::setPage(WebPage *_page)
{
    m_page = _page;
    QWebEngineView::setPage(_page);
    disconnect(page(), &QWebEnginePage::iconChanged, this, &WebView::iconChanged);
    connect(page(), SIGNAL(iconChanged(QIcon)),
            this, SLOT(onIconChanged(QIcon)));
    connect(page(), &WebPage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
}

void WebView::loadSettings()
{
    /*
    QSettings settings;
    settings.beginGroup(QLatin1String("WebView"));
    m_enableAccessKeys = settings.value(QLatin1String("enableAccessKeys"), m_enableAccessKeys).toBool();

    if (!m_enableAccessKeys)
        hideAccessKeys();
    */
    m_page->loadSettings();
}

TabWidget *WebView::tabWidget() const
{
    QObject *widget = this->parent();
    while (widget) {
        if (TabWidget *tw = qobject_cast<TabWidget*>(widget))
            return tw;
        widget = widget->parent();
    }
    return nullptr;
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    if (!page()->contextMenuData().linkUrl().isEmpty()) {
        QAction *newWindowAction = menu->addAction(tr("Open in New &Window"), this, SLOT(openActionUrlInNewWindow()));
        newWindowAction->setData(page()->contextMenuData().linkUrl());
        QAction *newTabAction = menu->addAction(tr("Open in New &Tab"), this, SLOT(openActionUrlInNewTab()));
        newTabAction->setData(page()->contextMenuData().linkUrl());
        menu->addSeparator();
        menu->addAction(tr("Save Lin&k"), this, SLOT(downloadLinkToDisk()));
        menu->addAction(tr("&Bookmark This Link"), this, SLOT(bookmarkLink()))->setData(page()->contextMenuData().linkUrl());
        menu->addSeparator();
        if (!page()->selectedText().isEmpty())
            menu->addAction(pageAction(QWebEnginePage::Copy));
        menu->addAction(tr("&Copy Link Location"), this, SLOT(copyLinkToClipboard()));
    }

    if (page()->contextMenuData().mediaType() == QWebEngineContextMenuData::MediaTypeImage) {
        if (!menu->isEmpty())
            menu->addSeparator();
        QAction *newWindowAction = menu->addAction(tr("Open Image in New &Window"), this, SLOT(openActionUrlInNewWindow()));
        newWindowAction->setData(page()->contextMenuData().mediaUrl());
        QAction *newTabAction = menu->addAction(tr("Open Image in New &Tab"), this, SLOT(openActionUrlInNewTab()));
        newTabAction->setData(page()->contextMenuData().mediaUrl());
        menu->addSeparator();
        menu->addAction(tr("&Save Image"), this, SLOT(downloadImageToDisk()));
        menu->addAction(tr("&Copy Image"), this, SLOT(copyImageToClipboard()));
        menu->addAction(tr("C&opy Image Location"), this, SLOT(copyImageLocationToClipboard()))->setData(page()->contextMenuData().mediaUrl().toString());
    }

    if (!page()->selectedText().isEmpty()) {
        if (menu->isEmpty()) {
            menu->addAction(pageAction(QWebEnginePage::Copy));
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
    /*
        QWebElement element = r.element();
        if (!element.isNull()
            && element.tagName().toLower() == QLatin1String("input")
            && element.attribute(QLatin1String("type"), QLatin1String("text")) == QLatin1String("text")) {
            if (menu->isEmpty()) {
                menu->addAction(pageAction(QWebEnginePage::Copy));
            } else {
                menu->addSeparator();
            }

            QVariant variant;
            variant.setValue(element);
            menu->addAction(tr("Add to the toolbar search"), this, SLOT(addSearchEngine()))->setData(variant);
        }
    */
    if (menu->isEmpty()) {
        delete menu;
        menu = page()->createStandardContextMenu();
    }

    QAction *action = new QAction(menu);
    action->setText("Inspect Element");
    connect(action, &QAction::triggered, [this]() {
        emit devToolsRequested(page());
    });
    menu->addAction(action);

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

    QWebEngineView::contextMenuEvent(event);
}

void WebView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int numDegrees = event->angleDelta().y() / 8;
        int numSteps = numDegrees / 15;
        m_currentZoom = m_currentZoom + numSteps * 10;
        applyZoom();
        event->accept();
        return;
    }
    QWebEngineView::wheelEvent(event);
}

void WebView::resizeEvent(QResizeEvent *event)
{
    /*
        int offset = event->size().height() - event->oldSize().height();
        int currentValue = page()->mainFrame()->scrollBarValue(Qt::Vertical);
        setUpdatesEnabled(false);
        page()->mainFrame()->setScrollBarValue(Qt::Vertical, currentValue - offset);
        setUpdatesEnabled(true);
    */
    QWebEngineView::resizeEvent(event);
}

void WebView::downloadLinkToDisk()
{
    pageAction(QWebEnginePage::DownloadLinkToDisk)->trigger();
}

void WebView::copyLinkToClipboard()
{
    pageAction(QWebEnginePage::CopyLinkToClipboard)->trigger();
}

void WebView::openActionUrlInNewTab()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QWebEnginePage *page = tabWidget()->getView(TabWidget::NewNotSelectedTab, this)->page();
        page->load(action->data().toUrl());
    }
}

void WebView::openActionUrlInNewWindow()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QWebEnginePage *page = tabWidget()->getView(TabWidget::NewWindow, this)->page();
        page->load(action->data().toUrl());
    }
}

void WebView::openImageInNewWindow()
{
    //pageAction(QWebEnginePage::OpenImageInNewWindow)->trigger();
}

void WebView::downloadImageToDisk()
{
    pageAction(QWebEnginePage::DownloadImageToDisk)->trigger();
}

void WebView::copyImageToClipboard()
{
    pageAction(QWebEnginePage::CopyImageToClipboard)->trigger();
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
/*
void WebView::addSearchEngine()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QVariant variant = action->data();
    if (!variant.canConvert<QWebElement>())
        return;

    QWebElement element = qvariant_cast<QWebElement>(variant);
    QString elementName = element.attribute(QLatin1String("name"));
    QWebElement formElement = element;
    while (formElement.tagName().toLower() != QLatin1String("form"))
        formElement = formElement.parent();

    if (formElement.isNull() || formElement.attribute(QLatin1String("action")).isEmpty())
        return;

    QString method = formElement.attribute(QLatin1String("method"), QLatin1String("get")).toLower();
    if (method != QLatin1String("get")) {
        QMessageBox::warning(this, tr("Method not supported"),
                             tr("%1 method is not supported.").arg(method.toUpper()));
        return;
    }

    QUrl searchUrl(page()->mainFrame()->baseUrl().resolved(QUrl(formElement.attribute(QLatin1String("action")))));
    QMap<QString, QString> searchEngines;
    QWebElementCollection inputFields = formElement.findAll(QLatin1String("input"));
    QUrlQuery query(searchUrl.query());

    foreach (QWebElement inputField, inputFields) {
        QString type = inputField.attribute(QLatin1String("type"), QLatin1String("text"));
        QString name = inputField.attribute(QLatin1String("name"));
        QString value = inputField.evaluateJavaScript(QLatin1String("this.value")).toString();

        if (type == QLatin1String("submit")) {
            searchEngines.insert(value, name);
        } else if (type == QLatin1String("text")) {
            if (inputField == element)
                value = QLatin1String("{searchTerms}");

            query.addQueryItem(name, value);
        } else if (type == QLatin1String("checkbox") || type == QLatin1String("radio")) {
            if (inputField.evaluateJavaScript(QLatin1String("this.checked")).toBool()) {
                query.addQueryItem(name, value);
            }
        } else if (type == QLatin1String("hidden")) {
            query.addQueryItem(name, value);
        }
    }

    QWebElementCollection selectFields = formElement.findAll(QLatin1String("select"));
    foreach (QWebElement selectField, selectFields) {
        QString name = selectField.attribute(QLatin1String("name"));
        int selectedIndex = selectField.evaluateJavaScript(QLatin1String("this.selectedIndex")).toInt();
        if (selectedIndex == -1)
            continue;

        QWebElementCollection options = selectField.findAll(QLatin1String("option"));
        QString value = options.at(selectedIndex).toPlainText();
        query.addQueryItem(name, value);
    }

    bool ok = true;
    if (searchEngines.count() > 1) {
        QString searchEngine = QInputDialog::getItem(this, tr("Search engine"),
                                                    tr("Choose the desired search engine"), searchEngines.keys(),
                                                    0, false, &ok);
        if (!ok)
            return;
        if (!searchEngines[searchEngine].isEmpty())
            query.addQueryItem(searchEngines[searchEngine], searchEngine);
    }
    searchUrl.setQuery(query);

    QString engineName;
    QWebElementCollection labels = formElement.findAll(QString(QLatin1String("label[for=\"%1\"]")).arg(elementName));
    if (labels.count() > 0)
        engineName = labels.at(0).toPlainText();

    engineName = QInputDialog::getText(this, tr("Engine name"), tr("Type in a name for the engine"),
                                       QLineEdit::Normal, engineName, &ok);
    if (!ok)
        return;

    OpenSearchEngine *engine = new OpenSearchEngine();
    engine->setName(engineName);
    engine->setDescription(engineName);
    engine->setSearchUrlTemplate(searchUrl.toString());
    engine->setImage(icon().pixmap(16, 16).toImage());

    ToolbarSearch::openSearchManager()->addEngine(engine);
}
*/
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
    setZoomFactor(qreal(m_currentZoom) / 100.0);
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
        QString scriptSource = QUrl::fromPercentEncoding(url.toString(Q_FLAGS(QUrl::TolerantMode|QUrl::RemoveScheme)).toLatin1());
        QEventLoop loop;
        QObject::connect(this, SIGNAL(notifyRanJavaScript()), &loop, SLOT(quit()));
        page()->runJavaScript(scriptSource, [this](const QVariant &v)
        {
            this->ranJavaScript();
        });
        loop.exec();
        return;
    }
    m_initialUrl = url;
    if (!title.isEmpty())
        emit titleChanged(tr("Loading..."));
    else
        emit titleChanged(title);
    QUrl oldurl = QUrl(url.toString());
    QUrl newurl = QUrl(url.toString());
    if(newurl.toString().startsWith(QLatin1String("endorphin://"))) {
        QString url_tmp = newurl.toString().mid(12);
        newurl = QUrl(QLatin1String("qrc:/") + url_tmp);
        QString urlstr = newurl.toString();
        if(!urlstr.endsWith(QLatin1String(".html"))) {
            newurl = QUrl(urlstr + QLatin1String(".html"));
        }
    }
    load(newurl);
}

void WebView::ranJavaScript()
{
    emit notifyRanJavaScript();
}

QString WebView::lastStatusBarText() const
{
    return m_statusBarText;
}

QUrl WebView::url() const
{
    QUrl url = QWebEngineView::url();
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
        QWebEngineView::mousePressEvent(event);
        break;
    }
}

void WebView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void WebView::dragMoveEvent(QDragMoveEvent *event)
{
    event->ignore();
    if (event->source() != this) {
        if (!event->mimeData()->urls().isEmpty()) {
            event->acceptProposedAction();
        } else {
            QUrl url(event->mimeData()->text());
            if (url.isValid())
                event->acceptProposedAction();
        }
    }
    if (!event->isAccepted()) {
        QWebEngineView::dragMoveEvent(event);
    }
}

void WebView::dropEvent(QDropEvent *event)
{
    QWebEngineView::dropEvent(event);
    if (!event->isAccepted()
            && event->source() != this
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
    page()->event(event);
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

void WebView::setStatusBarText(const QString &string)
{
    m_statusBarText = string;
}

void WebView::downloadRequested(const QNetworkRequest &request)
{
    BrowserApplication::downloadManager()->download(request);
}

void WebView::keyPressEvent(QKeyEvent *event)
{
    /*
    if (m_enableAccessKeys) {
        m_accessKeysPressed = (event->modifiers() == Qt::ControlModifier
                               && event->key() == Qt::Key_Control);
        if (!m_accessKeysPressed) {
            if (checkForAccessKey(event)) {
                hideAccessKeys();
                event->accept();
                return;
            }
            hideAccessKeys();
        } else {
            QTimer::singleShot(300, this, SLOT(accessKeyShortcut()));
        }
    }
    */
    QWebEngineView::keyPressEvent(event);
}
/*
void WebView::accessKeyShortcut()
{
    if (!hasFocus()
        || !m_accessKeysPressed
        || !m_enableAccessKeys)
        return;
    if (m_accessKeyLabels.isEmpty()) {
        showAccessKeys();
    } else {
        hideAccessKeys();
    }
    m_accessKeysPressed = false;
}
*/
void WebView::keyReleaseEvent(QKeyEvent *event)
{
    /*
        if (m_enableAccessKeys)
            m_accessKeysPressed = event->key() == Qt::Key_Control;
    */
    QWebEngineView::keyReleaseEvent(event);
}

void WebView::focusOutEvent(QFocusEvent *event)
{
    /*
        if (m_accessKeysPressed) {
            hideAccessKeys();
            m_accessKeysPressed = false;
        }
    */
    QWebEngineView::focusOutEvent(event);
}

/*
bool WebView::checkForAccessKey(QKeyEvent *event)
{
    if (m_accessKeyLabels.isEmpty())
        return false;

    QString text = event->text();
    if (text.isEmpty())
        return false;
    QChar key = text.at(0).toUpper();
    bool handled = false;
    if (m_accessKeyNodes.contains(key)) {
        QWebElement element = m_accessKeyNodes[key];
        QPoint p = element.geometry().center();
        QWebFrame *frame = element.webFrame();
        Q_ASSERT(frame);
        do {
            p -= frame->scrollPosition();
            frame = frame->parentFrame();
        } while (frame && frame != page()->mainFrame());
        QMouseEvent pevent(QEvent::MouseButtonPress, p, Qt::LeftButton, 0, 0);
        qApp->sendEvent(this, &pevent);
        QMouseEvent revent(QEvent::MouseButtonRelease, p, Qt::LeftButton, 0, 0);
        qApp->sendEvent(this, &revent);
        handled = true;
    }
    return handled;
}

void WebView::hideAccessKeys()
{
    if (!m_accessKeyLabels.isEmpty()) {
        for (int i = 0; i < m_accessKeyLabels.count(); ++i) {
            QLabel *label = m_accessKeyLabels[i];
            label->hide();
            label->deleteLater();
        }
        m_accessKeyLabels.clear();
        m_accessKeyNodes.clear();
        update();
    }
}

void WebView::showAccessKeys()
{
    QStringList supportedElement;
    supportedElement << QLatin1String("input")
                     << QLatin1String("a")
                     << QLatin1String("area")
                     << QLatin1String("button")
                     << QLatin1String("label")
                     << QLatin1String("legend")
                     << QLatin1String("textarea");

    QList<QChar> unusedKeys;
    for (char c = 'A'; c <= 'Z'; ++c)
        unusedKeys << QLatin1Char(c);
    for (char c = '0'; c <= '9'; ++c)
        unusedKeys << QLatin1Char(c);

    QRect viewport = QRect(page()->mainFrame()->scrollPosition(), page()->viewportSize());
    // Priority first goes to elements with accesskey attributes
    QList<QWebElement> alreadyLabeled;
    foreach (const QString &elementType, supportedElement) {
        QList<QWebElement> result = page()->mainFrame()->findAllElements(elementType).toList();
        foreach (const QWebElement &element, result) {
            const QRect geometry = element.geometry();
            if (geometry.size().isEmpty()
                || !viewport.contains(geometry.topLeft())) {
                continue;
            }
            QString accessKeyAttribute = element.attribute(QLatin1String("accesskey")).toUpper();
            if (accessKeyAttribute.isEmpty())
                continue;
            QChar accessKey;
            for (int i = 0; i < accessKeyAttribute.count(); i+=2) {
                const QChar &possibleAccessKey = accessKeyAttribute[i];
                if (unusedKeys.contains(possibleAccessKey)) {
                    accessKey = possibleAccessKey;
                    break;
                }
            }
            if (accessKey.isNull())
                continue;
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
            alreadyLabeled.append(element);
        }
    }

    // Pick an access key first from the letters in the text and then from the
    // list of unused access keys
    foreach (const QString &elementType, supportedElement) {
        QWebElementCollection result = page()->mainFrame()->findAllElements(elementType);
        foreach (const QWebElement &element, result) {
            const QRect geometry = element.geometry();
            if (unusedKeys.isEmpty()
                || alreadyLabeled.contains(element)
                || geometry.size().isEmpty()
                || !viewport.contains(geometry.topLeft())) {
                continue;
            }
            QChar accessKey;
            QString text = element.toPlainText().toUpper();
            for (int i = 0; i < text.count(); ++i) {
                const QChar &c = text.at(i);
                if (unusedKeys.contains(c)) {
                    accessKey = c;
                    break;
                }
            }
            if (accessKey.isNull())
                accessKey = unusedKeys.takeFirst();
            unusedKeys.removeOne(accessKey);
            makeAccessKeyLabel(accessKey, element);
        }
    }
}

void WebView::makeAccessKeyLabel(const QChar &accessKey, const QWebElement &element)
{
    QLabel *label = new QLabel(this);
    label->setText(QString(QLatin1String("<qt><b>%1</b>")).arg(accessKey));

    QPalette p = QToolTip::palette();
    QColor color(Qt::yellow);
    color = color.lighter(150);
    color.setAlpha(175);
    p.setColor(QPalette::Window, color);
    label->setPalette(p);
    label->setAutoFillBackground(true);
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPoint point = element.geometry().center();
    point -= page()->mainFrame()->scrollPosition();
    label->move(point);
    label->show();
    point.setX(point.x() - label->width() / 2);
    label->move(point);
    m_accessKeyLabels.append(label);
    m_accessKeyNodes[accessKey] = element;
}
*/

void WebView::onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
    PermissionBar *permissionBar = new PermissionBar(this);
    connect(permissionBar, &PermissionBar::featurePermissionProvided, page(), &QWebEnginePage::setFeaturePermission);

    // Discard the bar on new loads (if we navigate away or reload).
    connect(page(), &QWebEnginePage::loadStarted, permissionBar, &QObject::deleteLater);

    permissionBar->requestPermission(securityOrigin, feature);
}