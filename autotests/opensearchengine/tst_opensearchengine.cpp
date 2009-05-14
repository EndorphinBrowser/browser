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

#include "opensearchengine.h"
#include "qtest_arora.h"

#include <qnetworkaccessmanager.h>

class tst_OpenSearchEngine : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void openSearchEngine();
    void imageLoading();
    void requestSuggestions();
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_OpenSearchEngine::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_OpenSearchEngine::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_OpenSearchEngine::init()
{
}

// This will be called after every test function.
void tst_OpenSearchEngine::cleanup()
{
}

// Subclass that exposes the protected functions.
class SubOpenSearchEngine : public OpenSearchEngine
{
public:
    void call_imageChanged()
        { return SubOpenSearchEngine::imageChanged(); }

    void call_loadImage()
        { return SubOpenSearchEngine::loadImage(); }

    QString call_parseTemplate(QString const &searchTerm, QString templ) const
        { return SubOpenSearchEngine::parseTemplate(searchTerm, templ); }

    void call_suggestions(QStringList const &suggestions)
        { return SubOpenSearchEngine::suggestions(suggestions); }
};

void tst_OpenSearchEngine::openSearchEngine()
{
    SubOpenSearchEngine engine;
    QCOMPARE(engine.description(), QString());
    QCOMPARE(engine.image(), QImage());
    QCOMPARE(engine.imageUrl(), QString());
    QCOMPARE(engine.isValid(), false);
    QCOMPARE(engine.name(), QString());
    QCOMPARE(engine.operator<(OpenSearchEngine()), false);
    QCOMPARE(engine.operator==(OpenSearchEngine()), true);
    QCOMPARE(engine.providesSuggestions(), false);
    engine.requestSuggestions(QString());
    QCOMPARE(engine.searchParameters(), QList<OpenSearchEngine::Parameter>());
    QCOMPARE(engine.searchUrlTemplate(), QString());
    QCOMPARE(engine.searchUrl(QString()), QUrl());
    engine.setDescription(QString());
    engine.setImage(QImage());
    engine.setImageUrl(QString());
    engine.setName(QString());
    engine.setSearchParameters(QList<OpenSearchEngine::Parameter>());
    engine.setSearchUrlTemplate(QString());
    engine.setSuggestionsParameters(QList<OpenSearchEngine::Parameter>());
    engine.setSuggestionsUrlTemplate(QString());
    QCOMPARE(engine.suggestionsParameters(), QList<OpenSearchEngine::Parameter>());
    QCOMPARE(engine.suggestionsUrl(QString()), QUrl());
    QCOMPARE(engine.suggestionsUrlTemplate(), QString());
    engine.call_imageChanged();
    engine.call_loadImage();
    QCOMPARE(engine.call_parseTemplate(QString(), QString()), QString());
    engine.call_suggestions(QStringList());
}

void tst_OpenSearchEngine::imageLoading()
{
    OpenSearchEngine engine;

    QPixmap image(1, 1);
    image.fill();

    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    image.save(&imageBuffer, "PNG");

    QSignalSpy signalSpy(&engine, SIGNAL(imageChanged()));
    engine.setImageUrl(QString("data:image/png;base64,").append(imageBuffer.buffer().toBase64()));
    engine.image();

    QTRY_COMPARE(signalSpy.count(), 1);
}

void tst_OpenSearchEngine::requestSuggestions()
{
    OpenSearchEngine engine;
    engine.setSuggestionsUrlTemplate("x");
    engine.requestSuggestions("foo");
}

QTEST_MAIN(tst_OpenSearchEngine)

#include "tst_opensearchengine.moc"

