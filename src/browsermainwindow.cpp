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

#include "browsermainwindow.h"

#include "aboutdialog.h"
#include "autosaver.h"
#include "bookmarks.h"
#include "browserapplication.h"
#include "clearprivatedata.h"
#include "downloadmanager.h"
#include "history.h"
#include "languagemanager.h"
#include "networkaccessmanager.h"
#include "networkmonitor.h"
#include "settings.h"
#include "sourceviewer.h"
#include "tabbar.h"
#include "tabwidget.h"
#include "toolbarsearch.h"
#include "webview.h"
#include "webviewsearch.h"

#include <qdesktopwidget.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qprintdialog.h>
#include <qprintpreviewdialog.h>
#include <qprinter.h>
#include <qsettings.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qinputdialog.h>
#include <qsplitter.h>

#include <qurl.h>
#include <qwebframe.h>
#include <qwebhistory.h>

#include <qdebug.h>

BrowserMainWindow::BrowserMainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_navigationBar(0)
    , m_navigationSplitter(0)
    , m_toolbarSearch(0)
#if defined(Q_WS_MAC)
    , m_bookmarksToolbarFrame(0)
#endif
    , m_bookmarksToolbar(0)
    , m_tabWidget(new TabWidget(this))
    , m_autoSaver(new AutoSaver(this))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    statusBar()->setSizeGripEnabled(true);
    // fixes https://bugzilla.mozilla.org/show_bug.cgi?id=219070
    // yes, that's a Firefox bug!
    statusBar()->setLayoutDirection(Qt::LeftToRight);
    setupMenu();
    setupToolBar();

    m_filePrivateBrowsingAction->setChecked(BrowserApplication::isPrivate());

    QWidget *centralWidget = new QWidget(this);
    BookmarksModel *boomarksModel = BrowserApplication::bookmarksManager()->bookmarksModel();
    m_bookmarksToolbar = new BookmarksToolBar(boomarksModel, this);
    connect(m_bookmarksToolbar, SIGNAL(openUrl(const QUrl&, const QString&)),
            m_tabWidget, SLOT(loadUrlFromUser(const QUrl&, const QString&)));
    connect(m_bookmarksToolbar, SIGNAL(openUrl(const QUrl&, TabWidget::OpenUrlIn, const QString&)),
            m_tabWidget, SLOT(loadUrl(const QUrl&, TabWidget::OpenUrlIn, const QString&)));
    connect(m_bookmarksToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            this, SLOT(updateBookmarksToolbarActionText(bool)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
#if defined(Q_WS_MAC)
    m_bookmarksToolbarFrame = new QFrame(this);
    m_bookmarksToolbarFrame->setLineWidth(1);
    m_bookmarksToolbarFrame->setMidLineWidth(0);
    m_bookmarksToolbarFrame->setFrameShape(QFrame::HLine);
    m_bookmarksToolbarFrame->setFrameShadow(QFrame::Raised);
    QPalette fp = m_bookmarksToolbarFrame->palette();
    fp.setColor(QPalette::Active, QPalette::Light, QColor(64, 64, 64));
    fp.setColor(QPalette::Active, QPalette::Dark, QColor(192, 192, 192));
    fp.setColor(QPalette::Inactive, QPalette::Light, QColor(135, 135, 135));
    fp.setColor(QPalette::Inactive, QPalette::Dark, QColor(226, 226, 226));
    m_bookmarksToolbarFrame->setAttribute(Qt::WA_MacNoClickThrough, true);
    m_bookmarksToolbarFrame->setPalette(fp);
    layout->addWidget(m_bookmarksToolbarFrame);

    layout->addWidget(m_bookmarksToolbar);
    QPalette p = m_bookmarksToolbar->palette();
    p.setColor(QPalette::Active, QPalette::Window, QColor(150, 150, 150));
    p.setColor(QPalette::Inactive, QPalette::Window, QColor(207, 207, 207));
    m_bookmarksToolbar->setAttribute(Qt::WA_MacNoClickThrough, true);
    m_bookmarksToolbar->setAutoFillBackground(true);
    m_bookmarksToolbar->setPalette(p);
    m_bookmarksToolbar->setBackgroundRole(QPalette::Window);
    m_bookmarksToolbar->setMaximumHeight(19);

    QWidget *w = new QWidget(this);
    w->setMaximumHeight(0);
    layout->addWidget(w); // <- OS X tab widget style bug
#else
    addToolBarBreak();
    addToolBar(m_bookmarksToolbar);
#endif
    layout->addWidget(m_tabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(m_tabWidget, SIGNAL(setCurrentTitle(const QString &)),
            this, SLOT(slotUpdateWindowTitle(const QString &)));
    connect(m_tabWidget, SIGNAL(showStatusBarMessage(const QString&)),
            statusBar(), SLOT(showMessage(const QString&)));
    connect(m_tabWidget, SIGNAL(linkHovered(const QString&)),
            statusBar(), SLOT(showMessage(const QString&)));
    connect(m_tabWidget, SIGNAL(loadProgress(int)),
            this, SLOT(slotLoadProgress(int)));
    connect(m_tabWidget, SIGNAL(tabsChanged()),
            m_autoSaver, SLOT(changeOccurred()));
    connect(m_tabWidget, SIGNAL(geometryChangeRequested(const QRect &)),
            this, SLOT(geometryChangeRequested(const QRect &)));
    connect(m_tabWidget, SIGNAL(printRequested(QWebFrame *)),
            this, SLOT(printRequested(QWebFrame *)));
    connect(m_tabWidget, SIGNAL(menuBarVisibilityChangeRequested(bool)),
            menuBar(), SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(statusBarVisibilityChangeRequested(bool)),
            statusBar(), SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(toolBarVisibilityChangeRequested(bool)),
            m_navigationBar, SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(toolBarVisibilityChangeRequested(bool)),
            m_bookmarksToolbar, SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(lastTabClosed()),
            this, SLOT(close()));

    slotUpdateWindowTitle();
    loadDefaultState();
    m_tabWidget->newTab();
    m_tabWidget->currentLineEdit()->setFocus();
#if defined(Q_WS_MAC)
    m_navigationBar->setIconSize(QSize(18, 18));
#endif

    // Add each item in the menu bar to the main window so
    // if the menu bar is hidden the shortcuts still work.
    QList<QAction *> actions = menuBar()->actions();
    foreach (QAction *action, actions) {
        if (action->menu())
            actions += action->menu()->actions();
        addAction(action);
    }
#if defined(Q_WS_MAC)
    setWindowIcon(QIcon());
#endif
#if defined(Q_WS_X11)
    setWindowRole(QLatin1String("browser"));
#endif
    retranslate();
}

BrowserMainWindow::~BrowserMainWindow()
{
    m_autoSaver->changeOccurred();
    m_autoSaver->saveIfNeccessary();
}

void BrowserMainWindow::loadDefaultState()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray data = settings.value(QLatin1String("defaultState")).toByteArray();
    restoreState(data);
    settings.endGroup();
}

QSize BrowserMainWindow::sizeHint() const
{
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    QSize size = desktopRect.size() * 0.9;
    return size;
}

void BrowserMainWindow::save()
{
    BrowserApplication::instance()->saveSession();

    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray data = saveState(false);
    settings.setValue(QLatin1String("defaultState"), data);
    settings.endGroup();
}

static const qint32 BrowserMainWindowMagic = 0xba;

QByteArray BrowserMainWindow::saveState(bool withTabs) const
{
    int version = 2;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << qint32(BrowserMainWindowMagic);
    stream << qint32(version);

    stream << size();
    stream << !m_navigationBar->isHidden();
    stream << !m_bookmarksToolbar->isHidden();
    stream << !statusBar()->isHidden();
    if (withTabs)
        stream << tabWidget()->saveState();
    else
        stream << QByteArray();
    stream << m_navigationSplitter->saveState();
    stream << m_tabWidget->tabBar()->showTabBarWhenOneTab();

    stream << qint32(toolBarArea(m_navigationBar));
    stream << qint32(toolBarArea(m_bookmarksToolbar));
    return data;
}

bool BrowserMainWindow::restoreState(const QByteArray &state)
{
    int version = 2;
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    if (stream.atEnd())
        return false;

    qint32 marker;
    qint32 v;
    stream >> marker;
    stream >> v;
    if (marker != BrowserMainWindowMagic || v != version)
        return false;

    QSize size;
    bool showToolbar;
    bool showBookmarksBar;
    bool showStatusbar;
    QByteArray tabState;
    QByteArray splitterState;
    bool showTabBarWhenOneTab;
    qint32 navigationBarLocation;
    qint32 bookmarkBarLocation;

    stream >> size;
    stream >> showToolbar;
    stream >> showBookmarksBar;
    stream >> showStatusbar;
    stream >> tabState;
    stream >> splitterState;
    stream >> showTabBarWhenOneTab;
    stream >> navigationBarLocation;
    stream >> bookmarkBarLocation;

    resize(size);

    m_navigationBar->setVisible(showToolbar);
    updateToolbarActionText(showToolbar);

    m_bookmarksToolbar->setVisible(showBookmarksBar);
#if defined(Q_WS_MAC)
    m_bookmarksToolbarFrame->setVisible(showBookmarksBar);
#endif
    updateBookmarksToolbarActionText(showBookmarksBar);

    statusBar()->setVisible(showStatusbar);
    updateStatusbarActionText(showStatusbar);

    m_statusBarVisible = showStatusbar;

    m_navigationSplitter->restoreState(splitterState);

    if (!tabState.isEmpty() && !tabWidget()->restoreState(tabState))
        return false;

    m_tabWidget->tabBar()->setShowTabBarWhenOneTab(showTabBarWhenOneTab);

    Qt::ToolBarArea navigationArea = Qt::ToolBarArea(navigationBarLocation);
    if (navigationArea != Qt::TopToolBarArea && navigationArea != Qt::NoToolBarArea)
        addToolBar(navigationArea, m_navigationBar);
    Qt::ToolBarArea bookmarkArea = Qt::ToolBarArea(bookmarkBarLocation);
    if (bookmarkArea != Qt::TopToolBarArea && bookmarkArea != Qt::NoToolBarArea)
        addToolBar(bookmarkArea, m_bookmarksToolbar);

    return true;
}

QAction *BrowserMainWindow::showMenuBarAction() const
{
    return m_viewShowMenuBarAction;
}

void BrowserMainWindow::setupMenu()
{
    m_menuBarVisible = true;

    new QShortcut(QKeySequence(Qt::Key_F6), this, SLOT(slotSwapFocus()));

    // File
    m_fileMenu = new QMenu(menuBar());
    menuBar()->addMenu(m_fileMenu);

    m_fileNewWindowAction = new QAction(m_fileMenu);
    m_fileNewWindowAction->setShortcut(QKeySequence::New);
    connect(m_fileNewWindowAction, SIGNAL(triggered()),
            this, SLOT(slotFileNew()));
    m_fileMenu->addAction(m_fileNewWindowAction);
    m_fileMenu->addAction(m_tabWidget->newTabAction());

    m_fileOpenFileAction = new QAction(m_fileMenu);
    m_fileOpenFileAction->setShortcut(QKeySequence::Open);
    connect(m_fileOpenFileAction, SIGNAL(triggered()),
            this, SLOT(slotFileOpen()));
    m_fileMenu->addAction(m_fileOpenFileAction);

    m_fileOpenLocationAction = new QAction(m_fileMenu);
    m_fileOpenLocationAction->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_L));
    connect(m_fileOpenLocationAction, SIGNAL(triggered()),
            this, SLOT(slotSelectLineEdit()));
    m_fileMenu->addAction(m_fileOpenLocationAction);

    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_tabWidget->closeTabAction());
    m_fileMenu->addSeparator();

    m_fileSaveAsAction = new QAction(m_fileMenu);
    m_fileSaveAsAction->setShortcut(QKeySequence::Save);
    connect(m_fileSaveAsAction, SIGNAL(triggered()),
            this, SLOT(slotFileSaveAs()));
    m_fileMenu->addAction(m_fileSaveAsAction);
    m_fileMenu->addSeparator();

    BookmarksManager *bookmarksManager = BrowserApplication::bookmarksManager();
    m_fileImportBookmarksAction = new QAction(m_fileMenu);
    connect(m_fileImportBookmarksAction, SIGNAL(triggered()),
            bookmarksManager, SLOT(importBookmarks()));
    m_fileMenu->addAction(m_fileImportBookmarksAction);
    m_fileExportBookmarksAction = new QAction(m_fileMenu);
    connect(m_fileExportBookmarksAction, SIGNAL(triggered()),
            bookmarksManager, SLOT(exportBookmarks()));
    m_fileMenu->addAction(m_fileExportBookmarksAction);
    m_fileMenu->addSeparator();

    m_filePrintPreviewAction= new QAction(m_fileMenu);
    connect(m_filePrintPreviewAction, SIGNAL(triggered()),
            this, SLOT(slotFilePrintPreview()));
    m_fileMenu->addAction(m_filePrintPreviewAction);

    m_filePrintAction = new QAction(m_fileMenu);
    m_filePrintAction->setShortcut(QKeySequence::Print);
    connect(m_filePrintAction, SIGNAL(triggered()),
            this, SLOT(slotFilePrint()));
    m_fileMenu->addAction(m_filePrintAction);
    m_fileMenu->addSeparator();

    m_filePrivateBrowsingAction = new QAction(m_fileMenu);
    connect(m_filePrivateBrowsingAction, SIGNAL(triggered()),
            this, SLOT(slotPrivateBrowsing()));
    m_filePrivateBrowsingAction->setCheckable(true);
    m_fileMenu->addAction(m_filePrivateBrowsingAction);
    m_fileMenu->addSeparator();

    m_fileCloseWindow = new QAction(m_fileMenu);
    connect(m_fileCloseWindow, SIGNAL(triggered()), this, SLOT(close()));
    m_fileCloseWindow->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
    m_fileMenu->addAction(m_fileCloseWindow);

    m_fileQuit = new QAction(m_fileMenu);
    connect(m_fileQuit, SIGNAL(triggered()), BrowserApplication::instance(), SLOT(quitBrowser()));
    m_fileQuit->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    m_fileMenu->addAction(m_fileQuit);

    // Edit
    m_editMenu = new QMenu(menuBar());
    menuBar()->addMenu(m_editMenu);
    m_editUndoAction = new QAction(m_editMenu);
    m_editUndoAction->setShortcuts(QKeySequence::Undo);
    m_tabWidget->addWebAction(m_editUndoAction, QWebPage::Undo);
    m_editMenu->addAction(m_editUndoAction);
    m_editRedoAction = new QAction(m_editMenu);
    m_editRedoAction->setShortcuts(QKeySequence::Redo);
    m_tabWidget->addWebAction(m_editRedoAction, QWebPage::Redo);
    m_editMenu->addAction(m_editRedoAction);
    m_editMenu->addSeparator();
    m_editCutAction = new QAction(m_editMenu);
    m_editCutAction->setShortcuts(QKeySequence::Cut);
    m_tabWidget->addWebAction(m_editCutAction, QWebPage::Cut);
    m_editMenu->addAction(m_editCutAction);
    m_editCopyAction = new QAction(m_editMenu);
    m_editCopyAction->setShortcuts(QKeySequence::Copy);
    m_tabWidget->addWebAction(m_editCopyAction, QWebPage::Copy);
    m_editMenu->addAction(m_editCopyAction);
    m_editPasteAction = new QAction(m_editMenu);
    m_editPasteAction->setShortcuts(QKeySequence::Paste);
    m_tabWidget->addWebAction(m_editPasteAction, QWebPage::Paste);
    m_editMenu->addAction(m_editPasteAction);
    m_editMenu->addSeparator();

    m_editFindAction = new QAction(m_editMenu);
    m_editFindAction->setShortcuts(QKeySequence::Find);
    connect(m_editFindAction, SIGNAL(triggered()), this, SLOT(slotEditFind()));
    m_editMenu->addAction(m_editFindAction);
    new QShortcut(QKeySequence(Qt::Key_Slash), this, SLOT(slotEditFind()));

    m_editFindNextAction = new QAction(m_editMenu);
    m_editFindNextAction->setShortcuts(QKeySequence::FindNext);
    connect(m_editFindNextAction, SIGNAL(triggered()), this, SLOT(slotEditFindNext()));
    m_editMenu->addAction(m_editFindNextAction);

    m_editFindPreviousAction = new QAction(m_editMenu);
    m_editFindPreviousAction->setShortcuts(QKeySequence::FindPrevious);
    connect(m_editFindPreviousAction, SIGNAL(triggered()), this, SLOT(slotEditFindPrevious()));
    m_editMenu->addAction(m_editFindPreviousAction);

    m_editMenu->addSeparator();
    m_editPreferencesAction = new QAction(m_editMenu);
    connect(m_editPreferencesAction, SIGNAL(triggered()),
            this, SLOT(slotPreferences()));
    m_editMenu->addAction(m_editPreferencesAction);

    // View
    m_viewMenu = new QMenu(menuBar());
    menuBar()->addMenu(m_viewMenu);
    m_viewShowMenuBarAction = new QAction(m_viewMenu);
    m_viewShowMenuBarAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    connect(m_viewShowMenuBarAction, SIGNAL(triggered()), this, SLOT(slotViewMenuBar()));
    addAction(m_viewShowMenuBarAction);

    m_viewToolbarAction = new QAction(this);
    updateToolbarActionText(true);
    connect(m_viewToolbarAction, SIGNAL(triggered()), this, SLOT(slotViewToolbar()));
    m_viewMenu->addAction(m_viewToolbarAction);

    m_viewBookmarkBarAction = new QAction(m_viewMenu);
    updateBookmarksToolbarActionText(true);
    connect(m_viewBookmarkBarAction, SIGNAL(triggered()), this, SLOT(slotViewBookmarksBar()));
    m_viewMenu->addAction(m_viewBookmarkBarAction);

    QAction *viewTabBarAction = m_tabWidget->tabBar()->viewTabBarAction();
    m_viewMenu->addAction(viewTabBarAction);
    connect(viewTabBarAction, SIGNAL(changed()),
            m_autoSaver, SLOT(changeOccurred()));

    m_viewStatusbarAction = new QAction(m_viewMenu);
    updateStatusbarActionText(true);
    connect(m_viewStatusbarAction, SIGNAL(triggered()), this, SLOT(slotViewStatusbar()));
    m_viewMenu->addAction(m_viewStatusbarAction);

    m_viewMenu->addSeparator();

    m_viewStopAction = new QAction(m_viewMenu);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_viewStopAction->setShortcuts(shortcuts);
    m_tabWidget->addWebAction(m_viewStopAction, QWebPage::Stop);
    m_viewMenu->addAction(m_viewStopAction);

    m_viewReloadAction = new QAction(m_viewMenu);
    m_viewReloadAction->setShortcuts(QKeySequence::Refresh);
    m_tabWidget->addWebAction(m_viewReloadAction, QWebPage::Reload);
    m_viewMenu->addAction(m_viewReloadAction);

    m_viewZoomInAction = new QAction(m_viewMenu);
    m_viewZoomInAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(m_viewZoomInAction, SIGNAL(triggered()),
            this, SLOT(slotZoomIn()));
    m_viewMenu->addAction(m_viewZoomInAction);

    m_viewZoomNormalAction = new QAction(m_viewMenu);
    m_viewZoomNormalAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(m_viewZoomNormalAction, SIGNAL(triggered()),
            this, SLOT(slotZoomNormal()));
    m_viewMenu->addAction(m_viewZoomNormalAction);

    m_viewZoomOutAction = new QAction(m_viewMenu);
    m_viewZoomOutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(m_viewZoomOutAction, SIGNAL(triggered()),
            this, SLOT(slotZoomOut()));
    m_viewMenu->addAction(m_viewZoomOutAction);

    m_viewZoomTextOnlyAction = new QAction(m_viewMenu);
    connect(m_viewZoomTextOnlyAction, SIGNAL(triggered()),
            this, SLOT(slotZoomOut()));
    m_viewZoomTextOnlyAction->setCheckable(true);
#if QT_VERSION >= 0x040500
    connect(m_viewZoomTextOnlyAction, SIGNAL(toggled(bool)),
            BrowserApplication::instance(), SLOT(setZoomTextOnly(bool)));
    connect(BrowserApplication::instance(), SIGNAL(zoomTextOnlyChanged(bool)),
            this, SLOT(slotZoomTextOnlyChanged(bool)));
    m_viewMenu->addAction(m_viewZoomTextOnlyAction);
#endif

    m_viewMenu->addSeparator();

    m_viewSourceAction = new QAction(m_viewMenu);
    connect(m_viewSourceAction, SIGNAL(triggered()),
            this, SLOT(slotViewPageSource()));
    m_viewMenu->addAction(m_viewSourceAction);

    m_viewFullScreenAction = new QAction(m_viewMenu);
    m_viewFullScreenAction->setShortcut(Qt::Key_F11);
    connect(m_viewFullScreenAction, SIGNAL(triggered(bool)),
            this, SLOT(slotViewFullScreen(bool)));
    m_viewFullScreenAction->setCheckable(true);
    m_viewMenu->addAction(m_viewFullScreenAction);

    // History
    m_historyMenu = new HistoryMenu(this);
    connect(m_historyMenu, SIGNAL(openUrl(const QUrl&, const QString&)),
            m_tabWidget, SLOT(loadUrlFromUser(const QUrl&, const QString&)));
    menuBar()->addMenu(m_historyMenu);
    QList<QAction*> historyActions;

    m_historyBackAction = new QAction(this);
    m_tabWidget->addWebAction(m_historyBackAction, QWebPage::Back);
    m_historyBackAction->setShortcuts(QKeySequence::Back);
    m_historyBackAction->setIconVisibleInMenu(false);

    m_historyForwardAction = new QAction(this);
    m_tabWidget->addWebAction(m_historyForwardAction, QWebPage::Forward);
    m_historyForwardAction->setShortcuts(QKeySequence::Forward);
    m_historyForwardAction->setIconVisibleInMenu(false);

    m_historyHomeAction = new QAction(this);
    connect(m_historyHomeAction, SIGNAL(triggered()), this, SLOT(slotHome()));
    m_historyHomeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

    m_historyRestoreLastSessionAction = new QAction(this);
    connect(m_historyRestoreLastSessionAction, SIGNAL(triggered()),
            BrowserApplication::instance(), SLOT(restoreLastSession()));
    m_historyRestoreLastSessionAction->setEnabled(BrowserApplication::instance()->canRestoreSession());

    historyActions.append(m_historyBackAction);
    historyActions.append(m_historyForwardAction);
    historyActions.append(m_historyHomeAction);
    historyActions.append(m_tabWidget->recentlyClosedTabsAction());
    historyActions.append(m_historyRestoreLastSessionAction);
    m_historyMenu->setInitialActions(historyActions);

    // Bookmarks
    m_bookmarksMenu = new BookmarksMenu(this);
    connect(m_bookmarksMenu, SIGNAL(openUrl(const QUrl&, const QString &)),
            m_tabWidget, SLOT(loadUrlFromUser(const QUrl&, const QString&)));
    menuBar()->addMenu(m_bookmarksMenu);

    m_bookmarksShowAllAction = new QAction(this);
    m_bookmarksShowAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_B));
    connect(m_bookmarksShowAllAction, SIGNAL(triggered()),
            this, SLOT(slotShowBookmarksDialog()));

    m_bookmarksAddAction = new QAction(this);
    m_bookmarksAddAction->setIcon(QIcon(QLatin1String(":addbookmark.png")));
    m_bookmarksAddAction->setIconVisibleInMenu(false);
    connect(m_bookmarksAddAction, SIGNAL(triggered()),
            this, SLOT(slotAddBookmark()));
    m_bookmarksAddAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

    QList<QAction*> bookmarksActions;
    bookmarksActions.append(m_bookmarksShowAllAction);
    bookmarksActions.append(m_bookmarksAddAction);
    m_bookmarksMenu->setInitialActions(bookmarksActions);

    // Window
    m_windowMenu = new QMenu(menuBar());
    menuBar()->addMenu(m_windowMenu);
    connect(m_windowMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowWindowMenu()));
    slotAboutToShowWindowMenu();

    // Tools
    m_toolsMenu = new QMenu(menuBar());
    menuBar()->addMenu(m_toolsMenu);

    m_toolsWebSearchAction = new QAction(m_toolsMenu);
    connect(m_toolsWebSearchAction, SIGNAL(triggered()),
            this, SLOT(slotWebSearch()));
    m_toolsMenu->addAction(m_toolsWebSearchAction);

    m_toolsClearPrivateDataAction = new QAction(m_toolsMenu);
    connect(m_toolsClearPrivateDataAction, SIGNAL(triggered()),
            this, SLOT(slotClearPrivateData()));
    m_toolsMenu->addAction(m_toolsClearPrivateDataAction);

    m_toolsShowNetworkMonitor = new QAction(m_toolsMenu);
    connect(m_toolsShowNetworkMonitor, SIGNAL(triggered()),
            this, SLOT(showNetworkMonitor()));
    m_toolsMenu->addAction(m_toolsShowNetworkMonitor);

    m_toolsEnableInspector = new QAction(m_toolsMenu);
    connect(m_toolsEnableInspector, SIGNAL(triggered(bool)),
            this, SLOT(slotToggleInspector(bool)));
    m_toolsEnableInspector->setCheckable(true);
    QSettings settings;
    settings.beginGroup(QLatin1String("websettings"));
    m_toolsEnableInspector->setChecked(settings.value(QLatin1String("enableInspector"), false).toBool());
    m_toolsMenu->addAction(m_toolsEnableInspector);

    // Help
    m_helpMenu = new QMenu(menuBar());
    menuBar()->addMenu(m_helpMenu);

    m_helpChangeLanguageAction = new QAction(m_helpMenu);
    connect(m_helpChangeLanguageAction, SIGNAL(triggered()),
            BrowserApplication::languageManager(), SLOT(chooseNewLanguage()));
    m_helpMenu->addAction(m_helpChangeLanguageAction);
    m_helpMenu->addSeparator();

    m_helpAboutQtAction = new QAction(m_helpMenu);
    connect(m_helpAboutQtAction, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));
    m_helpMenu->addAction(m_helpAboutQtAction);

    m_helpAboutApplicationAction = new QAction(m_helpMenu);
    connect(m_helpAboutApplicationAction, SIGNAL(triggered()),
            this, SLOT(slotAboutApplication()));
    m_helpMenu->addAction(m_helpAboutApplicationAction);
}

void BrowserMainWindow::retranslate()
{
    m_fileMenu->setTitle(tr("&File"));
    m_fileNewWindowAction->setText(tr("&New Window"));
    m_fileOpenFileAction->setText(tr("&Open File..."));
    m_fileOpenLocationAction->setText(tr("Open &Location..."));
    m_fileSaveAsAction->setText(tr("&Save As..."));
    m_fileImportBookmarksAction->setText(tr("&Import Bookmarks..."));
    m_fileExportBookmarksAction->setText(tr("&Export Bookmarks..."));
    m_filePrintPreviewAction->setText(tr("P&rint Preview..."));
    m_filePrintAction->setText(tr("&Print..."));
    m_filePrivateBrowsingAction->setText(tr("Private &Browsing..."));
    m_fileCloseWindow->setText(tr("Close Window"));
    m_fileQuit->setText(tr("&Quit"));

    m_editMenu->setTitle(tr("&Edit"));
    m_editUndoAction->setText(tr("&Undo"));
    m_editRedoAction->setText(tr("&Redo"));
    m_editCutAction->setText(tr("Cu&t"));
    m_editCopyAction->setText(tr("&Copy"));
    m_editPasteAction->setText(tr("&Paste"));
    m_editFindAction->setText(tr("&Find"));
    m_editFindNextAction->setText(tr("Find Nex&t"));
    m_editFindPreviousAction->setText(tr("Find P&revious"));
    m_editPreferencesAction->setText(tr("Prefere&nces..."));
    m_editPreferencesAction->setShortcut(tr("Ctrl+,"));

    m_viewMenu->setTitle(tr("&View"));
    m_viewToolbarAction->setShortcut(tr("Ctrl+|"));
    m_viewBookmarkBarAction->setShortcut(tr("Shift+Ctrl+B"));
    m_viewStatusbarAction->setShortcut(tr("Ctrl+/"));
    m_viewShowMenuBarAction->setText(tr("Show Menu Bar"));
    m_viewReloadAction->setText(tr("&Reload Page"));
    m_viewStopAction->setText(tr("&Stop"));
    m_viewZoomInAction->setText(tr("Zoom &In"));
    m_viewZoomNormalAction->setText(tr("Zoom &Normal"));
    m_viewZoomOutAction->setText(tr("Zoom &Out"));
    m_viewZoomTextOnlyAction->setText(tr("Zoom &Text Only"));
    m_viewSourceAction->setText(tr("Page S&ource"));
    m_viewSourceAction->setShortcut(tr("Ctrl+Alt+U"));
    m_viewFullScreenAction->setText(tr("&Full Screen"));

    m_historyMenu->setTitle(tr("Hi&story"));
    m_historyBackAction->setText(tr("Back"));
    m_historyForwardAction->setText(tr("Forward"));
    m_historyHomeAction->setText(tr("Home"));
    m_historyRestoreLastSessionAction->setText(tr("Restore Last Session"));

    m_bookmarksMenu->setTitle(tr("&Bookmarks"));
    m_bookmarksShowAllAction->setText(tr("Manage Bookmarks..."));
    m_bookmarksAddAction->setText(tr("Add Bookmark..."));

    m_windowMenu->setTitle(tr("&Window"));

    m_toolsMenu->setTitle(tr("&Tools"));
    m_toolsWebSearchAction->setText(tr("Web &Search"));
    m_toolsWebSearchAction->setShortcut(QKeySequence(tr("Ctrl+K", "Web Search")));
    m_toolsClearPrivateDataAction->setText(tr("&Clear Private Data"));
    m_toolsClearPrivateDataAction->setShortcut(QKeySequence(tr("Ctrl+Shift+Delete", "Clear Private Data")));
    m_toolsShowNetworkMonitor->setText(tr("Show &Network Monitor"));
    m_toolsEnableInspector->setText(tr("Enable Web &Inspector"));

    m_helpMenu->setTitle(tr("&Help"));
    m_helpChangeLanguageAction->setText(tr("Switch application language "));
    m_helpAboutQtAction->setText(tr("About &Qt"));
    m_helpAboutApplicationAction->setText(tr("About &%1", "About Browser").arg(QApplication::applicationName()));

    // Toolbar
    m_navigationBar->setWindowTitle(tr("Navigation"));
    updateStatusbarActionText(m_viewStatusbarAction->isVisible());
    updateToolbarActionText(m_viewToolbarAction->isVisible());
    updateBookmarksToolbarActionText(m_viewBookmarkBarAction->isVisible());
}

void BrowserMainWindow::setupToolBar()
{
    setUnifiedTitleAndToolBarOnMac(true);
    m_navigationBar = new QToolBar(this);
    addToolBar(m_navigationBar);
    connect(m_navigationBar->toggleViewAction(), SIGNAL(toggled(bool)),
            this, SLOT(updateToolbarActionText(bool)));

    m_historyBackAction->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, this));
    m_historyBackMenu = new QMenu(this);
    m_historyBackAction->setMenu(m_historyBackMenu);
    connect(m_historyBackMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowBackMenu()));
    connect(m_historyBackMenu, SIGNAL(triggered(QAction *)),
            this, SLOT(slotOpenActionUrl(QAction *)));
    m_navigationBar->addAction(m_historyBackAction);

    m_historyForwardAction->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, this));
    m_historyForwardMenu = new QMenu(this);
    connect(m_historyForwardMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowForwardMenu()));
    connect(m_historyForwardMenu, SIGNAL(triggered(QAction *)),
            this, SLOT(slotOpenActionUrl(QAction *)));
    m_historyForwardAction->setMenu(m_historyForwardMenu);
    m_navigationBar->addAction(m_historyForwardAction);

    m_stopReloadAction = new QAction(this);
    m_reloadIcon = style()->standardIcon(QStyle::SP_BrowserReload);
    m_stopReloadAction->setIcon(m_reloadIcon);
    m_navigationBar->addAction(m_stopReloadAction);

    m_navigationSplitter = new QSplitter(m_navigationBar);
    m_navigationSplitter->addWidget(m_tabWidget->lineEditStack());

    m_toolbarSearch = new ToolbarSearch(m_navigationBar);
    m_navigationSplitter->addWidget(m_toolbarSearch);
    connect(m_toolbarSearch, SIGNAL(search(const QUrl&)),
            m_tabWidget, SLOT(loadUrl(const QUrl&)));
    m_navigationSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_tabWidget->lineEditStack()->setMinimumWidth(120);
    m_navigationSplitter->setCollapsible(0, false);
    m_navigationBar->addWidget(m_navigationSplitter);
    int splitterWidth = m_navigationSplitter->width();
    QList<int> sizes;
    sizes << (int)((double)splitterWidth * .80) << (int)((double)splitterWidth * .20);
    m_navigationSplitter->setSizes(sizes);
}

void BrowserMainWindow::slotShowBookmarksDialog()
{
    BookmarksDialog *dialog = new BookmarksDialog(this);
    connect(dialog, SIGNAL(openUrl(const QUrl&, TabWidget::OpenUrlIn, const QString &)),
            m_tabWidget, SLOT(loadUrl(const QUrl&, TabWidget::OpenUrlIn, const QString &)));
    dialog->show();
}

void BrowserMainWindow::slotAddBookmark()
{
    WebView *webView = currentTab();
    QString url = QLatin1String(webView->url().toEncoded());
    QString title = webView->title();

    AddBookmarkDialog dialog(url, title);
    BookmarkNode *menu = BrowserApplication::bookmarksManager()->menu();
    QModelIndex index = BrowserApplication::bookmarksManager()->bookmarksModel()->index(menu);
    dialog.setCurrentIndex(index);
    dialog.exec();
}

void BrowserMainWindow::slotViewMenuBar()
{
    menuBar()->setVisible(!menuBar()->isVisible());

    m_menuBarVisible = menuBar()->isVisible();
}

void BrowserMainWindow::slotViewToolbar()
{
    if (m_navigationBar->isVisible()) {
        updateToolbarActionText(false);
        m_navigationBar->close();
    } else {
        updateToolbarActionText(true);
        m_navigationBar->show();
    }
    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::slotViewBookmarksBar()
{
    if (m_bookmarksToolbar->isVisible()) {
        updateBookmarksToolbarActionText(false);
        m_bookmarksToolbar->hide();
#if defined(Q_WS_MAC)
        m_bookmarksToolbarFrame->hide();
#endif
    } else {
        updateBookmarksToolbarActionText(true);
        m_bookmarksToolbar->show();
#if defined(Q_WS_MAC)
        m_bookmarksToolbarFrame->show();
#endif
    }
    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::updateStatusbarActionText(bool visible)
{
    m_viewStatusbarAction->setText(!visible ? tr("Show Status Bar") : tr("Hide Status Bar"));
}

void BrowserMainWindow::updateToolbarActionText(bool visible)
{
    m_viewToolbarAction->setText(!visible ? tr("Show Toolbar") : tr("Hide Toolbar"));
}

void BrowserMainWindow::updateBookmarksToolbarActionText(bool visible)
{
    m_viewBookmarkBarAction->setText(!visible ? tr("Show Bookmarks Bar") : tr("Hide Bookmarks Bar"));
}

void BrowserMainWindow::slotViewStatusbar()
{
    if (statusBar()->isVisible()) {
        updateStatusbarActionText(false);
        statusBar()->close();
    } else {
        updateStatusbarActionText(true);
        statusBar()->show();
    }

    m_statusBarVisible = statusBar()->isVisible();

    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::slotDownloadManager()
{
    BrowserApplication::downloadManager()->show();
}

void BrowserMainWindow::slotSelectLineEdit()
{
    m_tabWidget->currentLineEdit()->selectAll();
    m_tabWidget->currentLineEdit()->setFocus();
}

void BrowserMainWindow::slotFileSaveAs()
{
    BrowserApplication::downloadManager()->download(currentTab()->url(), true);
}

void BrowserMainWindow::slotPreferences()
{
    SettingsDialog settingsDialog(this);
    settingsDialog.exec();
}

void BrowserMainWindow::slotUpdateStatusbar(const QString &string)
{
    statusBar()->showMessage(string, 2000);
}

void BrowserMainWindow::slotUpdateWindowTitle(const QString &title)
{
    if (title.isEmpty()) {
        setWindowTitle(QApplication::applicationName());
    } else {
#if defined(Q_WS_MAC)
        setWindowTitle(title);
#else
        setWindowTitle(tr("%1 - Arora", "Page title and Browser name").arg(title));
#endif
    }
}

void BrowserMainWindow::slotAboutApplication()
{
    AboutDialog *aboutDialog = new AboutDialog(this);
    aboutDialog->show();
}

void BrowserMainWindow::slotFileNew()
{
    BrowserApplication::instance()->newMainWindow();
    BrowserMainWindow *mw = BrowserApplication::instance()->mainWindow();
    mw->slotHome();
}

void BrowserMainWindow::slotFileOpen()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
                   tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

    if (file.isEmpty())
        return;

    tabWidget()->loadString(file);
}

void BrowserMainWindow::slotFilePrintPreview()
{
    if (!currentTab())
        return;
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(this);
    connect(dialog, SIGNAL(paintRequested(QPrinter *)),
            currentTab(), SLOT(print(QPrinter *)));
    dialog->exec();
}

void BrowserMainWindow::slotFilePrint()
{
    if (!currentTab())
        return;
    printRequested(currentTab()->page()->mainFrame());
}

void BrowserMainWindow::printRequested(QWebFrame *frame)
{
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    frame->print(&printer);
}

void BrowserMainWindow::slotPrivateBrowsing()
{
    if (!BrowserApplication::isPrivate()) {
        QString title = tr("Are you sure you want to turn on private browsing?");
        QString text = tr("<b>%1</b><br><br>When private browsing is turned on,"
            " some actions concerning your privacy will be disabled:"
            "<ul><li> Webpages are not added to the history.</li>"
            "<li> Items are automatically removed from the Downloads window.</li>"
            "<li> New cookies are not stored, current cookies can't be accessed.</li>"
            "<li> Site icons won't be stored, session won't be saved.</li>"
            "<li> Searches are not addded to the pop-up menu in the search box.</li></ul>"
            "Until you close the window, you can still click the Back and Forward buttons"
            " to return to the webpages you have opened.").arg(title);

        QMessageBox::StandardButton button = QMessageBox::question(this, QString(), text,
                               QMessageBox::Ok | QMessageBox::Cancel,
                               QMessageBox::Ok);
        if (button == QMessageBox::Ok) {
            BrowserApplication::setPrivate(true);
        } else {
            m_filePrivateBrowsingAction->setChecked(false);
        }
    } else {
        BrowserApplication::setPrivate(false);
    }
}

void BrowserMainWindow::slotZoomTextOnlyChanged(bool textOnly)
{
    m_viewZoomTextOnlyAction->setChecked(textOnly);
}

void BrowserMainWindow::slotPrivacyChanged(bool isPrivate)
{
    m_filePrivateBrowsingAction->setChecked(isPrivate);
    if (!isPrivate)
        tabWidget()->clear();
}

void BrowserMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        QSettings settings;
        settings.beginGroup(QLatin1String("tabs"));
        bool confirm = settings.value(QLatin1String("confirmClosingMultipleTabs"), true).toBool();
        if (confirm) {
            int ret = QMessageBox::warning(this, QString(),
                                           tr("Are you sure you want to close the window?"
                                              "  There are %1 tabs open").arg(m_tabWidget->count()),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if (ret == QMessageBox::No) {
                event->ignore();
                return;
            }
        }
    }
    event->accept();
    deleteLater();
}

void BrowserMainWindow::mousePressEvent(QMouseEvent *event)
{
    switch(event->button()) {
    case Qt::XButton1:
        m_historyBackAction->activate(QAction::Trigger);
        break;
    case Qt::XButton2:
        m_historyForwardAction->activate(QAction::Trigger);
        break;
    default:
        QMainWindow::mousePressEvent(event);
        break;
    }
}

void BrowserMainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QMainWindow::changeEvent(event);
}

void BrowserMainWindow::slotEditFind()
{
    tabWidget()->webViewSearch(m_tabWidget->currentIndex())->showFind();
}

void BrowserMainWindow::slotEditFindNext()
{
    tabWidget()->webViewSearch(m_tabWidget->currentIndex())->findNext();
}

void BrowserMainWindow::slotEditFindPrevious()
{
    tabWidget()->webViewSearch(m_tabWidget->currentIndex())->findPrevious();
}

void BrowserMainWindow::slotZoomIn()
{
    if (!currentTab())
        return;
    currentTab()->zoomIn();
}

void BrowserMainWindow::slotZoomNormal()
{
    if (!currentTab())
        return;
    currentTab()->resetZoom();
}

void BrowserMainWindow::slotZoomOut()
{
    if (!currentTab())
        return;
    currentTab()->zoomOut();
}

void BrowserMainWindow::slotViewFullScreen(bool makeFullScreen)
{
    if (makeFullScreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);

        menuBar()->hide();
        statusBar()->hide();
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);

        menuBar()->setVisible(m_menuBarVisible);
        statusBar()->setVisible(m_statusBarVisible);
    }
}

void BrowserMainWindow::slotViewPageSource()
{
    if (!currentTab())
        return;

    QString title = currentTab()->title();
    QString markup = currentTab()->page()->mainFrame()->toHtml();
    QUrl url = currentTab()->url();
    SourceViewer *viewer = new SourceViewer(markup, title, url, this);
    viewer->setAttribute(Qt::WA_DeleteOnClose);
    viewer->show();
}

void BrowserMainWindow::slotHome()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    QString home = settings.value(QLatin1String("home"), QLatin1String("http://www.arora-browser.org")).toString();
    tabWidget()->loadString(home);
}

void BrowserMainWindow::slotWebSearch()
{
    m_toolbarSearch->selectAll();
    m_toolbarSearch->setFocus();
}

void BrowserMainWindow::slotClearPrivateData()
{
    ClearPrivateData dialog;
    dialog.exec();
}

void BrowserMainWindow::slotToggleInspector(bool enable)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, enable);
    if (enable) {
        int result = QMessageBox::question(this, tr("Web Inspector"),
                                           tr("The web inspector will only work correctly for pages that were loaded after enabling.\n"
                                              "Do you want to reload all pages?"),
                                           QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            m_tabWidget->reloadAllTabs();
        }
    }
    QSettings settings;
    settings.beginGroup(QLatin1String("websettings"));
    settings.setValue(QLatin1String("enableInspector"), enable);
}

void BrowserMainWindow::slotSwapFocus()
{
    if (currentTab()->hasFocus()) {
        m_tabWidget->currentLineEdit()->setFocus();
        m_tabWidget->currentLineEdit()->selectAll();
    } else {
        currentTab()->setFocus();
    }
}

TabWidget *BrowserMainWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserMainWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

ToolbarSearch *BrowserMainWindow::toolbarSearch() const
{
    return m_toolbarSearch;
}

void BrowserMainWindow::slotLoadProgress(int progress)
{
    if (progress < 100 && progress > 0) {
        disconnect(m_stopReloadAction, SIGNAL(triggered()), m_viewReloadAction, SLOT(trigger()));
        if (m_stopIcon.isNull())
            m_stopIcon = style()->standardIcon(QStyle::SP_BrowserStop);
        m_stopReloadAction->setIcon(m_stopIcon);
        connect(m_stopReloadAction, SIGNAL(triggered()), m_viewStopAction, SLOT(trigger()));
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
    } else {
        disconnect(m_stopReloadAction, SIGNAL(triggered()), m_viewStopAction, SLOT(trigger()));
        m_stopReloadAction->setIcon(m_reloadIcon);
        connect(m_stopReloadAction, SIGNAL(triggered()), m_viewReloadAction, SLOT(trigger()));
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
    }
}

void BrowserMainWindow::slotAboutToShowBackMenu()
{
    m_historyBackMenu->clear();
    if (!currentTab())
        return;
    QWebHistory *history = currentTab()->history();
    int historyCount = history->count();
    for (int i = history->backItems(historyCount).count() - 1; i >= 0; --i) {
        QWebHistoryItem item = history->backItems(history->count()).at(i);
        QAction *action = new QAction(this);
        action->setData(-1*(historyCount - i - 1));
        QIcon icon = BrowserApplication::instance()->icon(item.url());
        action->setIcon(icon);
        action->setText(item.title());
        m_historyBackMenu->addAction(action);
    }
}

void BrowserMainWindow::slotAboutToShowForwardMenu()
{
    m_historyForwardMenu->clear();
    if (!currentTab())
        return;
    QWebHistory *history = currentTab()->history();
    int historyCount = history->count();
    for (int i = 0; i < history->forwardItems(history->count()).count(); ++i) {
        QWebHistoryItem item = history->forwardItems(historyCount).at(i);
        QAction *action = new QAction(this);
        action->setData(historyCount - i);
        QIcon icon = BrowserApplication::instance()->icon(item.url());
        action->setIcon(icon);
        action->setText(item.title());
        m_historyForwardMenu->addAction(action);
    }
}

void BrowserMainWindow::slotAboutToShowWindowMenu()
{
    m_windowMenu->clear();
    m_windowMenu->addAction(m_tabWidget->nextTabAction());
    m_windowMenu->addAction(m_tabWidget->previousTabAction());
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(tr("Downloads"), this, SLOT(slotDownloadManager()), QKeySequence(tr("Ctrl+Y", "Download Manager")));

    m_windowMenu->addSeparator();
    QList<BrowserMainWindow*> windows = BrowserApplication::instance()->mainWindows();
    for (int i = 0; i < windows.count(); ++i) {
        BrowserMainWindow *window = windows.at(i);
        QAction *action = m_windowMenu->addAction(window->windowTitle(), this, SLOT(slotShowWindow()));
        action->setData(i);
        action->setCheckable(true);
        if (window == this)
            action->setChecked(true);
    }
}

void BrowserMainWindow::slotShowWindow()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QVariant v = action->data();
        if (v.canConvert<int>()) {
            int offset = qvariant_cast<int>(v);
            QList<BrowserMainWindow*> windows = BrowserApplication::instance()->mainWindows();
            windows.at(offset)->activateWindow();
            windows.at(offset)->raise();
            windows.at(offset)->currentTab()->setFocus();
        }
    }
}

void BrowserMainWindow::slotOpenActionUrl(QAction *action)
{
    int offset = action->data().toInt();
    QWebHistory *history = currentTab()->history();
    if (offset < 0)
        history->goToItem(history->backItems(-1*offset).first()); // back
    else if (offset > 0)
        history->goToItem(history->forwardItems(history->count() - offset + 1).back()); // forward
}

void BrowserMainWindow::geometryChangeRequested(const QRect &geometry)
{
    setGeometry(geometry);
}

void BrowserMainWindow::showNetworkMonitor()
{
    NetworkMonitor *monitor = NetworkMonitor::self();
    monitor->show();
}

