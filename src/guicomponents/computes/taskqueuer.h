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

  explicit TaskQueuer(Entity* parent);
  virtual ~TaskQueuer();

  Q_INVOKABLE void open();
  Q_INVOKABLE void close();
  //Q_INVOKABLE bool is_open();
  Q_INVOKABLE void open_browser();
  Q_INVOKABLE void close_browser();
  Q_INVOKABLE void force_close_browser();
  Q_INVOKABLE void force_stack_reset();
  Q_INVOKABLE bool is_busy_cleaning();
  Q_INVOKABLE bool current_task_is_cancelable();
  Q_INVOKABLE bool is_waiting_for_response();

  Q_INVOKABLE void queue_stop_service();
  Q_INVOKABLE void queue_emit_option_texts();

  // Handle Incoming messages. Note the same messages are also handled by the AppTaskQueue.
  void handle_response(const Message& sm);
  void handle_info(const Message& msg);
  const Message& get_last_response() const{external(); return _last_response;}
  const QJsonObject& get_last_click_info() const {return _last_click_info;}
  const std::deque<QJsonObject>& get_last_results() const {return _last_results;}

  // Polling Control.
  // Polling is used to ensure that the browser is open and of the expected dimensions.
  static const int kPollInterval;
  static const int kWaitInterval;
  bool is_polling();
  void start_polling();
  void stop_polling();

  // ---------------------------------------------------------------------------------
  // Queue Tasks.
  // ---------------------------------------------------------------------------------

  void queue_send_msg(TaskContext& tc, const Message& msg, bool cancelable = true);

  // Queue Element Tasks.
  void queue_scroll_element_into_view(TaskContext& tc);

  // Queue Framework Tasks.
  void queue_build_compute_node(TaskContext& tc, ComponentDID compute_did, const QJsonObject& params = QJsonObject());
  void queue_receive_results(TaskContext& tc, std::function<void(const std::deque<QJsonObject>&)> receive_results);
  void queue_reset(TaskContext& tc);

  // Queue Browser Tasks.
  void queue_wait_for_chrome_connection(TaskContext& tc);
  void queue_open_browser(TaskContext& tc);

  void queue_update_current_tab(TaskContext& tc);

  // Queue Page Content Tasks.
  void queue_wait(TaskContext& tc);

  // Queue other actions.
  void queue_emit_option_texts(TaskContext& tc); // Used to extract options from dropdowns and emit back to qml.

signals:
  void show_web_action_menu();
  void select_option_texts(QStringList option_texts);

 private slots:
  void on_poll();
  void on_done_wait();

 private:
  void reset_state();

  // Socket Messaging Tasks.
  void send_msg_task(Message& msg);

  // Element Tasks.
  void scroll_element_into_view_task();

  // Infrastructure Tasks.
  void receive_results_task(std::function<void(const std::deque<QJsonObject>&)> on_finished_sequence);
  void start_sequence_task();
  void finished_sequence_task(std::function<void()> on_finished_sequence);
  void build_compute_node_task(ComponentDID compute_did, const QJsonObject& params);
  void reset_task();

  // Browser Tasks.
  void wait_for_chrome_connection_task();

  // Page Content Tasks.
  void wait_task();

  void emit_option_texts_task();
  void cache_results(const QJsonObject& results);

  // Our fixed dependencies.
  Dep<MessageSender> _msg_sender;
  Dep<TaskScheduler> _scheduler;
  Dep<BaseNodeGraphManipulator> _manipulator;

  // Poll timer.
  QTimer _poll_timer;
  QTimer _wait_timer;

  // State from the right click in the browser window.
  // This is used to fill in various parameters when creating new nodes.
  QJsonObject _last_click_info;

  // The results from responses will get merged into this state overriding previous values.
  static const size_t kMaxLastResults = 10;
  std::deque<QJsonObject> _last_results;

  // The last query we sent.
  Message _last_query;

  // The last response we got.
  Message _last_response;
};


}

