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

#include <QtTest/QtTest>
#include <QtGui/QtGui>
#include <searchlineedit.h>

class tst_SearchLineEdit : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void searchlineedit_data();
    void searchlineedit();

    void inactiveText_data();
    void inactiveText();

    void menu();
    void setMenu();
    void resizeEvent();
};

// Subclass that exposes the protected functions.
class SubSearchLineEdit : public SearchLineEdit
{
public:
    void call_paintEvent(QPaintEvent* event)
        { return SubSearchLineEdit::paintEvent(event); }

    void call_resizeEvent(QResizeEvent* event)
        { return SubSearchLineEdit::resizeEvent(event); }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_SearchLineEdit::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_SearchLineEdit::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_SearchLineEdit::init()
{
}

// This will be called after every test function.
void tst_SearchLineEdit::cleanup()
{
}

void tst_SearchLineEdit::searchlineedit_data()
{
}

void tst_SearchLineEdit::searchlineedit()
{
    SubSearchLineEdit edit;
    QCOMPARE(edit.inactiveText(), tr("Search"));
    edit.setInactiveText(QString());
    edit.setMenu((QMenu*)0);
    QVERIFY(edit.menu() != 0);
}

void tst_SearchLineEdit::inactiveText_data()
{
    QTest::addColumn<QString>("inactiveText");
    QTest::newRow("foo") << QString("foo");
}

// public QString inactiveText() const
void tst_SearchLineEdit::inactiveText()
{
    QFETCH(QString, inactiveText);

    SubSearchLineEdit edit;
    edit.setInactiveText(inactiveText);
    QCOMPARE(edit.inactiveText(), inactiveText);
}

// public QMenu* menu() const
void tst_SearchLineEdit::menu()
{
    SubSearchLineEdit edit;
    edit.show();
    QList<QAbstractButton *> widgets = edit.findChildren<QAbstractButton *>(QString("SearchButton"));
    QSize oldSize = widgets.at(0)->size();
    QMenu *menu = edit.menu();
    QSize newSize = widgets.at(0)->size();
    QCOMPARE(oldSize.height(), newSize.height());
    QVERIFY(oldSize.width() != newSize.width());

    QCOMPARE(edit.menu(), menu);
    oldSize = newSize;
    newSize = widgets.at(0)->size();
    QCOMPARE(oldSize.height(), newSize.height());
    QCOMPARE(oldSize.width(), newSize.width());
}

// public void setMenu(QMenu* menu)
void tst_SearchLineEdit::setMenu()
{
    SubSearchLineEdit edit;
    edit.menu();

    QMenu *newMenu = new QMenu(&edit);
    edit.setMenu(newMenu);
    QCOMPARE(edit.menu(), newMenu);
    
    edit.show();
    QList<QAbstractButton *> widgets = edit.findChildren<QAbstractButton *>(QString("SearchButton"));
    QSize oldSize = widgets.at(0)->size();
    edit.setMenu(0);
    QSize newSize = widgets.at(0)->size();
    QCOMPARE(oldSize.height(), newSize.height());
    QVERIFY(oldSize.width() != newSize.width());
}

// protected void resizeEvent(QResizeEvent* event)
void tst_SearchLineEdit::resizeEvent()
{
    SubSearchLineEdit edit;
    edit.resize(100, 100);

    QList<QAbstractButton *> widgets = edit.findChildren<QAbstractButton *>(QString("SearchButton"));
    QSize oldSize = widgets.at(0)->size();

    edit.show();
    edit.resize(200, 200);

    QSize newSize = widgets.at(0)->size();
    QVERIFY(oldSize.height() != newSize.height());
    QVERIFY(oldSize.width() != newSize.width());
}

QTEST_MAIN(tst_SearchLineEdit)
#include "tst_searchlineedit.moc"

