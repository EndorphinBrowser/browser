/*
 * Copyright 2020 Aaron Dewes <aaron.dewes@web.de>
 * Copyright 2008 Jason A. Donenfeld <Jason@zx2c4.com>
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
/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "downloadmanager.h"

#include "autosaver.h"
#ifndef NO_BROWSERAPPLICATION
#include "browserapplication.h"
#endif

#include <math.h>

#include <QDesktopServices>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QHeaderView>
#include <QMessageBox>
#include <QMetaObject>
#include <QMimeData>
#include <QProcess>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QWebEngineDownloadItem>
#include <QMetaEnum>

#include <QDebug>

#include <QWebEngineSettings>

//#define DOWNLOADMANAGER_DEBUG

/*!
    DownloadItem is a widget that is displayed in the download manager list.
    It moves the data from the QNetworkReply into the QFile as well
    as update the information/progressbar and report errors.
 */
DownloadItem::DownloadItem(QWebEngineDownloadItem *download, QWidget *parent)
    : QWidget(parent)
    , m_download(download)
{
    setupUi(this);
    QPalette p = downloadInfoLabel->palette();
    p.setColor(QPalette::Text, Qt::darkGray);
    downloadInfoLabel->setPalette(p);
    progressBar->setMaximum(0);
    tryAgainButton->hide();  
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
    connect(tryAgainButton, SIGNAL(clicked()), this, SLOT(tryAgain()));

    if (download) {
        QString file = QDir::cleanPath(download->downloadDirectory() + QDir::separator() + download->downloadFileName());
        m_file.setFile(file);
        m_url = download->url();
    }

    init();
}

void DownloadItem::init()
{
    if (m_download) {
        connect(m_download.data(), SIGNAL(downloadProgress(qint64,qint64)),
                this, SLOT(downloadProgress()));
        connect(m_download.data(), SIGNAL(finished()),
                this, SLOT(finished()));
    }

    // reset info
    downloadInfoLabel->clear();
    progressBar->setValue(0);
    getFileName();

    // start timer for the download estimation
    m_downloadTime.start();
}

bool DownloadItem::getFileName(bool promptForFileName)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("downloadmanager"));
#ifndef NO_BROWSERAPPLICATION
    QString defaultLocation = BrowserApplication::downloadManager()->downloadDirectory();
#else
    QString defaultLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + '/';
#endif
    if (m_file.absoluteDir().exists())
        defaultLocation = m_file.absolutePath();
    QString downloadDirectory = settings.value(QStringLiteral("downloadDirectory"), defaultLocation).toString();
    if (!downloadDirectory.isEmpty())
        downloadDirectory += QChar('/');

    QString defaultFileName = QFileInfo(downloadDirectory, m_file.fileName()).absoluteFilePath();
    QString fileName = defaultFileName;
    if (promptForFileName) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"), defaultFileName);
        if (fileName.isEmpty()) {
            if (m_download)
                m_download->cancel();
            fileNameLabel->setText(tr("Download canceled: %1").arg(QFileInfo(defaultFileName).fileName()));
            return false;
        }
    }
    m_file.setFile(fileName);

    if (m_download && m_download->state() == QWebEngineDownloadItem::DownloadRequested) {
        m_download->setDownloadFileName(m_file.fileName());
        m_download->setDownloadDirectory(m_file.absolutePath());
    }

    fileNameLabel->setText(m_file.fileName());
    return true;
}

void DownloadItem::stop()
{
    setUpdatesEnabled(false);
    stopButton->setEnabled(false);
    stopButton->hide();
    setUpdatesEnabled(true);
    if (m_download)
        m_download->cancel();

    Q_EMIT statusChanged();
    Q_EMIT downloadFinished();
}

void DownloadItem::open()
{
    QUrl url = QUrl::fromLocalFile(m_file.absoluteFilePath());
    QDesktopServices::openUrl(url);
}

void DownloadItem::tryAgain()
{
    if (!tryAgainButton->isEnabled())
        return;

    tryAgainButton->setEnabled(false);
    tryAgainButton->setVisible(false);
    stopButton->setEnabled(true);
    stopButton->setVisible(true);
    progressBar->setVisible(true);

    m_download.data()->resume();
    init();
    Q_EMIT statusChanged();
}

void DownloadItem::downloadProgress()
{
    QTime now = QTime::currentTime();
    if (m_lastProgressTime.msecsTo(now) < 200)
        return;

    m_lastProgressTime = now;

    if (m_download.data()->totalBytes() == -1) {
        progressBar->setValue(0);
        progressBar->setMaximum(0);
    } else {
        progressBar->setValue(m_download.data()->receivedBytes());
        progressBar->setMaximum(m_download.data()->totalBytes());
    }
    updateInfoLabel();
}

QString DownloadItem::dataString(qint64 size) {
    return DownloadManager::dataString(size);
}

void DownloadItem::updateInfoLabel()
{

    // update info label
    double speed = m_download.data()->receivedBytes() * 1000.0 / m_downloadTime.elapsed();
    double timeRemaining = ((double)(m_download.data()->totalBytes() - m_download.data()->receivedBytes())) / speed;
    QString timeRemainingString = tr("seconds");
    if (timeRemaining > 60) {
        timeRemaining = timeRemaining / 60;
        timeRemainingString = tr("minutes");
    }
    timeRemaining = floor(timeRemaining);

    // When downloading the eta should never be 0
    if (timeRemaining == 0)
        timeRemaining = 1;

    QString info;
    if (!downloadedSuccessfully()) {
        QString remaining;
        if (m_download.data()->totalBytes() != 0)
            remaining = tr("- %4 %5 remaining")
            .arg(timeRemaining)
            .arg(timeRemainingString);
        info = tr("%1 of %2 (%3/sec) %4")
            .arg(dataString(m_download.data()->receivedBytes()))
            .arg(m_download.data()->totalBytes() == 0 ? tr("?") : dataString(m_download.data()->totalBytes()))
            .arg(dataString((int)speed))
            .arg(remaining);
    } else {
        if (m_download.data()->receivedBytes() != m_download.data()->totalBytes()) {
            info = tr("%1 of %2 - Stopped")
                .arg(dataString(m_download.data()->receivedBytes()))
                .arg(dataString(m_download.data()->totalBytes()));
        } else
            info = dataString(m_download.data()->receivedBytes());
    }
    downloadInfoLabel->setText(info);
}

bool DownloadItem::downloading() const
{
    return (progressBar->isVisible());
}

bool DownloadItem::downloadedSuccessfully() const
{
    bool completed = m_download
            && m_download->isFinished()
            && m_download->state() == QWebEngineDownloadItem::DownloadCompleted;
    return completed || !stopButton->isVisible();
}

void DownloadItem::finished()
{
    if (m_download) {
        QWebEngineDownloadItem::DownloadState state = m_download->state();
        QString message;
        bool interrupted = false;

        switch (state) {
        case QWebEngineDownloadItem::DownloadRequested: // Fall-through.
        case QWebEngineDownloadItem::DownloadInProgress:
            Q_UNREACHABLE();
            break;
        case QWebEngineDownloadItem::DownloadCompleted:
            break;
        case QWebEngineDownloadItem::DownloadCancelled:
            message = QStringLiteral("Download cancelled");
            interrupted = true;
            break;
        case QWebEngineDownloadItem::DownloadInterrupted:
            message = QStringLiteral("Download interrupted");
            interrupted = true;
            break;
        }

        if (interrupted) {
            downloadInfoLabel->setText(message);
            return;
        }
    }

    progressBar->hide();
    stopButton->setEnabled(false);
    stopButton->hide();
    updateInfoLabel();
    Q_EMIT statusChanged();
    Q_EMIT downloadFinished();
}

/*!
    DownloadManager is a Dialog that contains a list of DownloadItems

    It is a basic download manager.  It only downloads the file, doesn't do BitTorrent,
    extract zipped files or anything fancy.
  */
DownloadManager::DownloadManager(QWidget *parent)
    : QDialog(parent)
    , m_autoSaver(new AutoSaver(this))
    , m_model(new DownloadModel(this))
    , m_iconProvider(nullptr)
    , m_removePolicy(Never)
{
    setupUi(this);

    QSettings settings;
    settings.beginGroup(QStringLiteral("downloadmanager"));
    QString defaultLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    setDownloadDirectory(settings.value(QStringLiteral("downloadDirectory"), defaultLocation).toString());

    downloadsView->setShowGrid(false);
    downloadsView->verticalHeader()->hide();
    downloadsView->horizontalHeader()->hide();
    downloadsView->setAlternatingRowColors(true);
    downloadsView->horizontalHeader()->setStretchLastSection(true);
    downloadsView->setModel(m_model);
    connect(cleanupButton, SIGNAL(clicked()), this, SLOT(cleanup()));
    load();
}

DownloadManager::~DownloadManager()
{
    m_autoSaver->changeOccurred();
    m_autoSaver->saveIfNeccessary();
    if (m_iconProvider)
        delete m_iconProvider;
}

int DownloadManager::activeDownloads() const
{
    int count = 0;
    for (int i = 0; i < m_downloads.count(); ++i) {
        if (m_downloads.at(i)->stopButton->isEnabled())
            ++count;
    }
    return count;
}

bool DownloadManager::allowQuit()
{
    if (activeDownloads() >= 1) {
        int choice = QMessageBox::warning(this, QString(),
                                          tr("There are %1 downloads in progress\n"
                                             "Do you want to quit anyway?").arg(activeDownloads()),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::No);
        if (choice == QMessageBox::No) {
            show();
            return false;
        }
    }
    return true;
}

bool DownloadManager::externalDownload(const QUrl &url)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("downloadmanager"));
    if (!settings.value(QStringLiteral("external"), false).toBool())
        return false;

    QString program = settings.value(QStringLiteral("externalPath")).toString();
    if (program.isEmpty())
        return false;

    // Split program at every space not inside double quotes
    QRegExp regex(QStringLiteral("\"([^\"]+)\"|([^ ]+)"));
    QStringList args;
    for (int pos = 0; (pos = regex.indexIn(program, pos)) != -1; pos += regex.matchedLength())
        args << regex.cap(1) + regex.cap(2);
    if (args.isEmpty())
        return false;

    return QProcess::startDetached(args.takeFirst(), args << QString::fromUtf8(url.toEncoded()));
}

void DownloadManager::download(QWebEngineDownloadItem *download)
{
    if (externalDownload(download->url()))
        return;
    DownloadItem *item = new DownloadItem(download, this);
    addItem(item);

    if (!isVisible())
        show();

    activateWindow();
    raise();
}

void DownloadManager::addItem(DownloadItem *item)
{
    connect(item, SIGNAL(statusChanged()), this, SLOT(updateRow()));
    connect(item, SIGNAL(downloadFinished()), this, SLOT(finished()));
    int row = m_downloads.count();
    m_model->beginInsertRows(QModelIndex(), row, row);
    m_downloads.append(item);
    m_model->endInsertRows();
    updateItemCount();
    if (row == 0)
        show();
    downloadsView->setIndexWidget(m_model->index(row, 0), item);
    QIcon icon = style()->standardIcon(QStyle::SP_FileIcon);
    item->fileIcon->setPixmap(icon.pixmap(48, 48));
    downloadsView->setRowHeight(row, item->sizeHint().height());
    updateRow(item); //incase download finishes before the constructor returns
    updateActiveItemCount();
}

void DownloadManager::updateActiveItemCount()
{
    int acCount = activeDownloads();
    if (acCount > 0) {
        setWindowTitle(QApplication::translate("DownloadDialog", "Downloading %1", 0).arg(acCount));
    } else {
        setWindowTitle(QApplication::translate("DownloadDialog", "Downloads", 0));
    }
}

void DownloadManager::finished()
{
    updateActiveItemCount();
    if (isVisible()) {
        QApplication::alert(this);
    }
}


void DownloadManager::updateRow()
{
    if (DownloadItem *item = qobject_cast<DownloadItem*>(sender()))
        updateRow(item);
}

void DownloadManager::updateRow(DownloadItem *item)
{
    int row = m_downloads.indexOf(item);
    if (-1 == row)
        return;
    if (!m_iconProvider)
        m_iconProvider = new QFileIconProvider();
    QIcon icon = m_iconProvider->icon(item->m_file);
    if (icon.isNull())
        icon = style()->standardIcon(QStyle::SP_FileIcon);
    item->fileIcon->setPixmap(icon.pixmap(48, 48));
    downloadsView->setRowHeight(row, item->minimumSizeHint().height());

    bool remove = false;
    if (!item->downloading()
        && BrowserApplication::instance()->isPrivate())
            remove = true;
    

    if (item->downloadedSuccessfully()
            && removePolicy() == DownloadManager::SuccessFullDownload) {
        remove = true;
    }
    if (remove)
        m_model->removeRow(row);

    cleanupButton->setEnabled(m_downloads.count() - activeDownloads() > 0);
}

DownloadManager::RemovePolicy DownloadManager::removePolicy() const
{
    return m_removePolicy;
}

void DownloadManager::setRemovePolicy(RemovePolicy policy)
{
    if (policy == m_removePolicy)
        return;
    m_removePolicy = policy;
    m_autoSaver->changeOccurred();
}

void DownloadManager::save() const
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("downloadmanager"));
    QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    settings.setValue(QStringLiteral("removeDownloadsPolicy"), QString(removePolicyEnum.valueToKey(m_removePolicy)));
    settings.setValue(QStringLiteral("size"), size());
    if (m_removePolicy == Exit)
        return;

    for (int i = 0; i < m_downloads.count(); ++i) {
        QString key = QString(QStringLiteral("download_%1_")).arg(i);
        settings.setValue(key + QStringLiteral("url"), m_downloads[i]->m_url);
        settings.setValue(key + QStringLiteral("location"), m_downloads[i]->m_file.filePath());
        settings.setValue(key + QStringLiteral("done"), m_downloads[i]->downloadedSuccessfully());
    }
    int i = m_downloads.count();
    QString key = QString(QStringLiteral("download_%1_")).arg(i);
    while (settings.contains(key + QStringLiteral("url"))) {
        settings.remove(key + QStringLiteral("url"));
        settings.remove(key + QStringLiteral("location"));
        settings.remove(key + QStringLiteral("done"));
        key = QString(QStringLiteral("download_%1_")).arg(++i);
    }
}

void DownloadManager::load()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("downloadmanager"));
    QSize size = settings.value(QStringLiteral("size")).toSize();
    if (size.isValid())
        resize(size);
    QByteArray value = settings.value(QStringLiteral("removeDownloadsPolicy"), QStringLiteral("Never")).toByteArray();
    QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    m_removePolicy = removePolicyEnum.keyToValue(value) == -1 ?
                     Never :
                     static_cast<RemovePolicy>(removePolicyEnum.keyToValue(value));

    int i = 0;
    QString key = QString(QStringLiteral("download_%1_")).arg(i);
    while (settings.contains(key + QStringLiteral("url"))) {
        QUrl url = settings.value(key + QStringLiteral("url")).toUrl();
        QString fileName = settings.value(key + QStringLiteral("location")).toString();
        bool done = settings.value(key + QStringLiteral("done"), true).toBool();
        if (done && !url.isEmpty() && !fileName.isEmpty()) {
            DownloadItem *item = new DownloadItem(0, this);
            item->m_file.setFile(fileName);
            item->fileNameLabel->setText(item->m_file.fileName());
            item->m_url = url;
            item->stopButton->setVisible(false);
            item->stopButton->setEnabled(false);
            item->progressBar->hide();
            addItem(item);
        }
        key = QString(QStringLiteral("download_%1_")).arg(++i);
    }
    cleanupButton->setEnabled(m_downloads.count() - activeDownloads() > 0);
    updateActiveItemCount();
}

void DownloadManager::cleanup()
{
    if (m_downloads.isEmpty())
        return;
    m_model->removeRows(0, m_downloads.count());
    updateItemCount();
    updateActiveItemCount();
    if (m_downloads.isEmpty() && m_iconProvider) {
        delete m_iconProvider;
        m_iconProvider = nullptr;
    }
    m_autoSaver->changeOccurred();
}

void DownloadManager::updateItemCount()
{
    int count = m_downloads.count();
    itemCount->setText(tr("%n Download(s)", "", count));
}

void DownloadManager::setDownloadDirectory(const QString &directory)
{
    m_downloadDirectory = directory;
    if (!m_downloadDirectory.isEmpty())
        m_downloadDirectory += QChar('/');
}

QString DownloadManager::downloadDirectory()
{
    return m_downloadDirectory;
}

QString DownloadManager::timeString(double timeRemaining)
{
    QString remaining;

    if (timeRemaining > 60) {
        timeRemaining = timeRemaining / 60;
        timeRemaining = floor(timeRemaining);
        remaining = tr("%n minutes remaining", "", int(timeRemaining));
    }
    else {
        timeRemaining = floor(timeRemaining);
        remaining = tr("%n seconds remaining", "", int(timeRemaining));
    }

    return remaining;
}

QString DownloadManager::dataString(qint64 size)
{
    QString unit;
    double newSize;

    if (size < 1024) {
        newSize = size;
        unit = tr("bytes");
    } else if (size < 1024 * 1024) {
        newSize = (double)size / (double)1024;
        unit = tr("kB");
    } else if (size < 1024 * 1024 * 1024) {
        newSize = (double)size / (double)(1024 * 1024);
        unit = tr("MB");
    } else {
        newSize = (double)size / (double)(1024 * 1024 * 1024);
        unit = tr("GB");
    }

    return QString(QStringLiteral("%1 %2")).arg(newSize, 0, 'f', 1).arg(unit);
}

DownloadModel::DownloadModel(DownloadManager *downloadManager, QObject *parent)
    : QAbstractListModel(parent)
    , m_downloadManager(downloadManager)
{
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();
    if (role == Qt::ToolTipRole)
        if (!m_downloadManager->m_downloads.at(index.row())->downloadedSuccessfully())
            return m_downloadManager->m_downloads.at(index.row())->downloadInfoLabel->text();
    return QVariant();
}

int DownloadModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : m_downloadManager->m_downloads.count();
}

bool DownloadModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    int lastRow = row + count - 1;
    for (int i = lastRow; i >= row; --i) {
        if (m_downloadManager->m_downloads.at(i)->downloadedSuccessfully()
                || m_downloadManager->m_downloads.at(i)->tryAgainButton->isEnabled()) {
            beginRemoveRows(parent, i, i);
            m_downloadManager->m_downloads.takeAt(i)->deleteLater();
            endRemoveRows();
        }
    }
    m_downloadManager->m_autoSaver->changeOccurred();
    m_downloadManager->updateItemCount();
    return true;
}

Qt::ItemFlags DownloadModel::flags(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return Qt::ItemFlags();

    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    DownloadItem *item = m_downloadManager->m_downloads.at(index.row());
    if (item->downloadedSuccessfully())
        return defaultFlags | Qt::ItemIsDragEnabled;

    return defaultFlags;
}

QMimeData *DownloadModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urls;
    Q_FOREACH (const QModelIndex &index, indexes) {
        if (!index.isValid())
            continue;
        DownloadItem *item = m_downloadManager->m_downloads.at(index.row());
        urls.append(QUrl::fromLocalFile(item->m_file.absoluteFilePath()));
    }
    mimeData->setUrls(urls);
    return mimeData;
}

