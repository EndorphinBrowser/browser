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

#include "qtest_arora.h"

#include "opensearchengine.h"
#include "opensearchmanager.h"

class tst_OpenSearchManager : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void addRemoveEngine_data();
    void addRemoveEngine();
    void setCurrentEngine_data();
    void setCurrentEngine();
    void generateEngineFileName_data();
    void generateEngineFileName();
    void restoreDefaults();
};

class SubOpenSearchManager : public OpenSearchManager
{
public:
    QString generateEngineFileName(const QString &engineName)
    {
        return OpenSearchManager::generateEngineFileName(engineName);
    }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_OpenSearchManager::initTestCase()
{
    QCoreApplication::setApplicationName("opensearchtest");

    SubOpenSearchManager manager;

    foreach (const QString &name, manager.allEnginesNames()) {
        manager.removeEngine(name);
    }

    // Never let the manager have no engines.
    QCOMPARE(manager.enginesCount(), 1);
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_OpenSearchManager::cleanupTestCase()
{
    SubOpenSearchManager manager;

    QCOMPARE(manager.enginesCount(), 1);
}

// This will be called before each test function is executed.
void tst_OpenSearchManager::init()
{
}

// This will be called after every test function.
void tst_OpenSearchManager::cleanup()
{
}

void tst_OpenSearchManager::addRemoveEngine_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("searchUrl");
    QTest::addColumn<bool>("valid");

    QTest::newRow("valid") << "Foo" << "Bar" << "http://foobaz.bar" << true;
    QTest::newRow("invalid") << "Baz" << "Foo" << "" << false;
}

void tst_OpenSearchManager::addRemoveEngine()
{
    QFETCH(QString, name);
    QFETCH(QString, description);
    QFETCH(QString, searchUrl);
    QFETCH(bool, valid);

    SubOpenSearchManager manager;

    QSignalSpy signalSpy(&manager, SIGNAL(changed()));

    OpenSearchEngine *engine = new OpenSearchEngine();
    engine->setName(name);
    engine->setDescription(description);
    engine->setSearchUrl(searchUrl);

    QCOMPARE(manager.enginesCount(), 1);
    QVERIFY(!manager.engineExists(name));

    bool result = manager.addEngine(engine);

    QCOMPARE(result, valid);
    QCOMPARE(manager.enginesCount(), (valid ? 2 : 1));
    QCOMPARE(manager.engineExists(name), valid);
    QCOMPARE(signalSpy.count(), (valid ? 1 : 0));

    manager.removeEngine(engine->name());

    QCOMPARE(manager.enginesCount(), 1);
    QVERIFY(!manager.engineExists(name));
    QCOMPARE(signalSpy.count(), (valid ? 2 : 0));
}

void tst_OpenSearchManager::setCurrentEngine_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("searchUrl");
    QTest::addColumn<bool>("valid");

    QTest::newRow("valid") << "Foo" << "Bar" << "http://foobaz.bar" << true;
    QTest::newRow("invalid") << "Baz" << "Foo" << "" << false;
}

void tst_OpenSearchManager::setCurrentEngine()
{
    QFETCH(QString, name);
    QFETCH(QString, description);
    QFETCH(QString, searchUrl);
    QFETCH(bool, valid);

    SubOpenSearchManager manager;

    QCOMPARE(manager.enginesCount(), 1);

    QString oldCurrentName = manager.currentName();
    OpenSearchEngine *oldCurrentEngine = manager.currentEngine();

    QSignalSpy signalSpy(&manager, SIGNAL(currentChanged()));

    OpenSearchEngine *engine = new OpenSearchEngine();
    engine->setName(name);
    engine->setDescription(description);
    engine->setSearchUrl(searchUrl);

    bool result = manager.addEngine(engine);
    QCOMPARE(result, valid);

    manager.setCurrentName(name);
    QCOMPARE(manager.currentName(), (valid ? name : oldCurrentName));
    QCOMPARE(*manager.currentEngine(), (valid ? *engine : *oldCurrentEngine));
    QCOMPARE(signalSpy.count(), (valid ? 1 : 0));

    manager.removeEngine(engine->name());
    QCOMPARE(signalSpy.count(), (valid ? 2 : 0));
}

void tst_OpenSearchManager::generateEngineFileName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("fileName");

    QTest::newRow("simple") << "FooBar" << "FooBar.xml";
    QTest::newRow("with-spaces") << "Foo Bar" << "Foo_Bar.xml";
    QTest::newRow("with-special-chars") << ":Foo&Bar*Baz-" << "FooBarBaz.xml";
    QTest::newRow("with-special-chars-and-spaces") << ": Foo & Bar -" << "_Foo__Bar_.xml";
}

void tst_OpenSearchManager::generateEngineFileName()
{
    QFETCH(QString, name);
    QFETCH(QString, fileName);

    SubOpenSearchManager manager;

    QCOMPARE(manager.generateEngineFileName(name), fileName);
}

void tst_OpenSearchManager::restoreDefaults()
{
    SubOpenSearchManager manager;

    QCOMPARE(manager.enginesCount(), 1);
    manager.restoreDefaults();
    QCOMPARE(manager.enginesCount(), 5);

    foreach (const QString &name, manager.allEnginesNames()) {
        manager.removeEngine(name);
    }

    // Never let the manager have no engines.
    QCOMPARE(manager.enginesCount(), 1);

    OpenSearchEngine *engine = new OpenSearchEngine();
    engine->setName("Foobarbaz");
    engine->setSearchUrl("http://foobarbaz.baz");

    manager.addEngine(engine);
    manager.restoreDefaults();
    QCOMPARE(manager.enginesCount(), 6);

    manager.removeEngine(engine->name());

    foreach (const QString &name, manager.allEnginesNames()) {
        manager.removeEngine(name);
    }

    // Never let the manager have no engines.
    QCOMPARE(manager.enginesCount(), 1);
}

QTEST_MAIN(tst_OpenSearchManager)

#include "tst_opensearchmanager.moc"

