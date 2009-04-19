/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
 * Copyright 2009 Christian Franke <cfchris6@ts2server.com>
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

#ifndef OPENSEARCHDESCRIPTION_H
#define OPENSEARCHDESCRIPTION_H

#include <qhash.h>
#include <qimage.h>
#include <qstring.h>
#include <qurl.h>

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;

class OpenSearchEngine : public QObject
{
    Q_OBJECT

signals:
    void imageChanged();
    void suggestions(const QStringList &suggestions);

public:
    OpenSearchEngine(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QString searchUrl() const;
    QUrl searchUrl(const QString &searchTerm) const;
    void setSearchUrl(const QString &searchUrl);

    bool providesSuggestions() const;

    QString suggestionsUrl() const;
    QUrl suggestionsUrl(const QString &searchTerm) const;
    void setSuggestionsUrl(const QString &suggestionsUrl);

    QHash<QString, QString> searchParameters() const;
    void setSearchParameters(const QHash<QString, QString> &searchParameters);

    QHash<QString, QString> suggestionsParameters() const;
    void setSuggestionsParameters(const QHash<QString, QString> &suggestionsParameters);

    QUrl imageUrl() const;
    void setImageUrl(const QUrl &url);

    QImage image() const;
    void setImage(const QImage &image);

    bool isValid() const;

    bool operator==(const OpenSearchEngine &other) const;
    bool operator<(const OpenSearchEngine &other) const;

    QNetworkAccessManager *networkAccessManager() const;
    void setNetworkAccessManager(QNetworkAccessManager *manager);

public slots:
    void requestSuggestions(const QString &searchTerm);

protected:
    QString parseTemplate(const QString &searchTerm, QString templ) const;
    void loadImage();

private slots:
    void imageObtained();
    void suggestionsObtained();

private:
    QString m_name;
    QString m_description;

    QUrl m_imageUrl;
    QImage m_image;

    QString m_searchUrl;
    QString m_suggestionsUrl;
    QHash<QString, QString> m_searchParameters;
    QHash<QString, QString> m_suggestionsParameters;

    QNetworkAccessManager *m_networkAccessManager;

    QNetworkReply *m_suggestionsReply;
};

#endif //OPENSEARCHDESCRIPTION_H
