/*
 * Copyright 2009 Christian Franke <cfchris6@ts2server.com>
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

#include "opensearchenginemodel.h"

#include "opensearchengine.h"

#include <qdebug.h>
#include <qpixmap.h>

OpenSearchEngineModel::OpenSearchEngineModel(QHash<QString, OpenSearchEngine *> *engines, QObject *parent)
    : QAbstractListModel(parent)
    , m_engines(engines)
{ }

int OpenSearchEngineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_engines->count();
}

QVariant OpenSearchEngineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_engines->count() || index.row() < 0)
        return QVariant();

    OpenSearchEngine *engine = m_engines->values().at(index.row());

    if (!engine)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return engine->name();
    break;
    case Qt::DecorationRole:
        return engine->icon();
    break;
    case Qt::ToolTipRole:
        QString description = tr("<strong>Description:</strong> %1").arg(engine->description());

        if (engine->providesSuggestions()) {
            description += QLatin1String("<br />");
            description += tr("<strong>Provides contextual suggestions</strong>");
        }

        return description;
    break;
    }

    return QVariant();
}

void OpenSearchEngineModel::reset()
{
    QAbstractListModel::reset();
}
