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

#include <QtTest/QtTest>

#include <historymanager.h>
#include <historycompleter.h>
#include <modeltest.h>

#include <QWebEngineSettings>

class tst_HistoryManager : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void history_data();
    void history();
    void addHistoryEntry_data();
    void addHistoryEntry();
    void addHistoryEntry_url();
    void updateHistoryEntry_data();
    void updateHistoryEntry();
    void daysToExpire_data();
    void daysToExpire();
    void clear_data();
    void clear();
    void setHistory_data();
    void setHistory();
    void saveload_data();
    void saveload();

    // TODO move to their own tests
    void big();

    void historyDialog_data();
    void historyDialog();

private:
    QList<HistoryEntry> bigHistory;
};

// Subclass that exposes the protected functions.
class SubHistory : public HistoryManager
{
public:
    SubHistory() : HistoryManager()
    {
        QWidget w;
        setParent(&w);
        setParent(0);
    }

    ~SubHistory() {
        setDaysToExpire(30);
    }

    void prependHistoryEntry(const HistoryEntry &item)
    {
        HistoryManager::prependHistoryEntry(item);
    }
};
