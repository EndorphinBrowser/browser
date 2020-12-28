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

#ifndef SEARCHBUTTON_H
#define SEARCHBUTTON_H

#include <QAbstractButton>

class QCompleter;
class SearchButton : public QAbstractButton
{
    Q_OBJECT

public:
    SearchButton(QWidget *parent = nullptr);
    void setImage(const QImage &image);
    void setShowMenuTriangle(bool show);
    bool showMenuTriangle() const;
    void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;

private:
    QImage generateSearchImage(bool dropDown);
    QImage m_cache;
    bool m_showMenuTriangle;
};

#endif // SEARCHBUTTON_H

