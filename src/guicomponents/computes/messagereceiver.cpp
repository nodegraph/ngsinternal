#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/computes/nodejsworker.h>

#include <guicomponents/comms/message.h>
#include <guicomponents/computes/messagesender.h>
#include <guicomponents/computes/messagereceiver.h>
#include <guicomponents/comms/taskscheduler.h>
#include <openssl/aes.h>
#include <QtWebSockets/QWebSocket>

namespace ngs {

class Grabber : public Component {
 public:
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  Grabber(Entity* app_root)
      : Component(NULL, kIID(), kDID()),
        _app_root(app_root) {
    // Note invalid components are created with a NULL entity parent.
  }
  QWebSocket* get_web_socket() {
    Dep<MessageSender> sender = get_dep<MessageSender>(_app_root);
    return sender->get_web_socket();
  }
  void clear_web_socket() {
    Dep<MessageSender> sender = get_dep<MessageSender>(_app_root);
        return sender->clear_web_socket();
  }
 private:
  Entity* _app_root;
};

MessageReceiver::MessageReceiver(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _app_worker(this),
      _task_queue(this),
      _connected(false) {
  get_dep_loader()->register_fixed_dep(_app_worker, Path());
  get_dep_loader()->register_fixed_dep(_task_queue, Path());
}

MessageReceiver::~MessageReceiver() {
  // The MessageSender component owns the web socket.
}

void MessageReceiver::update_wires() {
  Component::update_wires();

  // Create a qt signal slot connection. Make sure this only happens once!!
  if (!_connected) {
    Grabber* temp = new_ff Grabber(get_app_root());
    _web_socket = temp->get_web_socket();
    if (_web_socket) {
      connect(_web_socket, SIGNAL(connected()), this, SLOT(on_connected()));
      connect(_web_socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
      connect(_web_socket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_error(const QList<QSslError>&)));
      connect(_web_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
      connect(_web_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(on_state_changed(QAbstractSocket::SocketState)));
      connect(_web_socket, SIGNAL(textMessageReceived(const QString &)), this, SLOT(on_text_received(const QString &)));
      _connected = true;
    }

    delete_ff(temp);
  }
}

void MessageReceiver::on_connected() {
  internal();
  qDebug() << "message receiver is now connected";
}

void MessageReceiver::on_disconnected() {
  internal();
  Grabber* temp = new_ff Grabber(get_app_root());
  temp->clear_web_socket();
  _web_socket = NULL;
  _connected = false;
  qDebug() << "message receiver is now disconnected";
}

void MessageReceiver::on_error(QAbstractSocket::SocketError error) {
  internal();
  if (error == QAbstractSocket::SocketError::SslHandshakeFailedError) {
    // We expect this error because we're using a local ssl server.
    return;
  } else if (error == QAbstractSocket::RemoteHostClosedError) {
  }
  qDebug() << "MessageSender Error: " << error;
  qDebug() << "WebSocket error string: " << _web_socket->errorString();
}

void MessageReceiver::on_ssl_error(const QList<QSslError>& errors) {
  internal();
  qDebug() << "SSLError: " << errors;
  _web_socket->ignoreSslErrors(errors);
}

void MessageReceiver::on_state_changed(QAbstractSocket::SocketState s) {
  internal();
  qDebug() << "state changed: " << s;
}

void MessageReceiver::on_text_received(const QString & text) {
  std::cerr << "msg receiver got text: -->" << text.toStdString() << "<--\n";
  Message msg(text);
  MessageType type = msg.get_msg_type();
  if (type == MessageType::kRequestMessage) {
    // We shouldn't be getting request message from nodejs.
    std::cerr << "Error: App should not be receiving request messages.\n";
    assert(false);
  } else if (type == MessageType::kResponseMessage) {
    _app_worker->handle_response(msg);
  } else if (type == MessageType::kInfoMessage) {
    _app_worker->handle_info(msg);
  } else {
    std::cerr << "Error: Got message of unknown type!\n";
  }
}

void MessageReceiver::separate_messages(const QString& text) {
  // Multiple message may arrive concatenated in the json string, if an assert fails before
  // completely handling the message and returning from the on_text_received method in a previous call.
  // To split up the messages the following logic can be used.
  QRegExp regex("(\\{[^\\{\\}]*\\})");
  QStringList splits;
  int pos = 0;
  while ((pos = regex.indexIn(text, pos)) != -1) {
      splits << regex.cap(1);
      pos += regex.matchedLength();
  }

  // Loop over each message string.
  for (int i=0; i<splits.size(); ++i) {
    std::cerr << "hub --> app-" << i << ": " << splits[i].toStdString() << "\n";
    Message msg(splits[i]);
  }
}

}
