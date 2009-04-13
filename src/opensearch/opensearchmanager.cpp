/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
 * Copyright 2009 Christian Franke <cfchris6@ts2server.com>
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
#include "networkaccessmanager.h"
#include "opensearchengine.h"
#include "opensearchenginemodel.h"
#include "opensearchreader.h"
#include "opensearchwriter.h"

#include <qdesktopservices.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qregexp.h>
#include <qsettings.h>
#include <qstringlist.h>

OpenSearchManager::OpenSearchManager(QObject *parent)
    : QObject(parent)
    , m_autoSaver(new AutoSaver(this))
    , m_model(0)
    , m_engineFromUrlRequest(0)
{
    m_model = new OpenSearchEngineModel(&m_engines, this);

    load();
}

OpenSearchManager::~OpenSearchManager()
{
    m_autoSaver->saveIfNeccessary();
    qDeleteAll(m_engines.values());
    m_engines.clear();
    delete m_model;
}

QString OpenSearchManager::currentName() const
{
    return m_current;
}

void OpenSearchManager::setCurrentName(const QString &name)
{
    m_current = name;
    emit currentChanged();
}

OpenSearchEngine *OpenSearchManager::current() const
{
    if (currentName().isEmpty() || !m_engines.contains(currentName()))
        return 0;

    return m_engines[currentName()];
}

void OpenSearchManager::setCurrent(OpenSearchEngine *description)
{
    if (!description)
        return;

    setCurrentName(m_engines.key(description));
}

OpenSearchEngine *OpenSearchManager::engine(const QString &name)
{
    if (!m_engines.contains(name))
        return 0;

    return m_engines[name];
}

bool OpenSearchManager::engineExists(const QString &name)
{
    return m_engines.contains(name);
}

QList<QString> OpenSearchManager::nameList() const
{
    return m_engines.keys();
}

OpenSearchEngineModel *OpenSearchManager::model() const
{
    return m_model;
}

void OpenSearchManager::addEngine(const QUrl &url)
{
    if (!url.isValid())
        return;

    if (m_engineFromUrlRequest != 0) {
        m_engineFromUrlReply->abort();
        delete m_engineFromUrlRequest;
    }

    m_engineFromUrlRequest = new QNetworkRequest(url);
    m_engineFromUrlReply = BrowserApplication::networkAccessManager()->get(*m_engineFromUrlRequest);
    connect(m_engineFromUrlReply, SIGNAL(finished()), this, SLOT(engineFromUrlAvailable()));
    m_engineFromUrlReply->setParent(this);
}

bool OpenSearchManager::addEngine(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    OpenSearchReader reader;
    OpenSearchEngine *description = reader.read(&file);

    file.close();

    if (!description->isValid()) {
        delete description;
        return false;
    }

    if (!addEngine(description)) {
        delete description;
        return false;
    }

    return true;
}

bool OpenSearchManager::addEngine(OpenSearchEngine *description)
{
    if (!description)
        return false;

    if (!description->isValid())
        return false;

    if (m_engines.contains(description->name()))
        return false;

    description->setNetworkAccessManager(BrowserApplication::networkAccessManager());
    m_engines[description->name()] = description;

    m_autoSaver->changeOccurred();
    m_model->reset();
    emit changed();

    return true;
}

void OpenSearchManager::removeEngine(const QString &name)
{
    Q_ASSERT(m_engines.count() == 0);

    if (!m_engines.contains(name))
        return;

    delete m_engines[name];
    m_engines[name] = 0;
    m_engines.remove(name);

    QString file = QDir(enginesDirectory()).filePath(fileName(name));
    QFile::remove(file);

    if (name == currentName()) {
        setCurrentName(m_engines.keys().at(0));
        emit currentChanged();
    }

    m_autoSaver->changeOccurred();
    m_model->reset();
    emit changed();
}

QString OpenSearchManager::fileName(const QString &name)
{
    QString fileName;

    // Strip special characters from the name.
    for (int i = 0; i < name.count(); i++) {
        if (name.at(i).isSpace()) {
            fileName.append(QLatin1Char('_'));
            continue;
        }

        if (name.at(i).isLetterOrNumber())
            fileName.append(name.at(i));
    }

    fileName.append(QLatin1String(".xml"));

    return fileName;
}

void OpenSearchManager::saveDirectory(const QString &dirName)
{
    QDir dir;
    dir.mkpath(dirName);
    dir.setPath(dirName);

    OpenSearchWriter writer;

    foreach (OpenSearchEngine *engine, m_engines.values()) {
        QString name = fileName(engine->name());
        QString fileName = dir.filePath(name);

        QFile file(fileName);
        file.open(QIODevice::WriteOnly);

        writer.write(&file, engine);

        file.close();
    }
}

void OpenSearchManager::save()
{
    saveDirectory(enginesDirectory());

    QSettings settings;
    settings.beginGroup(QLatin1String("openSearch"));
    settings.setValue(QLatin1String("engine"), m_current);
    settings.endGroup();
}

bool OpenSearchManager::loadDirectory(const QString &dirName)
{
    if (!QFile::exists(dirName))
        return false;

    QDirIterator iterator(dirName, QStringList() << QLatin1String("*.xml"));

    if (!iterator.hasNext())
        return false;

    while (iterator.hasNext())
        addEngine(iterator.next());

    m_autoSaver->changeOccurred();
    emit changed();

    return true;
}

void OpenSearchManager::load()
{
    if (!loadDirectory(enginesDirectory()))
        loadDirectory(QLatin1String(":/searchengines"));

    // get current engine
    QSettings settings;
    settings.beginGroup(QLatin1String("openSearch"));
    m_current = settings.value(QLatin1String("engine"), 0).toString();
    settings.endGroup();
    if (!m_engines.contains(m_current) && m_engines.count() > 0)
        m_current = m_engines.keys().at(0);

    m_autoSaver->changeOccurred();
    emit currentChanged();
    emit changed();
}

void OpenSearchManager::restoreDefaults()
{
    loadDirectory(QLatin1String(":/searchengines"));
}

QString OpenSearchManager::enginesDirectory()
{
    QDir directory(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    return directory.filePath(QLatin1String("searchengines"));
}

bool OpenSearchManager::confirmAddition(OpenSearchEngine *engine)
{
    QString host = QUrl(engine->searchUrl()).host();

    QMessageBox::StandardButton button = QMessageBox::question(0, QString(),
            tr("Do you want to add the following engine to your list of search engines?<br /><br />"
               "Name: %1<br />Searches on: %2").arg(engine->name(), host),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return (button == QMessageBox::Yes);
}

void OpenSearchManager::engineFromUrlAvailable()
{
    if (m_engineFromUrlReply->error() != QNetworkReply::NoError) {
        m_engineFromUrlReply->close();
        delete m_engineFromUrlRequest;
        m_engineFromUrlRequest = 0;
        return;
    }

    OpenSearchReader reader;
    OpenSearchEngine *engine = reader.read(m_engineFromUrlReply);

    m_engineFromUrlReply->close();
    delete m_engineFromUrlRequest;
    m_engineFromUrlRequest = 0;

    if (!engine->isValid()) {
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

    m_autoSaver->changeOccurred();
    emit changed();
}
