/*
 * Copyright 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
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

Q_DECLARE_METATYPE(QModelIndex)

BookmarksToolBar::BookmarksToolBar(BookmarksModel *model, QWidget *parent)
    : QToolBar(tr("Bookmarks"), parent)
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
    setHidden(true);
}

QModelIndex BookmarksToolBar::index(QAction *action)
{
    QModelIndex index;
    if (!action)
        return index;

    QVariant variant = action->data();
    if (!variant.canConvert<QModelIndex>())
        return index;

    return qvariant_cast<QModelIndex>(variant);
}

void BookmarksToolBar::contextMenuRequested(const QPoint &position)
{
    QAction *action = actionAt(position);
    QMenu menu;

    if (action) {
        QVariant variant = action->data();
        Q_ASSERT(variant.canConvert<QModelIndex>());

        QAction *menuAction = 0;

        if (!action->menu()) {
            menuAction = menu.addAction(tr("Open"), this, SLOT(openBookmarkInCurrentTab()));
            menuAction->setData(variant);

            menuAction = menu.addAction(tr("Open in New &Tab"), this, SLOT(openBookmarkInNewTab()));
            menuAction->setData(variant);

            menu.addSeparator();
        }

        menuAction = menu.addAction(tr("Remove"), this, SLOT(removeBookmark()));
        menuAction->setData(variant);

        menu.addSeparator();
    }

    menu.addAction(tr("Add Bookmark..."), this, SLOT(newBookmark()));
    menu.addAction(tr("Add Folder..."), this, SLOT(newFolder()));

    menu.exec(QCursor::pos());
}

void BookmarksToolBar::openBookmark()
{
    QModelIndex index = this->index(qobject_cast<QAction*>(sender()));

    emit openUrl(
        index.data(BookmarksModel::UrlRole).toUrl(),
        index.data(Qt::DisplayRole).toString());
}

void BookmarksToolBar::openBookmarkInCurrentTab()
{
    QModelIndex index = this->index(qobject_cast<QAction*>(sender()));

    emit openUrl(
        index.data(BookmarksModel::UrlRole).toUrl(),
        TabWidget::CurrentTab,
        index.data(Qt::DisplayRole).toString());
}

void BookmarksToolBar::openBookmarkInNewTab()
{
    QModelIndex index = this->index(qobject_cast<QAction*>(sender()));

    emit openUrl(
        index.data(BookmarksModel::UrlRole).toUrl(),
        TabWidget::NewTab,
        index.data(Qt::DisplayRole).toString());
}

void BookmarksToolBar::removeBookmark()
{
    QModelIndex index = this->index(qobject_cast<QAction*>(sender()));

    m_bookmarksModel->removeRow(index.row(), m_root);
}

void BookmarksToolBar::newBookmark()
{
    AddBookmarkDialog dialog;
    dialog.setCurrentIndex(m_root);
    dialog.exec();
}

void BookmarksToolBar::newFolder()
{
    AddBookmarkDialog dialog;
    dialog.setCurrentIndex(m_root);
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

    if (mimeData->hasUrls()) {
        QUrl url = mimeData->urls().at(0);
        QString title = mimeData->text();

        if (url.isEmpty()) {
            QToolBar::dropEvent(event);
            return;
        }

        if (title.isEmpty())
            title = QString::fromUtf8(url.toEncoded());

        QModelIndex parentIndex = m_root;
        QAction *action = actionAt(event->pos());
        QModelIndex index = this->index(action);

        if (action && action->menu()) {
            if (action->menu()->isAncestorOf(event->source()))
                return;
            if (index.isValid())
                parentIndex = index;
        }

        BookmarkNode *node = new BookmarkNode(BookmarkNode::Bookmark);
        node->url = QString::fromUtf8(url.toEncoded());
        node->title = title;

        int row = m_bookmarksModel->rowCount(parentIndex);
        BookmarkNode *parent = m_bookmarksModel->node(parentIndex);
        BookmarksManager *bookmarksManager = m_bookmarksModel->bookmarksManager();
        bookmarksManager->addBookmark(parent, node, row);

        event->acceptProposedAction();
    }

    QToolBar::dropEvent(event);
}

void BookmarksToolBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        QToolBar::mouseMoveEvent(event);
        return;
    }

    if ((event->pos() - m_dragStartPosition).manhattanLength()
        < QApplication::startDragDistance()) {
        QToolBar::mouseMoveEvent(event);
        return;
    }

    QAction *action = actionAt(m_dragStartPosition);
    QModelIndex index = this->index(action);
    if (!index.isValid()) {
        QToolBar::mouseMoveEvent(event);
        return;
    }

    QString title = index.data().toString();
    QUrl url = index.data(BookmarksModel::UrlRole).toUrl();

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    mimeData->setText(title);
    mimeData->setUrls(QList<QUrl>() << url);
    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction);
}

void BookmarksToolBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragStartPosition = mapFromGlobal(event->globalPos());

    QToolBar::mousePressEvent(event);
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
        QVariant variant;
        variant.setValue(idx);

        QString title = idx.data().toString();
        bool folder = m_bookmarksModel->hasChildren(idx);

        QToolButton *button = 0;
        if (folder)
            button = new QToolButton(this);
        else
            button = new BookmarkToolButton(this);

        button->setPopupMode(QToolButton::InstantPopup);
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);

        QAction *action = addWidget(button);
        action->setData(variant);
        action->setText(title);
        button->setDefaultAction(action);

        if (folder) {
            button->setArrowType(Qt::DownArrow);

            ModelMenu *menu = new BookmarksMenu(this);
            menu->setModel(m_bookmarksModel);
            menu->setRootIndex(idx);
            menu->addAction(new QAction(menu));
            action->setMenu(menu);

            connect(menu, SIGNAL(openUrl(const QUrl &, const QString &)),
                    this, SIGNAL(openUrl(const QUrl &, const QString &)));
            connect(menu, SIGNAL(openUrl(const QUrl &, TabWidget::OpenUrlIn, const QString &)),
                    this, SIGNAL(openUrl(const QUrl &, TabWidget::OpenUrlIn, const QString &)));
        } else {
            connect(action, SIGNAL(triggered()),
                    this, SLOT(openBookmark()));
        }
    }
}

