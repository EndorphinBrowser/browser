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

#ifndef OPENSEARCHMANAGER_H
#define OPENSEARCHMANAGER_H

#include <qobject.h>

#include <qhash.h>
#include <qpixmap.h>
#include <qurl.h>

class QNetworkReply;
class QNetworkRequest;

class AutoSaver;
class OpenSearchEngine;
class OpenSearchEngineModel;

class OpenSearchManager : public QObject
{
    Q_OBJECT

signals:
    void changed();
    void currentChanged();

public:
    OpenSearchManager(QObject *parent = 0);
    ~OpenSearchManager();

    QStringList allEnginesNames() const;

    QString currentName() const;
    void setCurrentName(const QString &currentName);

    OpenSearchEngine *currentEngine() const;
    void setCurrentEngine(OpenSearchEngine *current);

    OpenSearchEngine *engine(const QString &name);

    bool engineExists(const QString &name);

    OpenSearchEngineModel *model();

    void addEngine(const QUrl &url);
    bool addEngine(const QString &fileName);
    bool addEngine(OpenSearchEngine *description);
    void removeEngine(const QString &name);
    void restoreDefaults();

public slots:
    void save();

protected:
    void load();
    bool loadDirectory(const QString &dirName);
    void saveDirectory(const QString &dirName);
    QString enginesDirectory() const;

private:
    bool confirmAddition(OpenSearchEngine *engine);
    QString fileName(const QString &engineName);

protected slots:
    void engineFromUrlAvailable();

private:
    AutoSaver *m_autoSaver;

    OpenSearchEngineModel *m_model;
    QHash<QString, OpenSearchEngine *> m_engines;
    QString m_current;
};

#endif //OPENSEARCHMANAGER_H
