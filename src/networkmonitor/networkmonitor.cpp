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

#include "networkmonitor.h"

#include "browserapplication.h"
#include "networkaccessmanager.h"

#include <qnetworkrequest.h>
#include <qnetworkreply.h>
#include <qstandarditemmodel.h>
#include <qheaderview.h>

#include <qdebug.h>

NetworkMonitor::NetworkMonitor(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    setupUi(this);
    requestList->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestHeaders = new QStandardItemModel(this);
    requestHeaders->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    requestDetailsView->setModel(requestHeaders);
    replyHeaders = new QStandardItemModel(this);
    replyHeaders->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    responseDetailsView->setModel(replyHeaders);
    requestDetailsView->horizontalHeader()->setStretchLastSection(true);
    responseDetailsView->horizontalHeader()->setStretchLastSection(true);

    connect(requestList, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(clicked(const QModelIndex &)));
    connect(clearButton, SIGNAL(clicked()),
            this, SLOT(clear()));
    model = new RequestModel(this);
    requestList->setModel(model);

    QFontMetrics fm = fontMetrics();
    int m = fm.width(QLatin1Char('m'));
    requestList->horizontalHeader()->resizeSection(0, m * 5);
    requestList->horizontalHeader()->resizeSection(1, m * 20);
    requestList->horizontalHeader()->resizeSection(3, m * 5);
    requestList->horizontalHeader()->resizeSection(4, m * 15);
}

void NetworkMonitor::clear()
{
    model->clear();
    requestHeaders->setRowCount(0);
    replyHeaders->setRowCount(0);
}

void NetworkMonitor::clicked(const QModelIndex &index)
{
    requestHeaders->setRowCount(0);
    replyHeaders->setRowCount(0);

    if (!index.isValid())
        return;

    QNetworkRequest req = model->requests[index.row()].request;

    foreach(const QByteArray &header, req.rawHeaderList()) {
        requestHeaders->insertRows(0, 1, QModelIndex());
        requestHeaders->setData(requestHeaders->index(0, 0), QString::fromLatin1(header));
        requestHeaders->setData(requestHeaders->index(0, 1), QString::fromLatin1(req.rawHeader(header)));
        requestHeaders->item(0, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        requestHeaders->item(0, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    for (int i = 0; i < model->requests[index.row()].replyHeaders.count(); ++i) {
        QByteArray first = model->requests[index.row()].replyHeaders[i].first;
        QByteArray second = model->requests[index.row()].replyHeaders[i].second;
        replyHeaders->insertRows(0, 1, QModelIndex());
        replyHeaders->setData(replyHeaders->index(0, 0), first);
        replyHeaders->setData(replyHeaders->index(0, 1), second);
        replyHeaders->item(0, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        replyHeaders->item(0, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
}

RequestModel::RequestModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    NetworkAccessManager *manager = BrowserApplication::networkAccessManager();
    connect(manager, SIGNAL(requestCreated(QNetworkAccessManager::Operation, const QNetworkRequest &, QNetworkReply *)),
            this, SLOT(requestCreated(QNetworkAccessManager::Operation, const QNetworkRequest &, QNetworkReply *)));
}

void RequestModel::clear()
{
    requests.clear();
    reset();
}

void RequestModel::requestCreated(QNetworkAccessManager::Operation op, const QNetworkRequest&req, QNetworkReply *reply)
{
    Request request;
    request.op = op;
    request.request = req;
    request.reply = reply;
    request.length = 0;
    addRequest(request);
}

void RequestModel::addRequest(const Request &request)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    requests.append(request);
    connect(request.reply, SIGNAL(finished()),
            this, SLOT(update()));
    endInsertRows();
}

void RequestModel::update()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    int offset;
    for (offset = requests.count() - 1; offset >= 0; --offset) {
        if (requests[offset].reply == reply)
            break;
    }
    if (offset < 0)
        return;

    // Save reply headers
    QByteArray header;
    foreach(header, reply->rawHeaderList() ) {
        QPair<QByteArray, QByteArray> pair(header, reply->rawHeader(header));
        requests[offset].replyHeaders.append(pair);
    }

    // Save reply info to be displayed
    int status = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    requests[offset].response = QString(QLatin1String("%1 %2")).arg(status).arg(reason);
    requests[offset].length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
    requests[offset].contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

    if (status == 302) {
        QUrl target = reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl();
        requests[offset].info = tr("Redirect: %1").arg(target.toString());
    }
}

QVariant RequestModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return tr("Method");
        case 1: return tr("Address");
        case 2: return tr("Response");
        case 3: return tr("Length");
        case 4: return tr("Content Type");
        case 5: return tr("Info");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant RequestModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= requests.size())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        switch (index.column()) {
        case 0:
            switch (requests[index.row()].op) {
                case QNetworkAccessManager::HeadOperation:
                    return QLatin1String("HEAD");
                    break;
                case QNetworkAccessManager::GetOperation:
                    return QLatin1String("GET");
                    break;
                case   QNetworkAccessManager::PutOperation:
                    return QLatin1String("PUT");
                    break;
                case QNetworkAccessManager::PostOperation:
                    return QLatin1String("POST");
                    break;
                default:
                    return QLatin1String("Unknown");
            }
        case 1:
            return requests[index.row()].request.url().toEncoded();
        case 2:
            return requests[index.row()].response;
        case 3:
            return requests[index.row()].length;
        case 4:
            return requests[index.row()].contentType;
        case 5:
            return requests[index.row()].info;

        }
        }
    }
    return QVariant();
}

int RequestModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : 6;
}

int RequestModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : requests.count();
}


