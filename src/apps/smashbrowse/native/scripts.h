#ifndef SCRIPTS_H
#define SCRIPTS_H

#include <QtCore/QObject>

class Scripts : public QObject
{
    Q_OBJECT
public:
    Scripts();

    Q_INVOKABLE QString get_event_recorder_script();
    Q_INVOKABLE QString get_event_retriever_script();
    Q_INVOKABLE QString get_error_retriever_script();

    Q_INVOKABLE QString get_start_recording_script();
    Q_INVOKABLE QString get_stop_recording_script();

    Q_INVOKABLE QString get_jquery();
    Q_INVOKABLE QString get_jquery_simulate();

    Q_INVOKABLE void test();

signals:
  void test_signal();


private:
    QString get_script(const std::string& filename);
};

#endif // SCRIPTS_H
