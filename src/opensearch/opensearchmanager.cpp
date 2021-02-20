/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
 * Copyright 2009 Christian Franke <cfchris6@ts2server.com>
 * Copyright 2009 Christopher Eby <kreed@kreed.org>
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

#include "opensearchmanager.h"

#include "autosaver.h"
#include "browserapplication.h"
#include <qnetworkaccessmanager.h>
#include "opensearchengine.h"
#include "opensearchreader.h"
#include "opensearchwriter.h"

#include <QDesktopServices>
#include <QDir>
#include <qdiriterator.h>
#include <QFile>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>
#include <QSettings>
#include <qstringlist.h>

OpenSearchManager::OpenSearchManager(QObject *parent)
    : QObject(parent)
    , m_autoSaver(new AutoSaver(this))
{
    connect(this, SIGNAL(changed()),
            m_autoSaver, SLOT(changeOccurred()));

    load();
}

OpenSearchManager::~OpenSearchManager()
{
    m_autoSaver->saveIfNeccessary();
    qDeleteAll(m_engines.values());
    m_engines.clear();
}

QString OpenSearchManager::currentEngineName() const
{
    return m_current;
}

void OpenSearchManager::setCurrentEngineName(const QString &name)
{
    if (!m_engines.contains(name))
        return;

    m_current = name;
    Q_EMIT currentEngineChanged();
    Q_EMIT changed();
}

OpenSearchEngine *OpenSearchManager::currentEngine() const
{
    if (m_current.isEmpty() || !m_engines.contains(m_current))
        return nullptr;

    return m_engines[m_current];
}

void OpenSearchManager::setCurrentEngine(OpenSearchEngine *engine)
{
    if (!engine)
        return;

    setCurrentEngineName(m_engines.key(engine));
}

OpenSearchEngine *OpenSearchManager::engine(const QString &name)
{
    if (!m_engines.contains(name))
        return nullptr;

    return m_engines[name];
}

bool OpenSearchManager::engineExists(const QString &name)
{
    return m_engines.contains(name);
}

QStringList OpenSearchManager::allEnginesNames() const
{
    return m_engines.keys();
}

int OpenSearchManager::enginesCount() const
{
    return m_engines.count();
}

void OpenSearchManager::addEngine(const QUrl &url)
{
    if (!url.isValid())
        return;

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(engineFromUrlAvailable()));
    reply->setParent(this);
}

bool OpenSearchManager::addEngine(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(&file);

    if (!addEngine(engine)) {
        delete engine;
        return false;
    }

    return true;
}

bool OpenSearchManager::addEngine(OpenSearchEngine *engine)
{
    if (!engine)
        return false;

    if (!engine->isValid())
        return false;

    if (m_engines.contains(engine->name()))
        return false;

    m_engines[engine->name()] = engine;

    Q_EMIT changed();

    return true;
}

void OpenSearchManager::removeEngine(const QString &name)
{
    if (m_engines.count() <= 1)
        return;

    if (!m_engines.contains(name))
        return;

    OpenSearchEngine *engine = m_engines[name];
    Q_FOREACH (const QString &keyword, m_keywords.keys(engine))
        m_keywords.remove(keyword);
    engine->deleteLater();

    m_engines[name] = 0;
    m_engines.remove(name);

    QString file = QDir(enginesDirectory()).filePath(generateEngineFileName(name));
    QFile::remove(file);

    if (name == m_current)
        setCurrentEngineName(m_engines.keys().at(0));

    Q_EMIT changed();
}

QString OpenSearchManager::generateEngineFileName(const QString &engineName) const
{
    QString fileName;

    // Strip special characters from the name.
    for (int i = 0; i < engineName.count(); ++i) {
        if (engineName.at(i).isSpace()) {
            fileName.append(QChar('_'));
            continue;
        }

        if (engineName.at(i).isLetterOrNumber())
            fileName.append(engineName.at(i));
    }

    fileName.append(QStringLiteral(".xml"));

    return fileName;
}

void OpenSearchManager::saveDirectory(const QString &dirName)
{
    QDir dir;
    if (!dir.mkpath(dirName))
        return;
    dir.setPath(dirName);

    OpenSearchWriter writer;

    Q_FOREACH (OpenSearchEngine *engine, m_engines.values()) {
        QString name = generateEngineFileName(engine->name());
        QString fileName = dir.filePath(name);

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
            continue;

        writer.write(&file, engine);
    }
}

void OpenSearchManager::save()
{
    saveDirectory(enginesDirectory());

    QSettings settings;
    settings.beginGroup(QStringLiteral("openSearch"));
    settings.setValue(QStringLiteral("engine"), m_current);

    settings.beginWriteArray(QStringLiteral("keywords"), m_keywords.count());
    QHash<QString, OpenSearchEngine*>::const_iterator i = m_keywords.constBegin();
    QHash<QString, OpenSearchEngine*>::const_iterator end = m_keywords.constEnd();
    int j = 0;
    for (; i != end; ++i) {
        settings.setArrayIndex(j++);
        settings.setValue(QStringLiteral("keyword"), i.key());
        settings.setValue(QStringLiteral("engine"), i.value()->name());
    }
    settings.endArray();

    settings.endGroup();
}

bool OpenSearchManager::loadDirectory(const QString &dirName)
{
    if (!QFile::exists(dirName))
        return false;

    QDirIterator iterator(dirName, QStringList() << QStringLiteral("*.xml"));

    if (!iterator.hasNext())
        return false;

    bool success = false;

    while (iterator.hasNext()) {
        if (addEngine(iterator.next()))
            success = true;
    }

    return success;
}

void OpenSearchManager::load()
{
    if (!loadDirectory(enginesDirectory()))
        loadDirectory(QStringLiteral(":/searchengines"));

    // get current engine
    QSettings settings;
    settings.beginGroup(QStringLiteral("openSearch"));
    m_current = settings.value(QStringLiteral("engine"), QStringLiteral("Google")).toString();

    int size = settings.beginReadArray(QStringLiteral("keywords"));
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString keyword = settings.value(QStringLiteral("keyword")).toString();
        QString engineName = settings.value(QStringLiteral("engine")).toString();
        m_keywords.insert(keyword, engine(engineName));
    }
    settings.endArray();

    settings.endGroup();

    if (!m_engines.contains(m_current) && m_engines.count() > 0)
        m_current = m_engines.keys().at(0);

    Q_EMIT currentEngineChanged();
}

void OpenSearchManager::restoreDefaults()
{
    loadDirectory(QStringLiteral(":/searchengines"));
}

QString OpenSearchManager::enginesDirectory() const
{
    QDir directory(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/data/Endorphin");
    return directory.filePath(QStringLiteral("searchengines"));
}

bool OpenSearchManager::confirmAddition(OpenSearchEngine *engine)
{
    if (!engine || !engine->isValid())
        return false;

    QString host = QUrl(engine->searchUrlTemplate()).host();

    QMessageBox::StandardButton button = QMessageBox::question(nullptr, QString(),
                                         tr("Do you want to add the following engine to your list of search engines?<br /><br />"
                                            "Name: %1<br />Searches on: %2").arg(engine->name(), host),
                                         QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return (button == QMessageBox::Yes);
}

void OpenSearchManager::engineFromUrlAvailable()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
        return;

    if (reply->error() != QNetworkReply::NoError) {
        reply->close();
        reply->deleteLater();
        return;
    }

    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(reply);

    reply->close();
    reply->deleteLater();

    if (!engine->isValid()) {
        delete engine;
        return;
    }

    if (engineExists(engine->name())) {
        delete engine;
        return;
    }

    if (!confirmAddition(engine)) {
        delete engine;
        return;
    }

    if (!addEngine(engine)) {
        delete engine;
        return;
    }
}

QUrl OpenSearchManager::convertKeywordSearchToUrl(const QString &string)
{
    int i = string.indexOf(QChar(' '));
    if (i <= 0)
        return QUrl();

    const QString keyword = string.left(i);
    const QString terms = string.mid(i + 1);
    if (terms.isEmpty())
        return QUrl();

    if (OpenSearchEngine *engine = engineForKeyword(keyword))
        return engine->searchUrl(terms);

    return QUrl();
}

OpenSearchEngine *OpenSearchManager::engineForKeyword(const QString &keyword) const
{
    if (keyword.isEmpty())
        return nullptr;
    if (!m_keywords.contains(keyword))
        return nullptr;
    return m_keywords.value(keyword);
}

void OpenSearchManager::setEngineForKeyword(const QString &keyword, OpenSearchEngine *engine)
{
    if (keyword.isEmpty())
        return;

    if (!engine)
        m_keywords.remove(keyword);
    else
        m_keywords.insert(keyword, engine);

    Q_EMIT changed();
}

QStringList OpenSearchManager::keywordsForEngine(OpenSearchEngine *engine) const
{
    return m_keywords.keys(engine);
}

void OpenSearchManager::setKeywordsForEngine(OpenSearchEngine *engine, const QStringList &keywords)
{
    if (!engine)
        return;

    Q_FOREACH (const QString &keyword, keywordsForEngine(engine))
        m_keywords.remove(keyword);

    Q_FOREACH (const QString &keyword, keywords) {
        if (keyword.isEmpty())
            continue;

        m_keywords.insert(keyword, engine);
    }

    Q_EMIT changed();
}
