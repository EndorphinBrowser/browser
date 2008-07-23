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

#ifndef LINEEDIT_P_H
#define LINEEDIT_P_H

#include <qwidget.h>

class SideWidget : public QWidget
{
    Q_OBJECT

signals:
    void sizeHintChanged();

public:
    SideWidget(QWidget *parent = 0);

protected:
    bool event(QEvent *event);

};

#if QT_VERSION < 0x040500

#include "qapplication.h"
#include "proxystyle.h"
#include "lineedit.h"

class LineEditStyle : public ProxyStyle
{

public:
    explicit LineEditStyle()
        : ProxyStyle(QApplication::style()->objectName()) {
    }

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = 0) const {
        QRect r = ProxyStyle::subElementRect(element, option, widget);
        if (element == SE_LineEditContents) {
            if (const LineEdit *le = qobject_cast<const LineEdit *>(widget)) {
                int left = le->textMargin(LineEdit::LeftSide);
                int right = le->textMargin(LineEdit::RightSide);
                r.adjust(left, 0, -right, 0);
            }
        }
        return r;
    }

};

#endif // QT_VERSION

#endif // LINEEDIT_P_H

