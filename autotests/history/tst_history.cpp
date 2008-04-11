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
#include "qtest_arora.h"
#include <history.h>
#include <modeltest.h>

class tst_History : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void history_data();
    void history();
    void addHistoryItem_data();
    void addHistoryItem();
    void updateHistoryItem_data();
    void updateHistoryItem();
    void historyLimit_data();
    void historyLimit();
    void clear_data();
    void clear();
    void setHistory_data();
    void setHistory();
    void saveload_data();
    void saveload();

    void big();

    void historyDialog_data();
    void historyDialog();

private:
    QList<HistoryItem> bigHistory;
};

// Subclass that exposes the protected functions.
class SubHistory : public HistoryManager
{
public:
    ~SubHistory() {
        setHistoryLimit(30);
    }

    void addHistoryItem(const HistoryItem &item)
        { HistoryManager::addHistoryItem(item); }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_History::initTestCase()
{
    QCoreApplication::setApplicationName("historytest");

    QFile file("myhistory.txt");
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "couldn't open file:" << file.fileName();
        return;
    }
    QTextStream stream(&file);

    QList<HistoryItem> list;
    do {
        QString url = stream.readLine();
        QString title = stream.readLine();
        QString date = stream.readLine();
        QDateTime dateTime = QDateTime::fromString(date);
        QCOMPARE(dateTime.toString(), date);
        HistoryItem item(url, dateTime, title);
        list.prepend(item);
    } while (!stream.atEnd());
    bigHistory = list;
}


// This will be called after the last test function is executed.
// It is only called once.
void tst_History::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_History::init()
{
}

// This will be called after every test function.
void tst_History::cleanup()
{
}

void tst_History::history_data()
{
}

void tst_History::history()
{
    SubHistory history;
    history.addHistoryItem(HistoryItem());
    history.clear();
    QCOMPARE(history.historyLimit(), 30);
    history.setHistoryLimit(-1);
    history.updateHistoryItem(QUrl(), QString());
}

typedef QList<HistoryItem> HistoryList;
Q_DECLARE_METATYPE(HistoryList)
Q_DECLARE_METATYPE(HistoryItem)
void tst_History::addHistoryItem_data()
{
    QTest::addColumn<HistoryList>("initial");
    QTest::addColumn<HistoryList>("items");
    QTest::addColumn<HistoryList>("expected");
    HistoryList empty;
    HistoryItem item1("http://foo.com", QDateTime::currentDateTime().addDays(-3));
    HistoryList one;
    one << item1;
    HistoryList one2;
    one2 << item1 << item1;

    QTest::newRow("0-1") << empty << (HistoryList() << item1) << one;
    QTest::newRow("1-0") << one << HistoryList() << one;
    QTest::newRow("1-1") << one << (HistoryList() << item1) << one2;

    HistoryItem item2("http://bar.com", QDateTime::currentDateTime().addDays(-2));
    HistoryItem item2n("http://bar.com", QDateTime::currentDateTime());

    HistoryList two = one;
    two.prepend(item2);
    HistoryList two2 = one;
    two2.prepend(item2);
    two2.prepend(item2);
    QTest::newRow("1-2") << one << (HistoryList() << item2) << two;
    QTest::newRow("2-2,2") << two << (HistoryList() << item2) << two2;

    HistoryList swap;
    swap << item2 << item1;
    QTest::newRow("2-2,1") << one << (HistoryList() << item2) << swap;
/*
    // move to test for the historyFilterModel
    HistoryItem item3("http://baz.com", QDateTime::currentDateTime().addDays(-1));
    HistoryItem item3n("http://baz.com", QDateTime::currentDateTime());
    QTest::newRow("move-1") << (HistoryList() << item3 << item2 << item1)
                             << (HistoryList() << item3)
                             <<  (HistoryList() << item3 << item2 << item1);

    QTest::newRow("move-2") << (HistoryList() << item3 << item2 << item1)
                             << (HistoryList() << item2n)
                             <<  (HistoryList() << item2n << item3 << item1);

    QTest::newRow("move-3") << (HistoryList())
                             << (HistoryList() << item1 << item2 << item3 << item2n << item3n)
                             <<  (HistoryList() << item3n << item2n << item1);
    */
}

// public void addHistoryItem(HistoryItem* item)
void tst_History::addHistoryItem()
{
    QFETCH(HistoryList, initial);
    QFETCH(HistoryList, items);
    QFETCH(HistoryList, expected);

    SubHistory history;
    history.setHistory(initial);
    for (int i = 0; i < items.count(); ++i)
        history.addHistoryItem(items[i]);
    QCOMPARE(history.history().count(), expected.count());
    QCOMPARE(history.history(), expected);
}

void tst_History::updateHistoryItem_data()
{
    QTest::addColumn<HistoryList>("list");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("title");

    QTest::newRow("null") << HistoryList() << QUrl() << QString();
    QTest::newRow("one") << (HistoryList() << HistoryItem()) << QUrl() << QString("foo");
    QTest::newRow("two") << (HistoryList() << HistoryItem() << HistoryItem("http://foo.com")) << QUrl() << QString("foo");
}

// public void updateHistoryItem(QUrl const& url, QString const title)
void tst_History::updateHistoryItem()
{
    QFETCH(HistoryList, list);
    QFETCH(QUrl, url);
    QFETCH(QString, title);

    SubHistory history;
    history.setHistory(list);
    history.updateHistoryItem(url, title);
    if (list.isEmpty())
        QVERIFY(history.history().isEmpty());
    else
        QVERIFY(history.history() != list);
}

void tst_History::historyLimit_data()
{
    QTest::addColumn<HistoryList>("list");
    QTest::addColumn<int>("historyLimit");
    QTest::addColumn<int>("wait_seconds");
    QTest::addColumn<HistoryList>("post");

    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now;
    yesterday.setDate(yesterday.date().addDays(-1));
    yesterday.setTime(yesterday.time().addSecs(2));

    HistoryItem fooNow("http://foo.com", now);
    HistoryItem barYesterday("http://bar.com", yesterday);
    QTest::newRow("two") << (HistoryList() << fooNow << barYesterday) << 1 << 3 << (HistoryList() << fooNow);

    yesterday.setTime(yesterday.time().addSecs(3));
    barYesterday.dateTime = yesterday;
    HistoryItem barYesterday2("http://bar2.com", yesterday);
    QTest::newRow("three") << (HistoryList() << fooNow << barYesterday << barYesterday2) << 1 << 3 << (HistoryList() << fooNow);
}

// public int historyLimit() const
void tst_History::historyLimit()
{
    QFETCH(HistoryList, list);
    QFETCH(int, historyLimit);
    QFETCH(int, wait_seconds);
    QFETCH(HistoryList, post);

    SubHistory history;

    history.setHistory(list);
    qSort(list.begin(), list.end());
    QCOMPARE(history.history(), list);

    history.setHistoryLimit(historyLimit);
    QCOMPARE(history.historyLimit(), historyLimit);

    QTest::qWait(wait_seconds * 1000);
    QCOMPARE(history.history(), post);

    // re-add the items that have probably expired to catch any cache issues
    for (int i = 0; i < list.count(); ++i) {
        HistoryItem item = list.at(i);
        item.dateTime = QDateTime::currentDateTime();
        history.addHistoryItem(item);
    }
}

void tst_History::clear_data()
{
    QTest::addColumn<HistoryList>("list");

    QTest::newRow("null") << HistoryList();
    QTest::newRow("one") << (HistoryList() << HistoryItem());
    QTest::newRow("two") << (HistoryList() << HistoryItem() << HistoryItem("http://foo.com"));
}

// public void clear()
void tst_History::clear()
{
    QFETCH(HistoryList, list);
    {
        SubHistory history;
        QSignalSpy spy(&history, SIGNAL(historyReset()));
        history.setHistory(list);
        history.clear();
        QCOMPARE(history.history().count(), 0);
        QCOMPARE(spy.count(), 2);
    }
    {
        SubHistory history;
        QCOMPARE(history.history().count(), 0);
    }
}

void tst_History::setHistory_data()
{
    QTest::addColumn<HistoryList>("list");
    QTest::addColumn<HistoryList>("post");

    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now;
    yesterday.setDate(yesterday.date().addDays(-1));

    QTest::newRow("empty") << HistoryList() << HistoryList();

    HistoryItem foo("http://foo.com", now);
    HistoryItem bar("http://bar.com", yesterday);
    QTest::newRow("sort") << (HistoryList() << bar << foo) << (HistoryList() << foo << bar);

//    QTest::newRow("dupe-1") << (HistoryList() << bar << bar) << (HistoryList() << bar);
//    QTest::newRow("dupe-2") << (HistoryList() << bar << bar << foo) << (HistoryList() << foo << bar);

    QDateTime longAgo = now;
    longAgo.setDate(longAgo.date().addYears(-1));
    HistoryItem expired("http://junk.com", longAgo);
    QTest::newRow("removeExpired-1") << (HistoryList() << expired) << HistoryList();
    QTest::newRow("removeExpired-2") << (HistoryList() << foo << expired) << (HistoryList() << foo);
}

void tst_History::setHistory()
{
    QFETCH(HistoryList, list);
    QFETCH(HistoryList, post);

    SubHistory history;

    history.setHistory(list);
    QCOMPARE(history.history(), post);
}

void tst_History::saveload_data()
{
    QTest::addColumn<HistoryList>("list");
    QTest::addColumn<HistoryList>("post");

    QDateTime now = QDateTime::currentDateTime();
    QDateTime yesterday = now;
    yesterday.setDate(yesterday.date().addDays(-1));

    QTest::newRow("empty") << HistoryList() << HistoryList();

    HistoryItem foo("http://foo.com", now);
    QTest::newRow("one item") << (HistoryList() << foo) << (HistoryList() << foo);

    HistoryItem bar("http://bar.com", yesterday);
    QTest::newRow("two items") << (HistoryList() << bar << foo) << (HistoryList() << foo << bar);

    QTest::newRow("dupe-1") << (HistoryList() << bar << bar) << (HistoryList() << bar);
    QTest::newRow("dupe-2") << (HistoryList() << bar << bar << foo) << (HistoryList() << foo << bar);

    QDateTime longAgo = now;
    longAgo.setDate(longAgo.date().addYears(-1));
    HistoryItem expired("http://junk.com", longAgo);
    QTest::newRow("removeExpired-1") << (HistoryList() << expired) << HistoryList();
    QTest::newRow("removeExpired-2") << (HistoryList() << foo << expired) << (HistoryList() << foo);
}

void tst_History::saveload()
{
    QFETCH(HistoryList, list);
    QFETCH(HistoryList, post);

    {
        SubHistory history;
        history.setHistory(list);
    }
    {
        SubHistory history;
        QCOMPARE(history.history(), post);
        // add url
        HistoryItem foo("http://new.com", QDateTime::currentDateTime().addDays(1));
        post.prepend(foo);
        history.addHistoryItem(foo);
    }

    {
        SubHistory history;
        QCOMPARE(history.history(), post);
    }
}

void tst_History::big()
{
    SubHistory history;
    history.setHistoryLimit(-1);
    history.setHistory(bigHistory);


    qDebug() << "removed dups:" << history.history().count();
    QCOMPARE(history.history().count(), bigHistory.count());

    qDebug() << "loaded, making menu";
    HistoryMenu menu;
    menu.show();

    qDebug() << "menu done, making model";
    HistoryModel model(&history);
    ModelTest test(&model);
    qDebug() << "model rowCount" << model.rowCount();
    QCOMPARE(model.rowCount(), bigHistory.count());

    qDebug() << "making completion model";
    HistoryCompletionModel completionModel;
    completionModel.setSourceModel(&model);
    ModelTest test2(&completionModel);
    QCOMPARE(completionModel.rowCount(), bigHistory.count() * 2);

    qDebug() << "making history dialog model";
    HistoryTreeModel dialogModel(&model);
    ModelTest test3(&dialogModel);


    int r = 0;
    QDate d;
    for (int i = 0; i < bigHistory.count(); ++i)
        if (bigHistory[i].dateTime.date() != d) {
            d = bigHistory[i].dateTime.date();
            QDate rowDate = dialogModel.index(r, 0).data(HistoryModel::DateRole).toDate();
            QCOMPARE(d, rowDate);
            r++;
        }
    QCOMPARE(dialogModel.rowCount(), 328);

    HistoryDialog dialog(0, &history);
    dialog.show();
    QTest::qWait(100);
}

void tst_History::historyDialog_data()
{
    QTest::addColumn<int>("parentRow");
    QTest::addColumn<int>("parentColumn");

    QTest::addColumn<int>("row");
    QTest::addColumn<int>("column");

    QTest::addColumn<int>("datesDiff");
    QTest::addColumn<int>("parentRowsDiff");

    QTest::newRow("page-c") << 0 << 0  << 0 << 1  << 0 << -1;
    QTest::newRow("page-0") << 0 << 0  << 0 << 0  << 0 << -1;
    QTest::newRow("page-1") << 0 << 0  << 1 << 0  << 0 << -1;
    QTest::newRow("page-2") << 0 << 0  << 2 << 0  << 0 << -1;
    QTest::newRow("page-3") << 0 << 0  << 3 << 0  << 0 << -1;
    QTest::newRow("page-4") << 0 << 0  << 4 << 0  << 0 << -1;
    QTest::newRow("page-last") << 0 << 0  << -2 << 0  << 0 << -1;
    QTest::newRow("page-only") << 32 << 0  << 0 << 0  << -1 << -1;

    QTest::newRow("date-c") << -1 << -1  << 0 << 1  << -1 << 0;
    QTest::newRow("date-0") << -1 << -1  << 0 << 0  << -1 << 0;
    QTest::newRow("date-1") << -1 << -1  << 1 << 0  << -1 << 0;
    QTest::newRow("date-2") << -1 << -1  << 2 << 0  << -1 << 0;
    QTest::newRow("date-3") << -1 << -1  << 3 << 0  << -1 << 0;
    QTest::newRow("date-last") << -1 << -1  << -2 << 0  << -1 << 0;

    QTest::newRow("removeAll") << -1 << -1  << -3 << 0  << -1 << 0;
}

void tst_History::historyDialog()
{
    QFETCH(int, parentRow);
    QFETCH(int, parentColumn);
    QFETCH(int, row);
    QFETCH(int, column);
    QFETCH(int, datesDiff);
    QFETCH(int, parentRowsDiff);

   qDebug() << "which one";
    SubHistory history;
    history.setHistoryLimit(-1);
    history.setHistory(bigHistory);
    HistoryDialog dialog(0, &history);
    dialog.show();
    //QTest::qWait(300);

    QAbstractItemModel *model = dialog.tree->model();
/*
    for (int i = 0; i < model->rowCount(); ++i)
        if (model->rowCount(model->index(i, 0)) == 1)
            qDebug() << i;
*/
    ModelTest test(model);
    if (parentRow == -2) parentRow = model->rowCount() - 1;
    QModelIndex parent = model->index(parentRow, parentColumn);
    if (row < -1) row = model->rowCount(parent) - 1;
    QModelIndex child = model->index(row, column, parent);
    QVERIFY(child.isValid());

    int topRowCount = model->rowCount();
    int parentRowCount = model->rowCount(parent.sibling(parent.row(), 0));
    int childRowCount = model->rowCount(child.sibling(child.row(), 0));
    dialog.tree->setCurrentIndex(child);
    if (row == -3) {
        dialog.tree->removeAll();
        QCOMPARE(model->rowCount(), 0);
        QModelIndex idx = dialog.tree->indexAt(QPoint(10, 10));
        QVERIFY(!idx.isValid());
    } else {
        dialog.tree->removeOne();
        QCOMPARE(history.history().count(), bigHistory.count() + parentRowsDiff + (childRowCount * datesDiff));
        QCOMPARE(model->rowCount(), topRowCount + datesDiff);
        if (parent.isValid() && datesDiff == 0)
            QCOMPARE(model->rowCount(parent), parentRowCount + parentRowsDiff);
    }
}

QTEST_MAIN(tst_History)
#include "tst_history.moc"
