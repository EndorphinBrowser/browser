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

#include "bookmarktoolbutton.h"

#include "bookmarksmanager.h"
#include "bookmarksmodel.h"
#include "browserapplication.h"

#include <qaction.h>
#include <qevent.h>

BookmarkToolButton::BookmarkToolButton(QWidget *parent)
    : QToolButton(parent)
{
}

void BookmarkToolButton::mouseReleaseEvent(QMouseEvent *event)
{
    BrowserApplication::instance()->setEventMouseButtons(event->button());
    BrowserApplication::instance()->setEventKeyboardModifiers(event->modifiers());

    if (event->button() == Qt::LeftButton) {
        QToolButton::mouseReleaseEvent(event);
    } else if (event->button() == Qt::MidButton) {
        QAction *action = defaultAction();
        if (action)
            action->trigger();
    }
}
