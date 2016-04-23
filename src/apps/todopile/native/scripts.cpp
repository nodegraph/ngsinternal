#include <native/scripts.h>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

Scripts::Scripts()
{
}

QString Scripts::get_script(const std::string& filename) {
    QFile file(filename.c_str());
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return "";
    }
    QTextStream stream(&file);
    return stream.readAll();
}

QString Scripts::get_event_recorder_script() {
    return get_script(":/browser/eventrecorder.js");
}

QString Scripts::get_event_retriever_script() {
    return get_script(":/browser/getevent.js");
}

QString Scripts::get_error_retriever_script() {
    return get_script(":/browser/geterrors.js");
}
