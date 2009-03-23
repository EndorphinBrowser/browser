/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#include <qtimer.h>
#include <qtextstream.h>
#include <qcoreapplication.h>
#include <qstringlist.h>
#include <qdebug.h>
#include <qnetworkaccessmanager.h>

#include "googlesuggest.h"

class Suggest : public GoogleSuggest
{
    Q_OBJECT
public:
    Suggest(QObject *parent = 0);

private slots:
    void suggestions(const QStringList &list);
    void error(const QString &error = QString());
};

Suggest::Suggest(QObject *parent)
    : GoogleSuggest(parent)
{
    setNetworkAccessManager(new QNetworkAccessManager(this));
    connect(this, SIGNAL(suggestions(const QStringList &, const QString &)),
            this, SLOT(suggestions(const QStringList &)));
    connect(this, SIGNAL(error(const QString&)),
            this, SLOT(error(const QString&)));
    QTimer::singleShot(5000, this, SLOT(error()));
}

void Suggest::suggestions(const QStringList &list)
{
    QTextStream out(stdout);
    out << list.join("\n") << endl;
    qApp->exit(0);
}

void Suggest::error(const QString &errorMessage)
{
    QTextStream out(stderr);
    if (errorMessage.isEmpty())
        out << "Unknown error getting suggestions, timedout." << endl;
    else
        out << "Error:" << errorMessage << endl;
    qApp->exit(1);
}

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);

    Suggest suggest;
    if (application.arguments().count() > 1) {
        suggest.suggest(application.arguments().value(1));
    } else {
        qWarning() << "Usage: googlesuggest [word]";
        return 1;
    }

    return application.exec();
}

#include "main_googlesuggest.moc"

