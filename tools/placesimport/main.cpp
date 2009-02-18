/*
 * Copyright 2008-2009 Benjamin K. Stuhl <bks24@cornell.edu>
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

#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QVariant>

#include <QtGui/QDesktopServices>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>

static const unsigned int HISTORY_VERSION = 23;

static QByteArray formatEntry(QByteArray url, QByteArray title, qlonglong prdate)
{
    QDateTime dateTime = QDateTime::fromTime_t(prdate / 1000000);
    dateTime.addMSecs((prdate % 1000000) / 1000);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << HISTORY_VERSION << QString::fromAscii(url) << dateTime << QString::fromUtf8(title);

    return data;
}

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setOrganizationDomain(QLatin1String("arora-browser.org"));
    QCoreApplication::setApplicationName(QLatin1String("Arora"));

    QStringList args = application.arguments();
    args.takeFirst();
    if (args.isEmpty()) {
        QTextStream stream(stdout);
        stream << "arora-placesimport is a tool for importing browser history from Firefox 3 and up" << endl;
        stream << "arora-placesinfo ~/.mozilla/firefox/[profile-dir]/places.sqlite" << endl;
        return 0;
    }

    QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    if (!QFile::exists(directory)) {
        QDir dir;
        dir.mkpath(directory);
    }

    QFile historyFile(directory + QLatin1String("/history"));
    if (!historyFile.open(QFile::Append))
        qFatal("unable to open Arora history file: %s", qPrintable(historyFile.errorString()));
    QDataStream history(&historyFile);

    QSqlDatabase placesDatabase = QSqlDatabase::addDatabase("QSQLITE");
    placesDatabase.setDatabaseName(args.first());

    if (!placesDatabase.open())
        qFatal("unable to open Firefox Places database: %s", qPrintable(placesDatabase.lastError().text()));

    QSqlQuery historyQuery(
        "SELECT moz_places.url, moz_places.title, moz_historyvisits.visit_date "
        "FROM moz_places, moz_historyvisits "
        "WHERE moz_places.id = moz_historyvisits.place_id;");
    historyQuery.setForwardOnly(true);

    if (!historyQuery.exec())
        qFatal("unable to extract Firefox history: %s", qPrintable(historyQuery.lastError().text()));

    while (historyQuery.next()) {
        QByteArray url = historyQuery.value(0).toByteArray();
        QByteArray title = historyQuery.value(1).toByteArray();
        qlonglong prdate = historyQuery.value(2).toLongLong();

        history << formatEntry(url, title, prdate);
    }
    historyFile.close();
    placesDatabase.close();

    return 0;
}

