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

#include <autosaver.h>

#ifndef TST_AUTOSAVER_H
#define TST_AUTOSAVER_H

class tst_AutoSaver : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void save();

private slots:
    void AutoSaver_data();
    void AutoSaver();
    void changeOccurred_data();
    void changeOccurred();
    void deleted();
};

// Subclass that exposes the protected functions.
class SubAutoSaver : public AutoSaver
{
public:
    SubAutoSaver(QObject *parent = 0) : AutoSaver(parent) {}
    void call_timerEvent(QTimerEvent *event)
    {
        return SubAutoSaver::timerEvent(event);
    }
};

class TestClass : public QObject
{
    Q_OBJECT

signals:
    void saveCalled();

public:
    TestClass(QObject *parent = 0) : QObject(parent), AutoSaver(new SubAutoSaver(this))
    {
    }

    ~TestClass()
    {
        AutoSaver->saveIfNeccessary();
    }

    SubAutoSaver *AutoSaver;

public slots:
    void save() {
        emit saveCalled();
    }
};

#endif //TST_AUTOSAVER_H
