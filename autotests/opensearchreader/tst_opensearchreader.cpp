/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
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

#include "opensearchreader.h"
#include "opensearchengine.h"

class tst_OpenSearchReader : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private Q_SLOTS:
    void read_data();
    void read();
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_OpenSearchReader::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_OpenSearchReader::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_OpenSearchReader::init()
{
}

// This will be called after every test function.
void tst_OpenSearchReader::cleanup()
{
}

Q_DECLARE_METATYPE(OpenSearchEngine::Parameters)
void tst_OpenSearchReader::read_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("valid");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("searchUrlTemplate");
    QTest::addColumn<QString>("suggestionsUrlTemplate");
    QTest::addColumn<QString>("imageUrl");
    QTest::addColumn<OpenSearchEngine::Parameters>("searchParameters");
    QTest::addColumn<OpenSearchEngine::Parameters>("suggestionsParameters");
    QTest::addColumn<QString>("searchMethod");
    QTest::addColumn<QString>("suggestionsMethod");

    QTest::newRow("null") << QStringLiteral(":/doesNotExist") << false << QString() << QString() << QString() << QString()
            << QString() << OpenSearchEngine::Parameters() << OpenSearchEngine::Parameters() << QStringLiteral("get") << QStringLiteral("get");

    QTest::newRow("testfile1") << QStringLiteral(":/testfile1.xml") << true << QStringLiteral("Wikipedia (en)")
            << QStringLiteral("Full text search in the English Wikipedia") << QStringLiteral("http://en.wikipedia.org/bar")
            << QStringLiteral("http://en.wikipedia.org/foo") << QStringLiteral("http://en.wikipedia.org/favicon.ico")
            << OpenSearchEngine::Parameters() << OpenSearchEngine::Parameters() << QStringLiteral("post") << QStringLiteral("get");

    QTest::newRow("testfile2") << QStringLiteral(":/testfile2.xml") << false << QStringLiteral("Wikipedia (en)")
            << QString() << QString() << QStringLiteral("http://en.wikipedia.org/foo") << QStringLiteral("http://en.wikipedia.org/favicon.ico")
            << OpenSearchEngine::Parameters() << OpenSearchEngine::Parameters() << QStringLiteral("get") << QStringLiteral("get");

    QTest::newRow("testfile3") << QStringLiteral(":/testfile3.xml") << true << QStringLiteral("GitHub") << QStringLiteral("Search GitHub")
            << QStringLiteral("http://github.com/search") << QStringLiteral("http://github.com/suggestions") << QString()
            << (OpenSearchEngine::Parameters() << OpenSearchEngine::Parameter(QStringLiteral("q"), QStringLiteral("{searchTerms}"))
                                               << OpenSearchEngine::Parameter(QStringLiteral("b"), QStringLiteral("foo")))
            << (OpenSearchEngine::Parameters() << OpenSearchEngine::Parameter(QStringLiteral("bar"), QStringLiteral("baz")))
            << QStringLiteral("get") << QStringLiteral("post");

    QTest::newRow("testfile4") << QStringLiteral(":/testfile4.xml") << true << QStringLiteral("Google") << QStringLiteral("Google Web Search")
            << QStringLiteral("http://www.google.com/search?bar") << QStringLiteral("http://suggestqueries.google.com/complete/foo")
            << QStringLiteral("http://www.google.com/favicon.ico") << OpenSearchEngine::Parameters()
            << OpenSearchEngine::Parameters() << QStringLiteral("get") << QStringLiteral("get");

    QTest::newRow("testfile5") << QStringLiteral(":/testfile5.xml") << false << QString() << QString() << QString() << QString()
            << QString() << OpenSearchEngine::Parameters() << OpenSearchEngine::Parameters() << QStringLiteral("get") << QStringLiteral("get");

    QTest::newRow("testfile6") << QStringLiteral(":/testfile6.xml") << false << QString() << QString() << QString() << QString()
            << QString() << OpenSearchEngine::Parameters() << OpenSearchEngine::Parameters()  << QStringLiteral("get") << QStringLiteral("get");

    QTest::newRow("testfile7") << QStringLiteral(":/testfile7.xml") << false << QString() << QString() << QString() << QString()
            << QString() << OpenSearchEngine::Parameters() << OpenSearchEngine::Parameters()  << QStringLiteral("get") << QStringLiteral("get");
}

void tst_OpenSearchReader::read()
{
    QFETCH(QString, fileName);
    QFETCH(bool, valid);
    QFETCH(QString, name);
    QFETCH(QString, description);
    QFETCH(QString, searchUrlTemplate);
    QFETCH(QString, suggestionsUrlTemplate);
    QFETCH(QString, imageUrl);
    QFETCH(OpenSearchEngine::Parameters, searchParameters);
    QFETCH(OpenSearchEngine::Parameters, suggestionsParameters);
    QFETCH(QString, searchMethod);
    QFETCH(QString, suggestionsMethod);

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(&file);

    QCOMPARE(engine->isValid(), valid);
    QCOMPARE(engine->name(), name);
    QCOMPARE(engine->description(), description);
    QCOMPARE(engine->searchUrlTemplate(), searchUrlTemplate);
    QCOMPARE(engine->suggestionsUrlTemplate(), suggestionsUrlTemplate);
    QCOMPARE(engine->searchParameters(), searchParameters);
    QCOMPARE(engine->suggestionsParameters(), suggestionsParameters);
    QCOMPARE(engine->imageUrl(), imageUrl);
    QCOMPARE(engine->searchMethod(), searchMethod);
    QCOMPARE(engine->suggestionsMethod(), suggestionsMethod);

    delete engine;
}

QTEST_MAIN(tst_OpenSearchReader)

#include "tst_opensearchreader.moc"

