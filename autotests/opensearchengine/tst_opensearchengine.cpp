/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#include <qtest.h>

#include "qtest_arora.h"
#include "languagemanager.h"

#include <qsignalspy.h>
#include <opensearchengine.h>
#include <qbuffer.h>

#include <qdebug.h>

class tst_OpenSearchEngine : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void opensearchengine_data();
    void opensearchengine();

    void description_data();
    void description();
    void image();
    void imageUrl_data();
    void imageUrl();
    void isValid_data();
    void isValid();
    void name_data();
    void name();
    void operatorlessthan();
    void operatorequal_data();
    void operatorequal();
    void providesSuggestions_data();
    void providesSuggestions();
    void requestSuggestions_data();
    void requestSuggestions();
    void searchParameters_data();
    void searchParameters();
    void searchUrl_data();
    void searchUrl();
    void searchUrlTemplate_data();
    void searchUrlTemplate();
    void suggestionsParameters_data();
    void suggestionsParameters();
    void suggestionsUrl_data();
    void suggestionsUrl();
    void suggestionsUrlTemplate_data();
    void suggestionsUrlTemplate();
    void parseTemplate_data();
    void parseTemplate();
};

// Subclass that exposes the protected functions.
class SubOpenSearchEngine : public OpenSearchEngine
{
public:
    void call_imageChanged()
        { return SubOpenSearchEngine::imageChanged(); }

    void call_loadImage() const
        { return SubOpenSearchEngine::loadImage(); }

    QNetworkAccessManager *call_networkAccessManager() const
        { return SubOpenSearchEngine::networkAccessManager(); }

    QString call_parseTemplate(QString const &searchTerm, QString const &searchTemplate) const
        { return SubOpenSearchEngine::parseTemplate(searchTerm, searchTemplate); }

    void call_suggestions(QStringList const &suggestions)
        { return SubOpenSearchEngine::suggestions(suggestions); }
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

void tst_OpenSearchEngine::opensearchengine_data()
{
}

void tst_OpenSearchEngine::opensearchengine()
{
    SubOpenSearchEngine engine;
    QCOMPARE(engine.description(), QString());
    QCOMPARE(engine.image(), QImage());
    QCOMPARE(engine.imageUrl(), QString());
    QCOMPARE(engine.isValid(), false);
    QCOMPARE(engine.name(), QString());
    OpenSearchEngine other;
    QCOMPARE(engine.operator<(other), false);
    QCOMPARE(engine.operator==(other), true);
    QCOMPARE(engine.providesSuggestions(), false);
    engine.requestSuggestions(QString());
    QCOMPARE(engine.searchParameters(), QList<OpenSearchEngine::Parameter>());
    QCOMPARE(engine.searchUrl(QString()), QUrl());
    QCOMPARE(engine.searchUrlTemplate(), QString());
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
    QVERIFY(engine.call_networkAccessManager());
    QCOMPARE(engine.call_parseTemplate(QString(), QString()), QString());
    engine.call_suggestions(QStringList());
}

void tst_OpenSearchEngine::description_data()
{
    QTest::addColumn<QString>("description");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public QString description() const
void tst_OpenSearchEngine::description()
{
    QFETCH(QString, description);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setDescription(description);
    QCOMPARE(engine.description(), description);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

// public QImage image() const
void tst_OpenSearchEngine::image()
{
    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    QPixmap image(1, 1);
    image.fill();
    image.save(&imageBuffer, "PNG");
    engine.setImageUrl(QString("data:image/png;base64,").append(imageBuffer.buffer().toBase64()));
    QCOMPARE(engine.image(), QImage());

    QTRY_COMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QVERIFY(engine.image() != QImage());
}

void tst_OpenSearchEngine::imageUrl_data()
{
    QTest::addColumn<QString>("imageUrl");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public QString imageUrl() const
void tst_OpenSearchEngine::imageUrl()
{
    QFETCH(QString, imageUrl);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setImageUrl(imageUrl);
    QCOMPARE(engine.imageUrl(), imageUrl);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

void tst_OpenSearchEngine::isValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("searchUrlTemplate");
    QTest::addColumn<bool>("isValid");
    QTest::newRow("false-0") << QString() << QString() << false;
    QTest::newRow("false-1") << QString() << QString("x") << false;
    QTest::newRow("false-2") << QString("x") << QString() << false;
    QTest::newRow("true") << QString("x") << QString("y") << true;
}

// public bool isValid() const
void tst_OpenSearchEngine::isValid()
{
    QFETCH(QString, name);
    QFETCH(QString, searchUrlTemplate);
    QFETCH(bool, isValid);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setName(name);
    engine.setSearchUrlTemplate(searchUrlTemplate);
    QCOMPARE(engine.isValid(), isValid);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

void tst_OpenSearchEngine::name_data()
{
    QTest::addColumn<QString>("name");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public QString name() const
void tst_OpenSearchEngine::name()
{
    QFETCH(QString, name);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setName(name);
    QCOMPARE(engine.name(), name);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

// public bool operator<(OpenSearchEngine const &other) const
void tst_OpenSearchEngine::operatorlessthan()
{
    SubOpenSearchEngine engine1;
    engine1.setName("a");
    SubOpenSearchEngine engine2;
    engine2.setName("b");

    QVERIFY(engine1 < engine2);
}

typedef QList<OpenSearchEngine::Parameter> Parameters;
Q_DECLARE_METATYPE(Parameters)
void tst_OpenSearchEngine::operatorequal_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("imageUrl");
    QTest::addColumn<QString>("searchUrlTemplate");
    QTest::addColumn<QString>("suggestionsUrlTemplate");
    QTest::addColumn<Parameters>("searchParameters");
    QTest::addColumn<Parameters>("suggestionsParameters");
    QTest::addColumn<bool>("operatorequal");
    QTest::newRow("null") << QString() << QString() << QString() << QString() << QString()
                          << Parameters() << Parameters()
                          << true;
    QTest::newRow("name") << QString("x") << QString() << QString() << QString() << QString()
                          << Parameters() << Parameters()
                          << false;
    QTest::newRow("description") << QString() << QString("x") << QString() << QString() << QString()
                          << Parameters() << Parameters()
                          << false;
    QTest::newRow("imageUrl") << QString() << QString() << QString("x") << QString() << QString()
                          << Parameters() << Parameters()
                          << false;
}

// public bool operator==(OpenSearchEngine const &other) const
void tst_OpenSearchEngine::operatorequal()
{
    QFETCH(QString, name);
    QFETCH(QString, description);
    QFETCH(QString, imageUrl);
    QFETCH(QString, searchUrlTemplate);
    QFETCH(QString, suggestionsUrlTemplate);
    QFETCH(Parameters, searchParameters);
    QFETCH(Parameters, suggestionsParameters);
    QFETCH(bool, operatorequal);

    SubOpenSearchEngine engine;
    SubOpenSearchEngine other;
    other.setName(name);
    other.setDescription(description);
    other.setImageUrl(imageUrl);
    other.setSearchUrlTemplate(searchUrlTemplate);
    other.setSuggestionsUrlTemplate(suggestionsUrlTemplate);
    other.setSearchParameters(searchParameters);
    other.setSuggestionsParameters(suggestionsParameters);

    QCOMPARE(engine.operator==(other), operatorequal);
}

void tst_OpenSearchEngine::providesSuggestions_data()
{
    QTest::addColumn<QString>("suggestionsUrlTemplate");
    QTest::addColumn<bool>("providesSuggestions");
    QTest::newRow("false") << QString() << false;
    QTest::newRow("true") << QString("foo") << true;
}

// public bool providesSuggestions() const
void tst_OpenSearchEngine::providesSuggestions()
{
    QFETCH(QString, suggestionsUrlTemplate);
    QFETCH(bool, providesSuggestions);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setSuggestionsUrlTemplate(suggestionsUrlTemplate);
    QCOMPARE(engine.providesSuggestions(), providesSuggestions);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

void tst_OpenSearchEngine::requestSuggestions_data()
{
    QTest::addColumn<QString>("searchTerm");
    QTest::addColumn<QStringList>("suggestions");
    QTest::newRow("null") << QString() << QStringList();
    QTest::newRow("foo") << QString("foo") << (QStringList());
    // TODO load a real engine and make sure it returns a suggestion
}

// public void requestSuggestions(QString const &searchTerm)
void tst_OpenSearchEngine::requestSuggestions()
{
    QFETCH(QString, searchTerm);
    QFETCH(QStringList, suggestions);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.requestSuggestions(searchTerm);

    QCOMPARE(spy0.count(), 0);
    QTRY_COMPARE(spy1.count(), suggestions.count());
    //QCOMPARE(spy1.value(0), suggestions);
}

void tst_OpenSearchEngine::searchParameters_data()
{
    QTest::addColumn<Parameters>("searchParameters");
    QTest::newRow("null") << QList<OpenSearchEngine::Parameter>();
    QTest::newRow("something") << (Parameters() << OpenSearchEngine::Parameter("a", "b"));
}

// public QList<OpenSearchEngine::Parameter> searchParameters() const
void tst_OpenSearchEngine::searchParameters()
{
    QFETCH(Parameters, searchParameters);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setSearchParameters(searchParameters);
    QCOMPARE(engine.searchParameters(), searchParameters);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

void tst_OpenSearchEngine::searchUrl_data()
{
    QTest::addColumn<QString>("searchTerm");
    QTest::addColumn<QString>("searchUrlTemplate");
    QTest::addColumn<QUrl>("searchUrl");
    QTest::newRow("null") << QString() << QString() << QUrl();
    QTest::newRow("foo") << QString("foo") << QString("http://foobar.baz/?q={searchTerms}")
                    << QUrl(QString("http://foobar.baz/?q=foo"));
    QTest::newRow("empty") << QString() << QString("http://foobar.baz/?q={searchTerms}")
                    << QUrl(QString("http://foobar.baz/?q="));
}

// public QUrl searchUrl(QString const &searchTerm) const
void tst_OpenSearchEngine::searchUrl()
{
    QFETCH(QString, searchTerm);
    QFETCH(QString, searchUrlTemplate);
    QFETCH(QUrl, searchUrl);

    SubOpenSearchEngine engine;
    engine.setSearchUrlTemplate(searchUrlTemplate);

    QCOMPARE(engine.searchUrl(searchTerm), searchUrl);
}

void tst_OpenSearchEngine::searchUrlTemplate_data()
{
    QTest::addColumn<QString>("searchUrlTemplate");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public QString searchUrlTemplate() const
void tst_OpenSearchEngine::searchUrlTemplate()
{
    QFETCH(QString, searchUrlTemplate);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setSearchUrlTemplate(searchUrlTemplate);
    QCOMPARE(engine.searchUrlTemplate(), searchUrlTemplate);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

void tst_OpenSearchEngine::suggestionsParameters_data()
{
    QTest::addColumn<Parameters>("suggestionsParameters");
    QTest::newRow("null") << QList<OpenSearchEngine::Parameter>();
    QTest::newRow("something") << (Parameters() << OpenSearchEngine::Parameter("a", "b"));
}

// public QList<OpenSearchEngine::Parameter> suggestionsParameters() const
void tst_OpenSearchEngine::suggestionsParameters()
{
    QFETCH(Parameters, suggestionsParameters);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setSuggestionsParameters(suggestionsParameters);
    QCOMPARE(engine.suggestionsParameters(), suggestionsParameters);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

void tst_OpenSearchEngine::suggestionsUrl_data()
{
    QTest::addColumn<QString>("searchTerm");
    QTest::addColumn<QString>("suggestionsUrlTemplate");
    QTest::addColumn<QUrl>("suggestionsUrl");
    QTest::newRow("null") << QString() << QString() << QUrl();
    QTest::newRow("null") << QString() << QString() << QUrl();
    QTest::newRow("foo") << QString("foo") << QString("http://foobar.baz/?q={searchTerms}")
    << QUrl(QString("http://foobar.baz/?q=foo"));
    QTest::newRow("empty") << QString() << QString("http://foobar.baz/?q={searchTerms}")
    << QUrl(QString("http://foobar.baz/?q="));
}

// public QUrl suggestionsUrl(QString const &searchTerm) const
void tst_OpenSearchEngine::suggestionsUrl()
{
    QFETCH(QString, searchTerm);
    QFETCH(QString, suggestionsUrlTemplate);
    QFETCH(QUrl, suggestionsUrl);

    SubOpenSearchEngine engine;
    engine.setSuggestionsUrlTemplate(suggestionsUrlTemplate);

    QCOMPARE(engine.suggestionsUrl(searchTerm), suggestionsUrl);
}

void tst_OpenSearchEngine::suggestionsUrlTemplate_data()
{
    QTest::addColumn<QString>("suggestionsUrlTemplate");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public QString suggestionsUrlTemplate() const
void tst_OpenSearchEngine::suggestionsUrlTemplate()
{
    QFETCH(QString, suggestionsUrlTemplate);

    SubOpenSearchEngine engine;

    QSignalSpy spy0(&engine, SIGNAL(imageChanged()));
    QSignalSpy spy1(&engine, SIGNAL(suggestions(QStringList const&)));

    engine.setSuggestionsUrlTemplate(suggestionsUrlTemplate);
    QCOMPARE(engine.suggestionsUrlTemplate(), suggestionsUrlTemplate);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
}

void tst_OpenSearchEngine::parseTemplate_data()
{
    QString lang = BrowserApplication::languageManager()->currentLanguage().mid(0, 2);

    QTest::addColumn<QString>("searchTerm");
    QTest::addColumn<QString>("searchTemplate");
    QTest::addColumn<QString>("parseTemplate");
    QTest::addColumn<bool>("valid");
    QTest::newRow("null") << QString() << QString() << QString() << false;
    QTest::newRow("foo") << QString("foo") << QString("http://foobar.baz/?q={searchTerms}")
                    << QString("http://foobar.baz/?q=foo") << true;
    QTest::newRow("allParameters") << QString("bar")
                    << QString("http://foobar.baz/?st={searchTerms}&amp;c={count}"
                               "&amp;si={startIndex}&amp;sp={startPage}&amp;l={language}"
                               "&amp;ie={inputEncoding}&amp;oe={outputEncoding}")
                    << QString("http://foobar.baz/?st=bar&amp;c=20&amp;si=0&amp;"
                               "sp=0&amp;l=%1&amp;ie=UTF-8&amp;oe=UTF-8").arg(lang)
                    << true;
    QTest::newRow("tricky") << QString("{count}") << QString("http://foobar.baz/q={searchTerms}&amp;count={count}")
                    << QString("http://foobar.baz/q={count}&amp;count=20") << true;
    QTest::newRow("multiple") << QString("abc") << QString("http://foobar.baz/?q={searchTerms}&amp;x={searchTerms}")
                    << QString("http://foobar.baz/?q=abc&amp;x=abc") << true;
    QTest::newRow("referrer") << QString("foo")
                    << QString("http://foobar.baz/?q={searchTerms}&amp;a={source}&amp;b={ref:source}&amp;c={referrer:source?}")
                    << QString("http://foobar.baz/?q=foo&amp;a=Arora&amp;b=Arora&amp;c=Arora") << true;
}

// protected QString parseTemplate(QString const &searchTerm, QString const &searchTemplate) const
void tst_OpenSearchEngine::parseTemplate()
{
    QFETCH(QString, searchTerm);
    QFETCH(QString, searchTemplate);
    QFETCH(QString, parseTemplate);
    QFETCH(bool, valid);

    SubOpenSearchEngine engine;
    QString url = engine.call_parseTemplate(searchTerm, searchTemplate);
    QCOMPARE(url, parseTemplate);
    QCOMPARE(QUrl(url).isValid(), valid);
}

QTEST_MAIN(tst_OpenSearchEngine)
#include "tst_opensearchengine.moc"

