#pragma once
#include <QtCore/QObject>
#include <iostream>
#include <vector>
#include <deque>

#include <QtWebSockets/QWebSocket>

class QMediaPlayer;
class QMediaPlaylist;
class QProcess;

namespace ngs {

class Entity;
class Node;
class NodeGraphSelection;
class NodeGraphQuickItem;
class VariantMapTreeModel;


class CppBridge : public QObject {
Q_OBJECT
 public:
  explicit CppBridge(QObject *parent = 0);
  virtual ~CppBridge();

  Q_INVOKABLE void push_script(QString script);
  Q_INVOKABLE void start_recording();
  Q_INVOKABLE void stop_recording();
  Q_INVOKABLE void replay_last();

signals:
  void result(const QString&); // Fired after a script has been processsed.
  void recording_received(const QString&);

 public slots:

  void on_connected();
  void on_disconnected();
  void on_error(QAbstractSocket::SocketError error);
  void on_ssl_error(const QList<QSslError>& errors);
  void on_text_message_received(const QString & message);

  void on_browser_controller_started();

 private:
  void start_browser_controller();
  void close_browser_controller();
  void connect_to_browser_controller();
  void process_command();
  void fire_commands();

  bool _use_external_process;
  QProcess* _process;
  QWebSocket* _websocket;

  std::deque<QString> _commands;
  QString _result;

  // testing
  QString _script;

};

}
