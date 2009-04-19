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
    void imageLoading();
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

void tst_OpenSearchEngine::imageLoading()
{
    OpenSearchEngine *engine = new OpenSearchEngine();

    engine->setNetworkAccessManager(new QNetworkAccessManager());

    QPixmap image(1, 1);
    image.fill();

    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    image.save(&imageBuffer, "PNG");

    QSignalSpy signalSpy(engine, SIGNAL(imageChanged()));
    engine->setImageUrl(QUrl(QString("data:image/png;base64,").append(imageBuffer.buffer().toBase64())));

    QTest::qWait(500);

    QCOMPARE(signalSpy.count(), 1);

    delete engine;
}

QTEST_MAIN(tst_OpenSearchEngine)

#include "tst_opensearchengine.moc"
