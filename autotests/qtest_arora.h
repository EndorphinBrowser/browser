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

#ifndef QTEST_ARORA_H
#define QTEST_ARORA_H

#include <QtTest/QTest>
#include "browserapplication.h"

#ifndef QTRY_COMPARE

#define __TRY_TIMEOUT__ 5000
#define __TRY_STEP__    50

#define __QTRY(__expression__, __functionToCall__) \
    do { \
        int __i = 0; \
        while (!(__expression__) &&  __i < __TRY_TIMEOUT__) { \
            QTest::qWait(__TRY_STEP__); \
            __i += __TRY_STEP__; \
        } \
        __functionToCall__; \
    } while(0)

#define QTRY_COMPARE(__expression__, __expected__) \
    __QTRY((__expression__ == __expected__), QCOMPARE(__expression__, __expected__));

#define QTRY_VERIFY(__expression__) \
    __QTRY(__expression__, QVERIFY(__expression__));

#endif // QTRY_COMPARE

#undef QTEST_MAIN

#define QTEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    Q_INIT_RESOURCE(htmls); \
    Q_INIT_RESOURCE(data); \
    BrowserApplication app(argc, argv); \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}

#endif

