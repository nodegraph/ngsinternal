
#include "clients.h"

#include <qmqtt/qmqtt.h>

#include <QtCore/QCoreApplication>
#include <Qtcore/QTimer>


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    ngs::Subscriber subscriber;
    subscriber.connectToHost();

    ngs::Publisher publisher;
    publisher.connectToHost();

    return app.exec();
}
