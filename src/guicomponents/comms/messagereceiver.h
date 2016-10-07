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

class AppWorker;
class AppTaskQueue;

// This class communicates with the nodejs process.
class COMMS_EXPORT MessageReceiver : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(MessageReceiver, MessageReceiver)

  explicit MessageReceiver(Entity* parent);
  virtual ~MessageReceiver();

 private slots:
  void on_text_received(const QString & text);

 protected:
  // Our state.
  virtual void initialize_wires();

 private:
  // Debug methods.
  void separate_messages(const QString& text);

  // Our fixed dependencies.
  Dep<AppWorker> _app_worker;
  Dep<AppTaskQueue> _task_queue; // The node js process we want to connect to.

  QWebSocket* _web_socket; // Borrowed reference.
  bool _connected; // Whether our slot is connected to receive text from the socket.
};

}

