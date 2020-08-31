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

#include "mainapp.h"
#include <QWebEnginePage>
#include <QEventLoop>
#include <QDebug>
#include <QTimer>
#include <QFile>

MainApp::MainApp(QObject *parent) : QObject(parent)
{
}

QString result;

QString MainApp::get(QString &input)
{
    QWebEnginePage webPage;
    QEventLoop loop;
    connect(&webPage, &QWebEnginePage::loadFinished, &loop, &QEventLoop::quit);
    webPage.setHtml(input);
    loop.exec();
    QEventLoop loop2;
    QObject::connect(this, SIGNAL(notifyRanJavaScript()), &loop2, SLOT(quit()));
    QFile jsFile(":/extract.js");
    if (!jsFile.open(QFile::ReadOnly)) {
        qWarning() << "Unable to load javascript to extract bookmarks.";
        exit(1);
    }
    webPage.runJavaScript(jsFile.readAll(), [this](const QVariant &v)
    {
        QString tmp = v.toString();
        this->ranJavaScript(tmp);
    });
    loop2.exec();
    return result;
}

void MainApp::ranJavaScript(QString &value)
{
    result = value;
    emit notifyRanJavaScript();
}