#pragma once
#include <QtCore/QObject>
#include <QtAndroidExtras/QAndroidJniObject>

// The JavaBridge exposes a number of java based android
// internals which are not normally accessible from qt.

namespace ngs {

class JavaBridge : public QObject
{
    Q_OBJECT
public:
    explicit JavaBridge(QObject *parent = 0);

    Q_INVOKABLE void notify(const QString& title, const QString& content);
    Q_INVOKABLE void vibrate(long milli_secs);

private:
    void acquire_activity();
    QAndroidJniObject _activity;
};

}

