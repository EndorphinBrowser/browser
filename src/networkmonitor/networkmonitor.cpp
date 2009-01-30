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

#include <qheaderview.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsortfilterproxymodel.h>
#include <qstandarditemmodel.h>

NetworkMonitor *NetworkMonitor::m_self = 0;

NetworkMonitor *NetworkMonitor::self() {
    if (!m_self) {
        m_self = new NetworkMonitor;
        m_self->setAttribute(Qt::WA_DeleteOnClose, true);
    }
    return m_self;
}

NetworkMonitor::NetworkMonitor(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    setupUi(this);
    requestList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_requestHeaders = new QStandardItemModel(this);
    m_requestHeaders->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    requestDetailsView->setModel(m_requestHeaders);
    m_replyHeaders = new QStandardItemModel(this);
    m_replyHeaders->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    responseDetailsView->setModel(m_replyHeaders);
    requestDetailsView->horizontalHeader()->setStretchLastSection(true);
    responseDetailsView->horizontalHeader()->setStretchLastSection(true);

    requestList->horizontalHeader()->setStretchLastSection(true);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setFilterKeyColumn(-1);
    requestList->setShowGrid(false);
    requestList->setAlternatingRowColors(true);
    requestList->verticalHeader()->setMinimumSectionSize(-1);
    connect(search, SIGNAL(textChanged(QString)),
            m_proxyModel, SLOT(setFilterFixedString(QString)));
    connect(removeButton, SIGNAL(clicked()), requestList, SLOT(removeSelected()));
    connect(removeAllButton, SIGNAL(clicked()), requestList, SLOT(removeAll()));
    m_model = new RequestModel(this);
    m_proxyModel->setSourceModel(m_model);
    requestList->setModel(m_proxyModel);
    connect(requestList->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this,
            SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));

    QFontMetrics fm = fontMetrics();
    int m = fm.width(QLatin1Char('m'));
    requestList->horizontalHeader()->resizeSection(0, m * 5);
    requestList->horizontalHeader()->resizeSection(1, m * 20);
    requestList->horizontalHeader()->resizeSection(3, m * 5);
    requestList->horizontalHeader()->resizeSection(4, m * 15);
}

void NetworkMonitor::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    m_requestHeaders->setRowCount(0);
    m_replyHeaders->setRowCount(0);

    if (!current.isValid())
        return;
    int row = m_proxyModel->mapToSource(current).row();

    QNetworkRequest req = m_model->m_requests[row].request;

    foreach(const QByteArray &header, req.rawHeaderList()) {
        m_requestHeaders->insertRows(0, 1, QModelIndex());
        m_requestHeaders->setData(m_requestHeaders->index(0, 0),
                QString::fromLatin1(header));
        m_requestHeaders->setData(m_requestHeaders->index(0, 1),
                QString::fromLatin1(req.rawHeader(header)));
        m_requestHeaders->item(0, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_requestHeaders->item(0, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    for (int i = 0; i < m_model->m_requests[row].replyHeaders.count(); ++i) {
        QByteArray first = m_model->m_requests[row].replyHeaders[i].first;
        QByteArray second = m_model->m_requests[row].replyHeaders[i].second;
        m_replyHeaders->insertRows(0, 1, QModelIndex());
        m_replyHeaders->setData(m_replyHeaders->index(0, 0), first);
        m_replyHeaders->setData(m_replyHeaders->index(0, 1), second);
        m_replyHeaders->item(0, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_replyHeaders->item(0, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
}

RequestModel::RequestModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    NetworkAccessManager *manager = BrowserApplication::networkAccessManager();
    connect(manager, SIGNAL(requestCreated(QNetworkAccessManager::Operation, const QNetworkRequest &, QNetworkReply *)),
            this, SLOT(requestCreated(QNetworkAccessManager::Operation, const QNetworkRequest &, QNetworkReply *)));
}

void RequestModel::requestCreated(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QNetworkReply *reply)
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
    m_requests.append(request);
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
    for (offset = m_requests.count() - 1; offset >= 0; --offset) {
        if (m_requests[offset].reply == reply)
            break;
    }
    if (offset < 0)
        return;

    // Save reply headers
    QByteArray header;
    foreach(header, reply->rawHeaderList() ) {
        QPair<QByteArray, QByteArray> pair(header, reply->rawHeader(header));
        m_requests[offset].replyHeaders.append(pair);
    }

    // Save reply info to be displayed
    int status = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    m_requests[offset].response = QString(QLatin1String("%1 %2")).arg(status).arg(reason);
    m_requests[offset].length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
    m_requests[offset].contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

    if (status == 302) {
        QUrl target = reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl();
        m_requests[offset].info = tr("Redirect: %1").arg(target.toString());
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
    if (index.row() < 0 || index.row() >= m_requests.size())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        switch (index.column()) {
        case 0:
            switch (m_requests[index.row()].op) {
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
            return m_requests[index.row()].request.url().toEncoded();
        case 2:
            return m_requests[index.row()].response;
        case 3:
            return m_requests[index.row()].length;
        case 4:
            return m_requests[index.row()].contentType;
        case 5:
            return m_requests[index.row()].info;

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
    return (parent.isValid()) ? 0 : m_requests.count();
}

bool RequestModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    int lastRow = row + count - 1;
    beginRemoveRows(parent, row, lastRow);
    for (int i = lastRow; i >= row; --i)
        m_requests.removeAt(i);
    endRemoveRows();
    return true;
}


