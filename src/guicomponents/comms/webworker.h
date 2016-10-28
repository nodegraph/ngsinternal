#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

namespace ngs {

class Message;
class TaskScheduler;
class TaskContext;
class Compute;
class BaseFactory;
class BaseNodeGraphManipulator;

// This class communicates with the nodejs process.
class COMMS_EXPORT WebWorker : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(WebWorker, WebWorker)

  explicit WebWorker(Entity* parent);
  virtual ~WebWorker();

  Q_INVOKABLE void open();
  Q_INVOKABLE void close();
  Q_INVOKABLE bool is_open();
  Q_INVOKABLE void open_browser();
  Q_INVOKABLE void close_browser();
  Q_INVOKABLE void force_close_browser();
  Q_INVOKABLE void force_stack_reset();
  Q_INVOKABLE bool is_busy_cleaning();
  Q_INVOKABLE void queue_emit_option_texts();

  const QString& get_iframe_to_switch_to() {return _iframe_to_switch_to;}

  // Handle Incoming messages. Note the same messages are also handled by the AppTaskQueue.
  void handle_response(const Message& sm);
  void handle_info(const Message& msg);

  // Polling Control.
  // Polling is used to ensure that the browser is open and of the expected dimensions.
  static const int kPollInterval;
  Q_INVOKABLE bool is_polling();
  Q_INVOKABLE void start_polling();
  Q_INVOKABLE void stop_polling();
  Q_INVOKABLE void show_browser() {_show_browser = true;}
  Q_INVOKABLE void hide_browser() {_show_browser = false;}

  // ---------------------------------------------------------------------------------
  // Queue Tasks.
  // ---------------------------------------------------------------------------------

  // Queue Framework Tasks.
  void queue_get_xpath(TaskContext& tc);
  void queue_get_crosshair_info(TaskContext& tc);
  void queue_merge_chain_state(TaskContext& tc, const QJsonObject& map);
  void queue_build_compute_node(TaskContext& tc, ComponentDID compute_did);
  void queue_get_outputs(TaskContext& tc, std::function<void(const QJsonObject&)> on_get_outputs);

  // Queue Cookie Tasks.
  void queue_get_all_cookies(TaskContext& tc);
  void queue_clear_all_cookies(TaskContext& tc);
  void queue_set_all_cookies(TaskContext& tc);

  // Queue Browser Tasks.
  void queue_open_browser(TaskContext& tc);
  void queue_close_browser(TaskContext& tc);
  void queue_is_browser_open(TaskContext& tc);
  void queue_resize_browser(TaskContext& tc);
  void queue_reset(TaskContext& tc);

  // Queue Page Content Tasks.
  void queue_block_events(TaskContext& tc);
  void queue_unblock_events(TaskContext& tc);
  void queue_wait_until_loaded(TaskContext& tc);

  // Queue Navigate Tasks.
  void queue_navigate_to(TaskContext& tc);
  void queue_navigate_refresh(TaskContext& tc);
  void queue_switch_to_iframe(TaskContext& tc);

  // Queue Set Tasks.
  void queue_update_overlays(TaskContext& tc);
  void queue_create_set_by_matching_values(TaskContext& tc);
  void queue_create_set_by_matching_type(TaskContext& tc);
  void queue_delete_set(TaskContext& tc);
  void queue_shift_set(TaskContext& tc);
  void queue_expand_set(TaskContext& tc);
  void queue_mark_set(TaskContext& tc);
  void queue_unmark_set(TaskContext& tc);
  void queue_merge_sets(TaskContext& tc);
  void queue_shrink_set_to_side(TaskContext& tc);
  void queue_shrink_against_marked(TaskContext& tc);

  // Queue Perform Action Tasks.
  void queue_perform_mouse_action(TaskContext& tc);
  void queue_perform_mouse_hover(TaskContext& tc);
  void queue_perform_post_mouse_hover(TaskContext& tc);
  void queue_perform_text_action(TaskContext& tc);
  void queue_perform_element_action(TaskContext& tc);

  // Queue other actions.
  void queue_start_mouse_hover(TaskContext& tc);
  void queue_stop_mouse_hover(TaskContext& tc);
  void queue_emit_option_texts(TaskContext& tc); // Used to extract options from dropdowns and emit back to qml.

signals:
  void show_web_action_menu();
  void show_iframe_menu();
  void select_option_texts(QStringList option_texts);

 private slots:
  void on_poll();

 private:
  void reset_state();

  // -----------------------------------------------------------------------------
  // Tasks can use the _chain_state to build up new request messages to send out over the
  // socket. However note that each task should only send out 1 request message at maximum.
  // This is because each message is accompanied by a response message. Instead of sending
  // out another message another task should be queued up to send our the next requests.
  // If a task doesn't send out a request message, then it should call run_next_task
  // to keep the queue alive. When request messages are sent out the responses keep
  // the queue processing alive.
  // -----------------------------------------------------------------------------

  // Infrastructure Tasks.
  void get_crosshair_info_task();
  void get_xpath_task();
  void merge_chain_state_task(const QJsonObject& map);
  void get_outputs_task(std::function<void(const QJsonObject&)> on_finished_sequence);
  void start_sequence_task();
  void finished_sequence_task(std::function<void()> on_finished_sequence);
  void build_compute_node_task(ComponentDID compute_did);

  // Cookie Tasks.
  void get_all_cookies_task();
  void clear_all_cookies_task();
  void set_all_cookies_task();

  // Browser Tasks.
  void open_browser_task();
  void close_browser_task();
  void is_browser_open_task();
  void resize_browser_task();

  // Page Content Tasks.
  void block_events_task();
  void unblock_events_task();
  void wait_until_loaded_task();

  // Browser Reset and Shutdown Tasks.
  void shutdown_task();
  void reset_browser_task();

  // Navigation Tasks.
  void navigate_to_task();
  void navigate_refresh_task();
  void switch_to_iframe_task();

  // Set Creation/Modification Tasks.
  void update_overlays_task();
  void create_set_by_matching_values_task();
  void create_set_by_matching_type_task();
  void delete_set_task();
  void shift_set_task();
  void expand_set_task();
  void mark_set_task();
  void unmark_set_task();
  void merge_sets_task();
  void shrink_set_to_side_task();
  void shrink_against_marked_task();

  void perform_mouse_action_task();
  void perform_hover_action_task();
  void perform_text_action_task();
  void perform_element_action_task();

  void start_mouse_hover_task();
  void stop_mouse_hover_task();
  void mouse_hover_task();
  void post_hover_task();

  void emit_option_texts_task();
  void reset_task();

  // Our fixed dependencies.
  Dep<TaskScheduler> _task_sheduler;
  Dep<BaseNodeGraphManipulator> _ng_manipulator;

  // Poll timer.
  QTimer _poll_timer;
  bool _show_browser;

  // State to bring up the web actions menu, and handle menu activations.
  QString _iframe_to_switch_to;
  QJsonObject _browser_click_pos;

  // State for hovering.
  bool _hovering;
  QJsonObject _hover_state;
  static const int kJitterSize;
  int _jitter;

  // The 'value' value from responses will get merged into this state overriding previous values.
  QJsonObject _chain_state;

  // The success value of the last response.
  // Note that "success" is overview/cumulative value denoting whether the task completed its task successfully.
  // More detailed values are in the "value" part of the message.
  bool _last_response_success;
};


}

