#ifndef UTILS_H
#define UTILS_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

// Workaround: As of Qt 5.4 QtQuick does not expose QUrl::fromUserInput.
class Utils : public QObject {
    Q_OBJECT
public:
    Utils(QObject* parent = 0) : QObject(parent) { }
    Q_INVOKABLE static QUrl url_from_input(const QString& input);
};



#endif // UTILS_H
