#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

class QWebSocket;

namespace ngs {

class FileModel;

// This class communicates with the nodejs process.
class COMMS_EXPORT NodeJSProcess : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(NodeJSProcess, NodeJSProcess)

  explicit NodeJSProcess(Entity* parent);
  virtual ~NodeJSProcess();

  void start_process();
  void stop_process();
  bool is_running() const;
  const QString& get_nodejs_port() const {external(); return _nodejs_port;}

 protected:

 private slots:
  // Slots for handling nodejs process.
  void on_started();
  void on_error(QProcess::ProcessError error);
  void on_state_changed(QProcess::ProcessState);
  void on_read_standard_error();
  void on_read_standard_output();

 private:



  // Our fixed dependencies.


  //Our nodejs process.
  QProcess* _process;

  // The nodejs server's port number for us, the app.
  QString _nodejs_port;
  QRegExp _nodejs_port_regex;
};

}

