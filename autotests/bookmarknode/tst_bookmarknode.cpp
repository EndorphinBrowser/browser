/*
 * Copyright (c) 2020 Aaron Dewes <aaron.dewes@web.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Endorphin nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <QtTest/QtTest>
#include <bookmarknode.h>
#include <tst_bookmarknode.h>

// This will be called before the first test function is executed.
// It is only called once.
void tst_BookmarkNode::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_BookmarkNode::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_BookmarkNode::init()
{
}

// This will be called after every test function.
void tst_BookmarkNode::cleanup()
{
}

void tst_BookmarkNode::bookmarknode_data()
{
}

void tst_BookmarkNode::bookmarknode()
{
    BookmarkNode node;
    QCOMPARE(node.children(), QList<BookmarkNode*>());
    QCOMPARE(node.parent(), (BookmarkNode*)0);
    QCOMPARE(node.type(), BookmarkNode::Root);
    node.add((BookmarkNode*)0, -1);
    node.remove((BookmarkNode*)0);
}

void tst_BookmarkNode::add_to_bookmark()
{
    BookmarkNode node(BookmarkNode::Bookmark);
    BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
    node.add(child);
    // qWarning()?
    QCOMPARE(node.children().count(), 0);
    QCOMPARE(child->parent(), (BookmarkNode*)0);
    delete child;
}

void tst_BookmarkNode::add_duplicate()
{
    BookmarkNode node;
    BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
    node.add(child);
    node.add(child);
    QCOMPARE(node.children().count(), 1);
}

void tst_BookmarkNode::add_move()
{
    BookmarkNode node1;
    BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
    node1.add(child);

    BookmarkNode node2;
    node2.add(child);

    QCOMPARE(child->parent(), &node2);
    QVERIFY(node2.children().contains(child));
    QVERIFY(!node1.children().contains(child));
}

void tst_BookmarkNode::add_data()
{
    QTest::addColumn<int>("startcount");
    QTest::addColumn<int>("insertoffset");
    QTest::addColumn<int>("resultingoffset");
    QTest::newRow("0,-1,0") << 0 << -1 << 0;
    QTest::newRow("0,0,0")  << 0 << 0  << 0;
    QTest::newRow("0,1,0")  << 0 << 1  << 0;
    QTest::newRow("1,-1,1") << 1 << -1 << 1;
    QTest::newRow("1,0,0")  << 1 << 0  << 0;
    QTest::newRow("1,1,0")  << 1 << 1  << 1;
}

// public void add(BookmarkNode *child, int offset = -1)
void tst_BookmarkNode::add()
{
    QFETCH(int, startcount);
    QFETCH(int, insertoffset);
    QFETCH(int, resultingoffset);

    BookmarkNode node;

    for (int i = 0; i < startcount; ++i) {
        BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
        node.add(child, -1);
    }

    BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
    node.add(child, insertoffset);

    QCOMPARE(child->parent(), &node);
    QList<BookmarkNode*> children = node.children();
    QVERIFY(children.contains(child));
    QCOMPARE(children.at(resultingoffset), child);
}

Q_DECLARE_METATYPE(BookmarkNode)
// public bool equal(BookmarkNode const &other)
void tst_BookmarkNode::equal()
{
    BookmarkNode null1;
    BookmarkNode null2;
    QCOMPARE(null1, null2);

    BookmarkNode b1(BookmarkNode::Bookmark);
    BookmarkNode b2(BookmarkNode::Bookmark);
    QCOMPARE(b1, b2);

    b1.title = "title";
    QVERIFY(!(b1 == b2));
    b2.title = "title";
    QCOMPARE(b1, b2);

    b1.url = "url";
    QVERIFY(!(b1 == b2));
    b2.url = "url";
    QCOMPARE(b1, b2);

    b1.desc = "description";
    QVERIFY(!(b1 == b2));
    b2.desc = "description";
    QCOMPARE(b1, b2);

    BookmarkNode f1(BookmarkNode::Folder);
    BookmarkNode f2(BookmarkNode::Folder);
    QCOMPARE(f1, f2);

    f1.expanded = true;
    QVERIFY(!(f1 == f2));
    f2.expanded = true;
    QCOMPARE(f1, f2);

    BookmarkNode *child1 = new BookmarkNode(BookmarkNode::Bookmark);
    child1->title = "title";
    f1.add(child1);
    QVERIFY(!(f1 == f2));

    BookmarkNode *child2 = new BookmarkNode(BookmarkNode::Bookmark);
    child2->title = "title";
    f2.add(child2);
    QCOMPARE(f1, f2);
}

// public void remove(BookmarkNode *child)
void tst_BookmarkNode::remove()
{
    BookmarkNode node;
    BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
    node.add(child);
    node.remove(child);
    QCOMPARE(node.children().count(), 0);
    QCOMPARE(child->parent(), (BookmarkNode*)0);
    delete child;
}

void tst_BookmarkNode::deleteNode()
{
    BookmarkNode node;
    BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
    node.add(child);

    delete child;

    QCOMPARE(node.children().count(), 0);
    QCOMPARE(child->parent(), (BookmarkNode*)0);
}

void tst_BookmarkNode::deleteChildren()
{
    BookmarkNode *node = new BookmarkNode();
    for (int i = 0; i < 10; ++i) {
        BookmarkNode *child = new BookmarkNode(BookmarkNode::Bookmark);
        node->add(child, -1);
    }
    delete node;
    // This shouldn't segfualt :)
}

Q_DECLARE_METATYPE(BookmarkNode::Type)
void tst_BookmarkNode::type_data()
{
    QTest::addColumn<BookmarkNode::Type>("type");
    QTest::newRow("root") << BookmarkNode::Root;
    QTest::newRow("folder") << BookmarkNode::Folder;
    QTest::newRow("sep") << BookmarkNode::Separator;
    QTest::newRow("bookmark") << BookmarkNode::Bookmark;
}

// public BookmarkNode::Type type() const
void tst_BookmarkNode::type()
{
    QFETCH(BookmarkNode::Type, type);

    BookmarkNode node(type);

    QCOMPARE(node.type(), type);
    if (node.type() == BookmarkNode::Root)
        QVERIFY(!node.parent());
    if (node.type() == BookmarkNode::Bookmark)
        QVERIFY(node.children().isEmpty());
}

QTEST_MAIN(tst_BookmarkNode)
