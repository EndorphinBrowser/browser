/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "edittreeview.h"

#include <qevent.h>

EditTreeView::EditTreeView(QWidget *parent)
    : QTreeView(parent)
{
}

void EditTreeView::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Delete
         || event->key() == Qt::Key_Backspace)
        && model()) {
        removeSelected();
    } else {
        QAbstractItemView::keyPressEvent(event);
    }
}

void EditTreeView::removeSelected()
{
    if (!model() || !selectionModel())
        return;

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    QModelIndex first = selectedRows.value(0);
    // get parent before removing first
    QModelIndex firstParent = first.parent();
    for (int i = selectedRows.count() - 1; i >= 0; --i) {
        QModelIndex idx = selectedRows.at(i);
        model()->removeRow(idx.row(), idx.parent());
    }
    // select the item at the same position
    QModelIndex idx = model()->index(first.row(), 0, firstParent);
    // if that was the last item
    if (!idx.isValid()) {
        int parentRows = model()->rowCount(firstParent);
        if (parentRows == 0) {
            // removed the only item, goto parent
            idx = firstParent;
        } else {
            // removed the last item, goto new last item
            idx = model()->index(parentRows - 1, 0, firstParent);
        }
    }
    if (!idx.isValid())
        idx = model()->index(0, 0, rootIndex());
    selectionModel()->select(idx, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    setCurrentIndex(idx);
}

void EditTreeView::removeAll()
{
    if (!model())
        return;
    model()->removeRows(0, model()->rowCount(rootIndex()), rootIndex());
}

