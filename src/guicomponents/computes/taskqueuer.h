#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/comms/message.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

namespace ngs {

class Message;
class MessageSender;
class TaskScheduler;
class TaskContext;
class Compute;
class BaseFactory;
class BaseNodeGraphManipulator;

class GUICOMPUTES_EXPORT TaskQueuer : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(TaskQueuer, TaskQueuer)

  static const int kWaitInterval;

  explicit TaskQueuer(Entity* parent);
  virtual ~TaskQueuer();

  // Socket Connections.
  Q_INVOKABLE void open();
  Q_INVOKABLE void close();

  // Browser.
  Q_INVOKABLE void open_browser();
  Q_INVOKABLE void close_browser();
  Q_INVOKABLE void force_close_browser();

  // Web Driver Service.
  Q_INVOKABLE void queue_stop_service();

  // Handle Incoming messages.
  void handle_response(const Message& sm);
  void handle_info(const Message& msg);

  // Get internal state.
  Q_INVOKABLE QJsonObject get_last_click_info() {return _last_click_info;} // Qt doesn't like getting references to QJsonObjects.
  const QJsonObject& get_last_click_info() const {return _last_click_info;}
  const std::deque<QJsonObject>& get_last_results() const {return _last_results;}

  // ---------------------------------------------------------------------------------
  // Queue Tasks.
  // ---------------------------------------------------------------------------------

  // Most tasks are about sending messages to the WebDriver or the chrome extension.
  void queue_send_msg(TaskContext& tc, const Message& msg, bool cancelable = true, const std::string& notes = "send msg");

  // Browser side tasks that are used often from browser nodes.
  // These could be broken out later.
  void queue_wait_for_chrome_connection(TaskContext& tc);
  void queue_open_browser(TaskContext& tc);
  void queue_update_current_tab(TaskContext& tc);

  // System side tasks. These can be broken out later.
  void queue_wait(TaskContext& tc);

signals:
  void show_web_action_menu();
  void select_option_texts(QStringList option_texts);

 private slots:
  void on_done_wait();

 private:

  // Socket Messaging Tasks.
  void send_msg_task(Message& msg);

  // Infrastructure Tasks.
  void start_sequence_task();
  void finished_sequence_task(std::function<void()> on_finished_sequence);

  // Browser Side Tasks.
  void wait_for_chrome_connection_task();

  // System Side Tasks.
  void wait_task();

  // After tasks finished, their results get cached.
  void cache_results(const QJsonObject& results);

  // Our fixed dependencies.
  Dep<MessageSender> _msg_sender;
  Dep<TaskScheduler> _scheduler;
  Dep<BaseNodeGraphManipulator> _manipulator;

  // Our wait timer.
  QTimer _wait_timer;

  // State from the right click in the browser window.
  // This is used to fill in various parameters when creating new nodes.
  QJsonObject _last_click_info;

  // The results from responses will get merged into this state overriding previous values.
  static const size_t kMaxLastResults = 10;
  std::deque<QJsonObject> _last_results;

  // The last query we sent.
  Message _last_query;
};


}

