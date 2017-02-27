#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

class QWebSocket;

namespace ngs {

class Message;
class BaseNodeGraphManipulator;

class GUICOMPUTES_EXPORT JavaProcess : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(JavaProcess, JavaProcess)

  static void kill_all_chromedriver_processes();

  explicit JavaProcess(Entity* parent);
  virtual ~JavaProcess();

  void set_msg_id(int msg_id);
  void send_msg(const Message& msg);

  void start_process(int app_server_port);
  void stop_process();
  bool is_running() const;

 protected:

 private slots:
  void on_started();
  void on_error(QProcess::ProcessError error);
  void on_state_changed(QProcess::ProcessState);
  void on_read_standard_error();
  void on_read_standard_output();

 private:
  // Our fixed dependencies.
  Dep<BaseNodeGraphManipulator> _manipulator;
  //Our process.
  QProcess* _process;
  int _msg_id;
};

}

