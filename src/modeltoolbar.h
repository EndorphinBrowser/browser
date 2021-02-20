/*
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

#ifndef MODELTOOLBAR_H
#define MODELTOOLBAR_H

#include <QToolBar>

#include <QAbstractItemModel>

Q_DECLARE_METATYPE(QModelIndex)

class QEvent;
class ModelMenu;
class ModelToolBar : public QToolBar
{
    Q_OBJECT

Q_SIGNALS:
    void activated(const QModelIndex &index);

public:
    ModelToolBar(QWidget *parent = nullptr);
    ModelToolBar(const QString &title, QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setRootIndex(const QModelIndex &index);
    QModelIndex rootIndex() const;

    static QModelIndex index(QAction *action);

protected:
    virtual ModelMenu *createMenu();

    bool eventFilter(QObject *object, QEvent *event);

    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected Q_SLOTS:
    void build();

private:
    QAbstractItemModel *m_model;
    QPersistentModelIndex m_rootIndex;
    QPoint m_dragStartPos;
};

#endif // MODELTOOLBAR_H
