#ifndef NETWORKACCESSEDITOR_H
#define NETWORKACCESSEDITOR_H

#include <qobject.h>
#include <qmap.h>
#include <qpair.h>
#include <qlist.h>
#include <qbytearray.h>
#include <qnetworkaccessmanager.h>

#include "ui_networkrequests.h"

class QDialog;
class QTreeWidgetItem;
class QSignalMapper;

class NetworkAccessEditor : public QObject
{
    Q_OBJECT

public:
    NetworkAccessEditor();
    ~NetworkAccessEditor();

    QNetworkRequest tamperRequest( QNetworkAccessManager::Operation op, const QNetworkRequest&req, QIODevice *outgoingData );
    void addRequest( QNetworkAccessManager::Operation op, const QNetworkRequest&req, QIODevice *outgoingData, QNetworkReply *reply );

    void show();
    void hide();

private slots:
    void clear();
    void requestFinished( QObject *replyObject );
    void showItemDetails( QTreeWidgetItem *item );

private:
    QDialog *dialog;
    Ui::NetworkRequestsDialog *networkRequestsDialog;
    QMap<QNetworkReply *, QNetworkRequest> requestMap;
    QMap<QTreeWidgetItem *, QNetworkRequest> itemRequestMap;
    QMap<QNetworkReply *, QTreeWidgetItem *> itemMap;
    QMap<QTreeWidgetItem *, QPair< QList<QByteArray>, QList<QByteArray> > > itemReplyMap;
    QSignalMapper *mapper;
    bool tamperingEnabled;
    bool interactiveTamperingEnabled;
};

#endif // NETWORKACCESSEDITOR_H
