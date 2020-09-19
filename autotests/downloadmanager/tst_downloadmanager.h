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

#ifndef TST_DOWNLOADMANAGER_H
#define TST_DOWNLOADMANAGER_H

#include <QObject>
#include "downloadmanager.h"

class tst_DownloadManager : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void downloadmanager_data();
    void downloadmanager();
    void cleanupButton_data();
    void cleanupButton();
    void download_data();
    void download();
    void removePolicy_data();
    void removePolicy();
};

// Subclass that exposes the protected functions.
class SubDownloadManager : public DownloadManager
{
public:
    SubDownloadManager(QWidget *parent = 0)
        : DownloadManager(parent)
    {}

};

#endif