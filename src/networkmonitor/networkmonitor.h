/*
 * Copyright 2008-2009 Arora Developers
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

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include "ui_networkmonitor.h"
#include <qdialog.h>

#include <qnetworkaccessmanager.h>

class RequestModel;
class QStandardItemModel;
class QSortFilterProxyModel;
class NetworkMonitor : public QDialog, public Ui_NetworkMonitorDialog
{
    Q_OBJECT

protected:
    NetworkMonitor(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    static NetworkMonitor *m_self;

private slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public:
    static NetworkMonitor *self();

private:
    QSortFilterProxyModel *m_proxyModel;
    RequestModel *m_model;
    QStandardItemModel *m_requestHeaders;
    QStandardItemModel *m_replyHeaders;
};

#include <qnetworkrequest.h>

class Request {
public:
    QNetworkAccessManager::Operation op;
    QNetworkRequest request;
    QNetworkReply *reply;

    QString response;
    int length;
    QString contentType;
    QString info;
    QList<QPair<QByteArray, QByteArray> > replyHeaders;
};

class RequestModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    RequestModel(QObject *parent = 0);

    void addRequest(const Request &request);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent);

private slots:
    void requestCreated(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QNetworkReply *reply);
    void update();

private:
    friend class NetworkMonitor;
    QList<Request> m_requests;
};

#endif // NETWORKMONITOR_H

