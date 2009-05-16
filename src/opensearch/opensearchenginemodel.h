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

#ifndef OPENSEARCHENGINEMODEL_H
#define OPENSEARCHENGINEMODEL_H

#include <qabstractitemmodel.h>

class OpenSearchEngine;
class OpenSearchManager;

class OpenSearchEngineModel : public QAbstractListModel
{
    Q_OBJECT

public:
    OpenSearchEngineModel(OpenSearchManager *manager, QObject *parent = 0);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected slots:
    void enginesChanged();

private:
    OpenSearchManager *m_manager;
};

#endif //OPENSEARCHENGINEMODEL_H

