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

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void validFile();
    void validByteArray();
    void invalidFile();
    void invalidByteArray();
    void urlParameters();
    void emptyByteArray();
    void infinitiveLoops();
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

void tst_OpenSearchReader::validFile()
{
    QFile file(QLatin1String("valid.xml"));
    OpenSearchReader reader;
    OpenSearchEngine *description = reader.read(&file);

    QVERIFY(description->isValid());
    QCOMPARE(description->name(), QLatin1String("Wikipedia (en)"));
    QCOMPARE(description->description(), QLatin1String("Full text search in the English Wikipedia"));
    QCOMPARE(description->searchUrl(), QLatin1String("http://en.wikipedia.org/bar"));
    QCOMPARE(description->suggestionsUrl(), QLatin1String("http://en.wikipedia.org/foo"));
    QCOMPARE(description->iconUrl().toString(), QLatin1String("http://en.wikipedia.org/favicon.ico"));

    delete description;
}

void tst_OpenSearchReader::validByteArray()
{
    QByteArray data;
    data.append("<OpenSearchDescription xmlns=\"http://a9.com/-/spec/opensearch/1.1/\">");
    data.append("<ShortName>Google</ShortName>");
    data.append("<Description>Google Web Search</Description>");
    data.append("<Url method=\"get\" type=\"text/html\""
                " template=\"http://www.google.com/search?bar\" />");
    data.append("<Url method=\"get\" type=\"application/x-suggestions+json\""
                " template=\"http://suggestqueries.google.com/complete/foo\" />");
    data.append("<Image width=\"16\" height=\"16\">http://www.google.com/favicon.ico</Image>");
    data.append("</OpenSearchDescription>");

    OpenSearchReader reader;
    OpenSearchEngine *description = reader.read(data);

    QVERIFY(description->isValid());
    QCOMPARE(description->name(), QLatin1String("Google"));
    QCOMPARE(description->description(), QLatin1String("Google Web Search"));
    QCOMPARE(description->searchUrl(), QLatin1String("http://www.google.com/search?bar"));
    QCOMPARE(description->suggestionsUrl(), QLatin1String("http://suggestqueries.google.com/complete/foo"));
    QCOMPARE(description->iconUrl().toString(), QLatin1String("http://www.google.com/favicon.ico"));

    delete description;
}

void tst_OpenSearchReader::invalidFile()
{
    QFile file(QLatin1String("invalid.xml"));
    OpenSearchReader reader;
    OpenSearchEngine *description = reader.read(&file);

    QVERIFY(!description->isValid());
    QCOMPARE(description->name(), QLatin1String("Wikipedia (en)"));
    QCOMPARE(description->description(), QString());

    delete description;
}

void tst_OpenSearchReader::invalidByteArray()
{
    QByteArray data;
    data.append("<OpenSearchDescription>");
    data.append("<ShortName>Google</ShortName>");
    data.append("<Description>Google Web Search</Description>");
    data.append("<Url method=\"get\" type=\"text/html\""
    " template=\"http://www.google.com/search?bar\" />");
    data.append("</OpenSearchDescription>");

    OpenSearchReader reader;
    OpenSearchEngine *description = reader.read(data);

    QVERIFY(!description->isValid()); // lacking in namespace URI
    QCOMPARE(description->name(), QString());
    QCOMPARE(description->description(), QString());
    QCOMPARE(description->searchUrl(), QString());

    delete description;
}

void tst_OpenSearchReader::urlParameters()
{
    QFile file(QLatin1String("urlparams.xml"));
    OpenSearchReader reader;
    OpenSearchEngine *description = reader.read(&file);

    QVERIFY(description->isValid());
    QCOMPARE(description->name(), QLatin1String("GitHub"));
    QCOMPARE(description->description(), QLatin1String("Search GitHub"));
    QCOMPARE(description->searchUrl(), QLatin1String("http://github.com/search"));
    QCOMPARE(description->suggestionsUrl(), QString());

    QCOMPARE(description->searchParameters().count(), 2);
    QVERIFY(description->searchParameters().contains(QLatin1String("q")));
    QCOMPARE(description->searchParameters().value(QLatin1String("q")), QLatin1String("{searchTerms}"));
    QCOMPARE(description->searchParameters().value(QLatin1String("b")), QLatin1String("foo"));

    delete description;
}

void tst_OpenSearchReader::emptyByteArray()
{
    OpenSearchReader reader;
    OpenSearchEngine *description = reader.read(QByteArray());

    QVERIFY(!description->isValid());

    delete description;
}

void tst_OpenSearchReader::infinitiveLoops()
{
    QString xml("<foo><bar></bar></foo>");
    OpenSearchReader reader;

    OpenSearchEngine *description = reader.read(xml);
    QVERIFY(!description->isValid());
}

QTEST_MAIN(tst_OpenSearchReader)

#include "tst_opensearchreader.moc"
