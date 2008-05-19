/*
 * Copyright 2007-2008 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "aboutdialog.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qicon.h>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("About") + QString(" %1").arg(qApp->applicationName()));
    logo = new QLabel();
    logo->setPixmap(qApp->windowIcon().pixmap(128, 128));
    logo->setAlignment(Qt::AlignCenter);

    name = new QLabel(qApp->applicationName());
    QFont font = name->font();
    font.setBold(true);
    name->setFont(font);
    name->setAlignment(Qt::AlignHCenter);

    version = new QLabel();
    version->setAlignment(Qt::AlignHCenter);
    version->setText(QApplication::applicationVersion());

    layout = new QVBoxLayout();
    layout->addWidget(logo);
    layout->addWidget(name);
    layout->addWidget(version);

    layout->insertStretch(-1, 1);
    setLayout(layout);
}

void AboutDialog::addAuthors(const QStringList &list)
{
    for (int i = list.count() - 1; i >= 0; --i) {
        QLabel *author = new QLabel(list.at(i), this);
        author->setOpenExternalLinks(true);
        author->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
        author->setAlignment(Qt::AlignHCenter);
        layout->insertWidget(3, author);
    }
}

