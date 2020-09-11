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

#include <QtTest/QtTest>
#include <autosaver.h>
#include "tst_autosaver.h"

// This will be called before the first test function is executed.
// It is only called once.
void tst_AutoSaver::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_AutoSaver::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_AutoSaver::init()
{
}

// This will be called after every test function.
void tst_AutoSaver::cleanup()
{
}

void tst_AutoSaver::AutoSaver_data()
{
}

void tst_AutoSaver::AutoSaver()
{
    SubAutoSaver save(this);
    save.changeOccurred();
    save.saveIfNeccessary();
}

void tst_AutoSaver::save()
{
}

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)
void tst_AutoSaver::changeOccurred_data()
{
    QTest::addColumn<IntList>("changed");
    QTest::addColumn<int>("saved");
    QTest::newRow("no changes") << (QList<int>()) << 0;
    QTest::newRow("1 changes") << (QList<int>() << 0) << 1;
    QTest::newRow("3 changes in a row") << (QList<int>() << 0 << 0 << 0) << 1;
    QTest::newRow("2 changes 4 secs, 1 change") << (QList<int>() << 0 << 0 << 4000) << 2;
    QTest::newRow("0, 1.5s, 2s, 12s(force save), 100m, 100m") << (QList<int>() << 0 << 1500 << 2000 << 12000 << 100 << 100) << 2;
}

// public void changeOccurred()
void tst_AutoSaver::changeOccurred()
{
    QFETCH(IntList, changed);
    QFETCH(int, saved);

    TestClass *test = new TestClass;
    QSignalSpy spy(test, SIGNAL(saveCalled()));
    for (int i = 0; i < changed.count(); ++i) {
        QTest::qWait(changed.at(i));
        test->AutoSaver->changeOccurred();
    }
    delete test;
    QCOMPARE(spy.count(), saved);
}

void tst_AutoSaver::deleted()
{
    TestClass *test = new TestClass;
    QSignalSpy spy(test, SIGNAL(saveCalled()));
    test->AutoSaver->changeOccurred();
    delete test;
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(tst_AutoSaver)
