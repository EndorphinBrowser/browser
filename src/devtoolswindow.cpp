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

#include "devtoolswindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QScreen>

DevToolsWindow::DevToolsWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_webView(new QWebEngineView())
{
    setCentralWidget(m_webView);
}

DevToolsWindow::~DevToolsWindow()
{
}

void DevToolsWindow::geometryChangeRequested(const QRect &geometry)
{
    setGeometry(geometry);
}

QWebEnginePage *DevToolsWindow::page()
{
    return m_webView->page();
}

QSize DevToolsWindow::sizeHint() const
{
    QRect desktopRect = qApp->primaryScreen()->geometry();
    QSize size = desktopRect.size() * 0.4;
    return size;
}
