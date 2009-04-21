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
#include "opensearchmanager.h"

#include <qpixmap.h>

OpenSearchEngineModel::OpenSearchEngineModel(OpenSearchManager *manager, QObject *parent)
    : QAbstractListModel(parent)
    , m_manager(manager)
{
    connect(manager, SIGNAL(changed()),
            this, SLOT(enginesChanged()));
}

bool OpenSearchEngineModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    if (count < 1)
        return false;

    int lastRow = row + count - 1;

    beginRemoveRows(parent, row, lastRow);

    QStringList nameList = m_manager->allEnginesNames();
    for (int i = row; i <= lastRow; ++i)
        m_manager->removeEngine(nameList.at(i));

    endRemoveRows();

    return true;
}

int OpenSearchEngineModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : m_manager->enginesCount();
}

QVariant OpenSearchEngineModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= m_manager->enginesCount() || index.row() < 0)
        return QVariant();

    OpenSearchEngine *engine = m_manager->engine(m_manager->allEnginesNames().at(index.row()));

    if (!engine)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return engine->name();
    break;
    case Qt::DecorationRole:
        return engine->image();
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

void OpenSearchEngineModel::enginesChanged()
{
    QAbstractListModel::reset();
}

