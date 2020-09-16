/*
 * Copyright 2020 Aaron Dewes <aaron.dewes@web.de>
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

#include <history.h>
#include <historymanager.h>

class tst_HistoryFilterModel : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void historyfiltermodel_data();
    void historyfiltermodel();

    void historyContains_data();
    void historyContains();

    void setSourceModel();

    void historyLocation_data();
    void historyLocation();

    void addRow_data();
    void addRow();

    void removeRows_data();
    void removeRows();
};

// Subclass that exposes the protected functions.
class SubHistoryFilterModel : public HistoryFilterModel
{
public:
    SubHistoryFilterModel(QObject *parent = 0)
        : HistoryFilterModel(0, parent)
    {
        history = new HistoryManager(this);
        historyModel = new HistoryModel(history, this);
        setSourceModel(historyModel);
        history->setDaysToExpire(-1);
    }

    HistoryModel *historyModel;
    HistoryManager *history;
};
