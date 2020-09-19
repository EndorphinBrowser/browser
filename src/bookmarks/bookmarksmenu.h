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

#ifndef BOOKMARKSMENU_H
#define BOOKMARKSMENU_H

#include "modelmenu.h"

#include "tabwidget.h"

#include <qabstractitemmodel.h>
#include <qlist.h>

class BookmarksManager;

// Base class for BookmarksMenuBarMenu and BookmarksToolBarMenu
class BookmarksMenu : public ModelMenu
{
    Q_OBJECT

signals:
    void openUrl(const QUrl &url, const QString &title);
    void openUrl(const QUrl &url, TabWidget::OpenUrlIn tab, const QString &title);

public:
    BookmarksMenu(QWidget *parent = 0);

protected:
    void postPopulated();
    ModelMenu *createBaseMenu();

private slots:
    void openAll();
    void activated(const QModelIndex &index);

};

// Menu that is dynamically populated from the bookmarks
class BookmarksMenuBarMenu : public BookmarksMenu
{
    Q_OBJECT

public:
    BookmarksMenuBarMenu(QWidget *parent = 0);
    void setInitialActions(QList<QAction*> actions);

protected:
    bool prePopulated();

private:
    BookmarksManager *m_bookmarksManager;
    QList<QAction*> m_initialActions;
};

#endif // BOOKMARKSMENU_H
