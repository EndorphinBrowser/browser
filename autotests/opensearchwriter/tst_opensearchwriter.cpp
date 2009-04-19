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

#include "opensearchwriter.h"
#include "opensearchengine.h"

class tst_OpenSearchWriter : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void engineToByteArray();
    void engineToFile();
    void engineWithUrlParameters();
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_OpenSearchWriter::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_OpenSearchWriter::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_OpenSearchWriter::init()
{
}

// This will be called after every test function.
void tst_OpenSearchWriter::cleanup()
{
}

void tst_OpenSearchWriter::engineToByteArray()
{
    OpenSearchEngine engine;
    QByteArray array;
    OpenSearchWriter writer;

    engine.setName(QLatin1String("Foo Bar"));
    engine.setDescription(QLatin1String("Bar Foo"));
    engine.setSearchUrl(QLatin1String("http://foobar.barfoo/search"));

    writer.write(&array, &engine);

    QFile expected(QLatin1String("expected1.xml"));
    expected.open(QIODevice::ReadOnly);

    QCOMPARE(array, expected.readAll());
}

void tst_OpenSearchWriter::engineToFile()
{
    OpenSearchEngine engine;
    QFile file(QLatin1String("result2.xml"));
    OpenSearchWriter writer;

    engine.setName(QLatin1String("Arora!"));
    engine.setDescription(QLatin1String("a cross platform web browser built using Qt and WebKit"));
    engine.setSearchUrl(QLatin1String("http://foobar.barfoo/search"));
    engine.setSuggestionsUrl(QLatin1String("http://foobar.barfoo/suggest"));

    writer.write(&file, &engine);

    file.close();
    file.open(QIODevice::ReadOnly);

    QFile expected(QLatin1String("expected2.xml"));
    expected.open(QIODevice::ReadOnly);

    QCOMPARE(file.readAll(), expected.readAll());
}

void tst_OpenSearchWriter::engineWithUrlParameters()
{
    OpenSearchEngine engine;
    QByteArray array;
    OpenSearchWriter writer;

    engine.setName(QLatin1String("Foo Bar"));
    engine.setDescription(QLatin1String("Bar Foo"));
    engine.setSearchUrl(QLatin1String("http://foobar.barfoo/search"));
    engine.setSuggestionsUrl(QLatin1String("http://foobar.barfoo/suggest"));

    QHash<QString, QString> searchParameters;
    QHash<QString, QString> suggestionsParameters;

    searchParameters[QLatin1String("q")] = QLatin1String("{searchTerms}");
    searchParameters[QLatin1String("a")] = QLatin1String("foo");

    suggestionsParameters[QLatin1String("q")] = QLatin1String("{searchTerms}");

    engine.setSearchParameters(searchParameters);
    engine.setSuggestionsParameters(suggestionsParameters);

    writer.write(&array, &engine);

    QFile expected(QLatin1String("expected3.xml"));
    expected.open(QIODevice::ReadOnly);

    QCOMPARE(array, expected.readAll());
}

QTEST_MAIN(tst_OpenSearchWriter)

#include "tst_opensearchwriter.moc"
