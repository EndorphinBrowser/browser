/*
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
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

#ifndef OPENSEARCHENGINE_H
#define OPENSEARCHENGINE_H

#include <qpair.h>
#include <qimage.h>
#include <qmap.h>
#include <qnetworkaccessmanager.h>
#include <qstring.h>
#include <QUrl>

class QNetworkReply;
class QJSEngine;

class OpenSearchEngineDelegate;
class OpenSearchEngine : public QObject
{
    Q_OBJECT

signals:
    void imageChanged();
    void nameChanged();
    void suggestions(const QStringList &suggestions);
    void descriptionChanged();
    void searchUrlTemplateChanged();
    void searchUrlChanged();
    void providesSuggestionsChanged();
    void suggestionsUrlTemplateChanged();
    void suggestionsUrlChanged();
    void searchParametersChanged();
    void suggestionsParametersChanged();
    void searchMethodChanged();
    void suggestionsMethodChanged();
    void imageUrlChanged();
    void validChanged();
    void networkAccessManagerChanged();

public:
    typedef QPair<QString, QString> Parameter;
    typedef QList<Parameter> Parameters;

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString searchUrlTemplate READ searchUrlTemplate WRITE setSearchUrlTemplate NOTIFY searchUrlTemplateChanged)
    Q_PROPERTY(Parameters searchParameters READ searchParameters WRITE setSearchParameters NOTIFY searchParametersChanged)
    Q_PROPERTY(QString searchMethod READ searchMethod WRITE setSearchMethod NOTIFY searchMethodChanged)
    Q_PROPERTY(QString suggestionsUrlTemplate READ suggestionsUrlTemplate WRITE setSuggestionsUrlTemplate  NOTIFY suggestionsUrlTemplateChanged)
    Q_PROPERTY(Parameters suggestionsParameters READ suggestionsParameters WRITE setSuggestionsParameters NOTIFY suggestionsParametersChanged)
    Q_PROPERTY(QString suggestionsMethod READ suggestionsMethod WRITE setSuggestionsMethod NOTIFY suggestionsMethodChanged)
    Q_PROPERTY(bool providesSuggestions READ providesSuggestions  NOTIFY providesSuggestionsChanged)
    Q_PROPERTY(QString imageUrl READ imageUrl WRITE setImageUrl  NOTIFY imageUrlChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(QNetworkAccessManager *networkAccessManager READ networkAccessManager WRITE setNetworkAccessManager NOTIFY networkAccessManagerChanged)

    OpenSearchEngine(QObject *parent = nullptr);
    ~OpenSearchEngine();

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QString searchUrlTemplate() const;
    void setSearchUrlTemplate(const QString &searchUrl);
    QUrl searchUrl(const QString &searchTerm) const;

    bool providesSuggestions() const;

    QString suggestionsUrlTemplate() const;
    void setSuggestionsUrlTemplate(const QString &suggestionsUrl);
    QUrl suggestionsUrl(const QString &searchTerm) const;

    Parameters searchParameters() const;
    void setSearchParameters(const Parameters &searchParameters);

    Parameters suggestionsParameters() const;
    void setSuggestionsParameters(const Parameters &suggestionsParameters);

    QString searchMethod() const;
    void setSearchMethod(const QString &method);

    QString suggestionsMethod() const;
    void setSuggestionsMethod(const QString &method);

    QString imageUrl() const;
    void setImageUrl(const QString &url);

    QImage image() const;
    void setImage(const QImage &image);

    bool isValid() const;

    QNetworkAccessManager *networkAccessManager() const;
    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

    OpenSearchEngineDelegate *delegate() const;
    void setDelegate(OpenSearchEngineDelegate *delegate);

    bool operator==(const OpenSearchEngine &other) const;
    bool operator<(const OpenSearchEngine &other) const;

public slots:
    void requestSuggestions(const QString &searchTerm);
    void requestSearchResults(const QString &searchTerm);

protected:
    static QString parseTemplate(const QString &searchTerm, const QString &searchTemplate);
    void loadImage() const;

private slots:
    void imageObtained();
    void suggestionsObtained();

private:
    QString m_name;
    QString m_description;

    QString m_imageUrl;
    QImage m_image;

    QString m_searchUrlTemplate;
    QString m_suggestionsUrlTemplate;
    Parameters m_searchParameters;
    Parameters m_suggestionsParameters;
    QString m_searchMethod;
    QString m_suggestionsMethod;

    QMap<QString, QNetworkAccessManager::Operation> m_requestMethods;

    QNetworkAccessManager *m_networkAccessManager;
    QNetworkReply *m_suggestionsReply;

    QJSEngine *m_scriptEngine;

    OpenSearchEngineDelegate *m_delegate;
};

#endif // OPENSEARCHENGINE_H

