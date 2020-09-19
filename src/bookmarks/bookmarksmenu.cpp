/*
 * Copyright 2020 Aaron Dewes <aaron.dewes@web.de>
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
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
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

#include "bookmarksmenu.h"

#include "bookmarknode.h"
#include "bookmarksmanager.h"
#include "bookmarksmodel.h"
#ifndef NO_BROWSERAPPLICATION
#include "browserapplication.h"
#else
#ifdef FOR_AUTOTEST
#include "tst_addbookmarkdialog.h"
#endif
#endif

BookmarksMenu::BookmarksMenu(QWidget *parent)
    : ModelMenu(parent)
{
    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(activated(const QModelIndex &)));
    setStatusBarTextRole(BookmarksModel::UrlStringRole);
    setSeparatorRole(BookmarksModel::SeparatorRole);
}

ModelMenu *BookmarksMenu::createBaseMenu()
{
    BookmarksMenu *menu = new BookmarksMenu(this);
    connect(menu, SIGNAL(openUrl(const QUrl&, TabWidget::OpenUrlIn, const QString&)),
            this, SIGNAL(openUrl(const QUrl&, TabWidget::OpenUrlIn, const QString&)));
    return menu;
}

void BookmarksMenu::activated(const QModelIndex &index)
{
    emit openUrl(index.data(BookmarksModel::UrlRole).toUrl(),
                 index.data(Qt::DisplayRole).toString());
}

void BookmarksMenu::postPopulated()
{
    if (isEmpty())
        return;

    QModelIndex parent = rootIndex();

    bool hasBookmarks = false;

    for (int i = 0; i < parent.model()->rowCount(parent); ++i) {
        QModelIndex child = parent.model()->index(i, 0, parent);

        if (child.data(BookmarksModel::TypeRole) == BookmarkNode::Bookmark) {
            hasBookmarks = true;
            break;
        }
    }

    if (!hasBookmarks)
        return;

    addSeparator();
    QAction *action = addAction(tr("Open in Tabs"));
    connect(action, SIGNAL(triggered()),
            this, SLOT(openAll()));
}

void BookmarksMenu::openAll()
{
    ModelMenu *menu = qobject_cast<ModelMenu*>(sender()->parent());
    if (!menu)
        return;
    QModelIndex parent = menu->rootIndex();
    if (!parent.isValid())
        return;
    for (int i = 0; i < parent.model()->rowCount(parent); ++i) {
        QModelIndex child = parent.model()->index(i, 0, parent);

        if (child.data(BookmarksModel::TypeRole) != BookmarkNode::Bookmark)
            continue;

        TabWidget::OpenUrlIn tab;
        tab = (i == 0) ? TabWidget::CurrentTab : TabWidget::NewTab;
        emit openUrl(child.data(BookmarksModel::UrlRole).toUrl(),
                     tab,
                     child.data(Qt::DisplayRole).toString());
    }
}

BookmarksMenuBarMenu::BookmarksMenuBarMenu(QWidget *parent)
    : BookmarksMenu(parent)
    , m_bookmarksManager(nullptr)
{
}

bool BookmarksMenuBarMenu::prePopulated()
{
#ifndef NO_BROWSERAPPLICATION
    m_bookmarksManager = BrowserApplication::bookmarksManager();
#else
    #ifdef FOR_AUTOTEST
    m_bookmarksManager = tst_AddBookmarkDialog::bookmarksManager();
    #else
        #error "Nothing provides a bookmarksmanager"
    #endif
#endif
    setModel(m_bookmarksManager->bookmarksModel());
    setRootIndex(m_bookmarksManager->bookmarksModel()->index(m_bookmarksManager->menu()));
    // initial actions
    for (int i = 0; i < m_initialActions.count(); ++i)
        addAction(m_initialActions.at(i));
    if (!m_initialActions.isEmpty())
        addSeparator();
    createMenu(m_bookmarksManager->bookmarksModel()->index(m_bookmarksManager->toolbar()), 1, this);
    return true;
}

void BookmarksMenuBarMenu::setInitialActions(QList<QAction*> actions)
{
    m_initialActions = actions;
    for (int i = 0; i < m_initialActions.count(); ++i)
        addAction(m_initialActions.at(i));
}
