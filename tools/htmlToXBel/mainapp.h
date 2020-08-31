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

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QObject>
#include <QVariant>

class QWebEngineView;
class QPushButton;

class MainApp : public QObject
{
Q_OBJECT
public:
    MainApp(QObject *parent = nullptr);
    QString get(QString &input);
    void ranJavaScript(QString &value);

Q_SIGNALS:
    void notifyRanJavaScript();

};

#endif // MAINAPP_H