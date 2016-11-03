#include <QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <external/firebase/actionhandler.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ActionHandler *actionHandler=new ActionHandler();
    return a.exec();
}

