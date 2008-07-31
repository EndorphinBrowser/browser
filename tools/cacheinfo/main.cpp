/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
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

#include <QtNetwork/QtNetwork>
#include <QtGui/QtGui>

class NetworkDiskCache : public QNetworkDiskCache
{
public:
    QNetworkCacheMetaData _fileMetaData(const QString &fileName)
        { return fileMetaData(fileName); }

};

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setOrganizationDomain(QLatin1String("arora-browser.org"));
    QCoreApplication::setApplicationName(QLatin1String("Arora"));

    NetworkDiskCache diskCache;
    QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
            + QLatin1String("browser/");
    diskCache.setCacheDirectory(location);

    QStringList args = application.arguments();
    args.takeFirst();
    if (args.isEmpty()) {
        QTextStream stream(stdout);
        stream << "cacheinfo [-o cachefile] [file | url]" << endl;
        return 0;
    }

    QNetworkCacheMetaData metaData;
    QString last = args.takeLast();
    if (QFile::exists(last)) {
        qDebug() << "Reading in from a file and not a url.";
        metaData = diskCache._fileMetaData(last);
    } else {
        qDebug() << "Reading in from a url and not a file.";
        metaData = diskCache.metaData(last);
    }

    if (!args.isEmpty()
        && args.count() >= 1
        && args.first() == QLatin1String("-o")) {
        QUrl url = metaData.url();
        QIODevice *device = diskCache.data(url);
        if (!device) {
            qDebug() << "Error: disk cache data is 0!";
            return 1;
        }
        QString fileName;
        if (args.count() == 2) {
            fileName = args.last();
        } else {
            QFileInfo info(url.path());
            fileName = info.fileName();
            if (fileName.isEmpty()) {
                qDebug() << "URL base name is empty, please specify an output file.";
                return 1;
            }
            if (QFile::exists(fileName)) {
                qDebug() << "File already exists, not overwriting, please specify an output file.";
                return 1;
            }
        }
        qDebug() << "Saved cache file to:" << fileName;
        QFile file(fileName);
        if (!file.open(QFile::ReadWrite))
            qDebug() << "Unable to open the file.";
        else
            file.write(device->readAll());
        delete device;
    }

    QTextStream stream(stdout);
    stream << "Url: " << metaData.url().toString() << endl;
    stream << "Expiration Date: " << metaData.expirationDate().toString() << endl;
    stream << "Last Modified Date: " << metaData.lastModified().toString() << endl;
    stream << "Save to disk: " << metaData.saveToDisk() << endl;
    stream << "Headers:" << endl;
    foreach(QNetworkCacheMetaData::RawHeader header, metaData.rawHeaders())
        stream << "\t" << header.first << ": " << header.second << endl;
    QIODevice *device = diskCache.data(metaData.url());
    if (device) {
        stream << "Data Size: " << device->size() << endl;
        stream << "First line: " << device->readLine(100);
    } else {
        stream << "No data? corrupt or error" << endl;
    }

    delete device;
    return 0;
}

