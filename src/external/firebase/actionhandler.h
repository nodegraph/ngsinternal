#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

#include <external/firebase/firebase_export.h>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QString>
#include <firebase/datasnapshot.h>

class FIREBASE_EXPORT ActionHandler:public QObject
{
    Q_OBJECT
public:
    ActionHandler();
public slots:
    void onResponseReady(QString);
    void onDataChanged(DataSnapshot*);
private:
};

#endif // ACTIONHANDLER_H
