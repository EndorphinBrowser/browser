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

#include "privacyindicator.h"

#include "browserapplication.h"

#include <QIcon>

PrivacyIndicator::PrivacyIndicator(QWidget *parent)
    : QLabel(parent)
{
    QIcon icon = QIcon::fromTheme("view-private");
    setPixmap(icon.pixmap(icon.actualSize(QSize(16, 16))));
    connect(BrowserApplication::instance(), SIGNAL(privacyChanged(bool)),
            this, SLOT(setVisible(bool)));
    setCursor(Qt::ArrowCursor);
    setVisible(BrowserApplication::instance()->isPrivate());
}

void PrivacyIndicator::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    BrowserApplication::instance()->setPrivate(false);
}
