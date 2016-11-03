#include "firebase.h"
#include <string.h>
#include <QIODevice>
#include <QBuffer>
#include <QJsonDocument>
#include <datasnapshot.h>
Firebase::Firebase(QObject *parent) :
    QObject(parent)
{

}
void Firebase::init()
{
    manager=new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
}
Firebase::Firebase(QString hostName)
{
    host=hostName;
    currentNode="";
    init();
}
void Firebase::setToken(QString token)
{
    firebaseToken=token;
}
Firebase::Firebase(QString hostName,QString child)
{
    host=hostName
            .append(child).append("/");
    latestNode=child;
    init();
}
Firebase* Firebase::child(QString childName)
{
    Firebase *childNode=new Firebase(host,childName);
    childNode->setToken(firebaseToken);
    return childNode;
}
void Firebase::open(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("Accept",
                         "text/event-stream");
    QNetworkReply *_reply = manager->get(request);
    connect(_reply, &QNetworkReply::readyRead, this, &Firebase::eventReadyRead);
    connect(_reply, &QNetworkReply::finished, this, &Firebase::eventFinished);
}
void Firebase::eventFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
    {
        QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if (!redirectUrl.isEmpty())
        {
            reply->deleteLater();
            open(redirectUrl);
            return;
        }
        reply->deleteLater();
    }
}
void Firebase::eventReadyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        QByteArray line=reply->readLine();
        if(!line.isEmpty())
        {
            QByteArray eventName=trimValue(line);
            line=reply->readAll();
            if(eventName=="put")
            {
                DataSnapshot *dataSnapshot=new DataSnapshot(line);
                emit eventDataChanged(dataSnapshot);
            }
        }
    }
    reply->readAll();
}
void Firebase::onReadyRead(QNetworkReply *reply)
{
    /*qDebug()<<"incoming data";
    qDebug()<<reply->readAll();*/
}
void Firebase::replyFinished(QNetworkReply *reply)
{
    //qDebug()<<reply->readAll();
    QString data=QString(reply->readAll());
    emit eventResponseReady(data);
}
void Firebase::setValue(QString strVal)
{
    //Json data creation
    QNetworkRequest request(buildPath(1));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    QBuffer *buffer=new QBuffer();
    buffer->open((QBuffer::ReadWrite));
    buffer->write(createJson(strVal).toUtf8());
    buffer->seek(0);
    /*
     * To be able to send "PATCH" request sendCustomRequest method is used.
     * sendCustomRequest requires a QIOdevice so QBuffer is used.
     * I had to seek 0 because it starts reading where it paused.
     */
    manager->sendCustomRequest(request,"PATCH",buffer);
    buffer->close();
}
void Firebase::getValue()
{
    QNetworkRequest request(buildPath(0));
    manager->get(request);
}
void Firebase::listenEvents()
{
    open(buildPath(0));
}
void Firebase::deleteValue()
{
    QNetworkRequest request(buildPath(0));
    manager->deleteResource(request);
}
QString Firebase::createJson(QString str)
{
    QString data=QString(QString("{").append("\"").append(latestNode).append("\"").append(":").append("\"").append(str).append("\"").append(QString("}")));
    return data;
}
QString Firebase::buildPath(int mode)
{
    QString destination="";
    if(mode)
    {
        host.replace(QString("/").append(latestNode).append("/"),"");
        destination
                .append(host)
                .append("/.json");
    }
    else
    {
        destination
                .append(host)
                .append(currentNode)
                .append(".json");

    }
    if(!firebaseToken.isEmpty())
        destination.append("?auth=").append(firebaseToken);
    return destination;

}
QByteArray Firebase::trimValue(const QByteArray &line) const
{
    QByteArray value;
    int index = line.indexOf(':');
    if (index > 0)
        value = line.right(line.size() - index  - 1);
    return value.trimmed();
}
