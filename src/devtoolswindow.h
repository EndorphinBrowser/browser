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

#ifndef DEVTOOLSWINDOW_H
#define DEVTOOLSWINDOW_H

#include <qmainwindow.h>
#include <QWebEngineView>

class QWebEnginePage;
/*!
    The MainWindow of the Browser Application.

    Handles the tab widget and all the actions
 */
class DevToolsWindow : public QMainWindow
{
    Q_OBJECT

public:
    DevToolsWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = nullptr);
    ~DevToolsWindow();
    QWebEnginePage *page();
    QSize sizeHint() const;

private slots:
    void geometryChangeRequested(const QRect &geometry);

private:
    QWebEngineView *m_webView;
};

#endif // DEVTOOLSWINDOW_H

