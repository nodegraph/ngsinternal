#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include "fbdemo.h"
#include "helper.h"



int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Helper helper;
    QTimer::singleShot(0, &helper, SLOT(processArgs()));
    return a.exec();
}

