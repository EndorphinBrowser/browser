/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#ifndef SEARCHBUTTON_H
#define SEARCHBUTTON_H

#include <qabstractbutton.h>

class QCompleter;
class SearchButton : public QAbstractButton
{
    Q_OBJECT

public:
    SearchButton(QWidget *parent = 0);
    void setImage(const QImage &image);
    void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    QImage generateSearchImage(bool dropDown);
    QCompleter *hasCompleter() const;
    QImage m_cache;
};

#endif // SEARCHBUTTON_H

