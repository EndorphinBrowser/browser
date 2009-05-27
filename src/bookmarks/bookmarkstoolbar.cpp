/*
 * Copyright 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "bookmarkstoolbar.h"

#include "addbookmarkdialog.h"
#include "bookmarknode.h"
#include "bookmarksmanager.h"
#include "bookmarksmenu.h"
#include "bookmarksmodel.h"
#include "bookmarktoolbutton.h"
#include "browserapplication.h"
#include "modelmenu.h"

#include <qevent.h>

BookmarksToolBar::BookmarksToolBar(BookmarksModel *model, QWidget *parent)
    : QToolBar(tr("Bookmark"), parent)
    , m_bookmarksModel(model)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(contextMenuRequested(const QPoint &)));

    setRootIndex(model->index(BrowserApplication::bookmarksManager()->toolbar()));
    connect(m_bookmarksModel, SIGNAL(modelReset()), this, SLOT(build()));
    connect(m_bookmarksModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(build()));
    connect(m_bookmarksModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(build()));
    connect(m_bookmarksModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(build()));
    setAcceptDrops(true);
}

Q_DECLARE_METATYPE(QModelIndex)
void BookmarksToolBar::contextMenuRequested(const QPoint &position)
{
    QToolButton *button = qobject_cast<QToolButton*>(childAt(position));

    QMenu menu;
    QAction *action;

    if (button) {
        QModelIndex index;
        QVariant variant;

        BookmarkToolButton *bookmarkButton = qobject_cast<BookmarkToolButton*>(button);
        ModelMenu *modelMenu = qobject_cast<ModelMenu*>(button->menu());
        if (modelMenu) {
            index = modelMenu->rootIndex();
            variant.setValue(index);
        } else if (bookmarkButton) {
            index = bookmarkButton->index();
            variant.setValue(index);

            action = menu.addAction(tr("Open"), this, SLOT(openBookmark()));
            action->setData(variant);

            action = menu.addAction(tr("Open in New &Tab"), this, SLOT(openBookmarkInNewTab()));
            action->setData(variant);

            menu.addSeparator();
        }

        if (variant.isValid()) {
            action = menu.addAction(tr("Remove"), this, SLOT(removeBookmark()));
            action->setData(variant);
        }

        menu.addSeparator();
    }

    action = menu.addAction(tr("Add Bookmark..."), this, SLOT(newBookmark()));

    action = menu.addAction(tr("Add Folder..."), this, SLOT(newFolder()));

    menu.exec(QCursor::pos());
}

void BookmarksToolBar::openBookmark()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QVariant variant = action->data();
    if (!variant.canConvert<QModelIndex>())
        return;
    QModelIndex index = qvariant_cast<QModelIndex>(variant);

    emit openUrl(
        index.data(BookmarksModel::UrlRole).toUrl(),
        TabWidget::CurrentTab,
        index.data(Qt::DisplayRole).toString());
}

void BookmarksToolBar::openBookmarkInNewTab()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QVariant variant = action->data();
    if (!variant.canConvert<QModelIndex>())
        return;
    QModelIndex index = qvariant_cast<QModelIndex>(variant);

    emit openUrl(
        index.data(BookmarksModel::UrlRole).toUrl(),
        TabWidget::NewTab,
        index.data(Qt::DisplayRole).toString());
}

void BookmarksToolBar::removeBookmark()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QVariant variant = action->data();
    if (!variant.canConvert<QModelIndex>())
        return;
    QModelIndex index = qvariant_cast<QModelIndex>(variant);

    m_bookmarksModel->removeRow(index.row(), m_root);
}

void BookmarksToolBar::newBookmark()
{
    AddBookmarkDialog dialog;
    BookmarkNode *toolbar = BrowserApplication::bookmarksManager()->toolbar();
    QModelIndex index = m_bookmarksModel->index(toolbar);
    dialog.setCurrentIndex(index);
    dialog.exec();
}

void BookmarksToolBar::newFolder()
{
    BookmarkNode *toolbar = BrowserApplication::bookmarksManager()->toolbar();
    QModelIndex index = m_bookmarksModel->index(toolbar);

    AddBookmarkDialog dialog;
    dialog.setCurrentIndex(index);
    dialog.setFolder(true);
    dialog.exec();
}

void BookmarksToolBar::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls())
        event->acceptProposedAction();
    QToolBar::dragEnterEvent(event);
}

void BookmarksToolBar::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls() && mimeData->hasText()) {
        QList<QUrl> urls = mimeData->urls();
        QString title = mimeData->text();
        QUrl url = urls.at(0);

        if (url.isEmpty()) {
            QToolBar::dropEvent(event);
            return;
        }

        if (title.isEmpty()) {
            title = QLatin1String(url.toEncoded());
        }

        int row = -1;
        QModelIndex parentIndex = m_root;
        QToolButton *target = qobject_cast<QToolButton*>(childAt(event->pos()));

        if (target && target->menu()) {
            ModelMenu *menu = qobject_cast<ModelMenu*>(target->menu());
            if (menu)
                parentIndex = menu->rootIndex();
        }

        BookmarkNode *bookmark = new BookmarkNode(BookmarkNode::Bookmark);
        bookmark->url = QLatin1String(url.toEncoded());
        bookmark->title = title;

        BookmarkNode *parent = m_bookmarksModel->node(parentIndex);
        BookmarksManager *bookmarksManager = m_bookmarksModel->bookmarksManager();
        bookmarksManager->addBookmark(parent, bookmark, row);
        event->acceptProposedAction();
    }
    QToolBar::dropEvent(event);
}


void BookmarksToolBar::setRootIndex(const QModelIndex &index)
{
    m_root = index;
    build();
}

QModelIndex BookmarksToolBar::rootIndex() const
{
    return m_root;
}

void BookmarksToolBar::build()
{
    clear();
    for (int i = 0; i < m_bookmarksModel->rowCount(m_root); ++i) {
        QModelIndex idx = m_bookmarksModel->index(i, 0, m_root);
        if (m_bookmarksModel->hasChildren(idx)) {
            QToolButton *button = new QToolButton(this);
            button->setPopupMode(QToolButton::InstantPopup);
            button->setArrowType(Qt::DownArrow);
            button->setText(idx.data().toString());
            ModelMenu *menu = new BookmarksMenu(this);
            menu->setModel(m_bookmarksModel);
            menu->setRootIndex(idx);
            menu->addAction(new QAction(menu));
            button->setMenu(menu);
            button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            QAction *a = addWidget(button);
            a->setText(idx.data().toString());
            connect(menu, SIGNAL(openUrl(const QUrl &, const QString &)),
                    this, SIGNAL(openUrl(const QUrl &, const QString &)));
            connect(menu, SIGNAL(openUrl(const QUrl&, TabWidget::OpenUrlIn, const QString&)),
                    this, SIGNAL(openUrl(const QUrl&, TabWidget::OpenUrlIn, const QString&)));
        } else {
            BookmarkToolButton *button = new BookmarkToolButton(idx, this);
            button->setText(idx.data().toString());
            connect(button, SIGNAL(openBookmark(const QUrl &, const QString &)),
                    this, SIGNAL(openUrl(const QUrl &, const QString &)));
            addWidget(button);
        }
    }
}

