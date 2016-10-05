#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/comms/message.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

#include <functional>
#include <deque>

namespace ngs {

class AppComm;
class FileModel;
class GraphBuilder;
class Compute;
class BaseFactory;
class NodeSelection;

class TaskContext;

typedef std::function<void()> AppTask;

// This class communicates with the nodejs process.
class COMMS_EXPORT AppWorker : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(AppWorker, AppWorker)

  explicit AppWorker(Entity* parent);
  virtual ~AppWorker();

  void set_empty_stack_callback(std::function<void()> callback);

  // Paths to resources.
  static QString get_app_bin_dir();
  static QString get_user_data_dir();
  Q_INVOKABLE QString get_smash_browse_url();

  // Polling Control.
  // Polling can ensure the browser is open and of the expected dimensions.
  static const int kPollInterval;
  Q_INVOKABLE bool is_polling();
  Q_INVOKABLE void start_polling();
  Q_INVOKABLE void stop_polling();
  Q_INVOKABLE void make_browser_visible() {_show_browser = true;}

  // Task queue info.
  Q_INVOKABLE bool is_busy() {return !_stack.empty();}


  // ---------------------------------------------------------------------------------
  // Queued Tasks.
  // ---------------------------------------------------------------------------------

  // Queue Framework Tasks.
  void queue_get_xpath(TaskContext& tc);
  void queue_get_crosshair_info(TaskContext& tc);
  void queue_merge_chain_state(TaskContext& tc, const QVariantMap& map);
  void queue_build_compute_node(TaskContext& tc, ComponentDID compute_did, std::function<void(Entity*,Compute*)> on_node_built);
  void queue_get_outputs(TaskContext& tc, std::function<void(const QVariantMap&)> on_get_outputs);
 private:
  // Special queue methods which control the stack of queues.
  void queue_start_sequence(TaskContext& tc);
  void queue_finished_sequence(TaskContext& tc, std::function<void()> on_finished_sequence);
 public:

  // Queue Cookie Tasks.
  void queue_get_all_cookies(TaskContext& tc);
  void queue_clear_all_cookies(TaskContext& tc);
  void queue_set_all_cookies(TaskContext& tc);

  // Queue Browser Tasks.
  void queue_open_browser(TaskContext& tc);
  void queue_close_browser(TaskContext& tc);
  void queue_check_browser_is_open(TaskContext& tc);
  void queue_check_browser_size(TaskContext& tc);
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
  void on_text_received(const QString & text);
  void on_poll();

 protected:
  // Our state.
  virtual void initialize_wires();

 private:
  void reset_state();

  // -----------------------------------------------------------------------------
  // Tasks grab input settings from the chain_state and build
  // messages to send out from those settings.
  // -----------------------------------------------------------------------------

  // Task Management.
  typedef std::deque<AppTask> TaskQueue;
  typedef std::vector<TaskQueue> TaskQueueStack;

  TaskQueue& get_task_queue();
  void queue_task(TaskContext& tc, AppTask task, const std::string& about);
  void run_next_task();
  void handle_response_from_nodejs(const Message& sm);
  void handle_info_from_nodejs(const Message& msg);

  // Infrastructure Tasks.
  void send_msg_task(Message msg);
  void get_crosshair_info_task();
  void get_xpath_task();
  void merge_chain_state_task(const QVariantMap& map);
  void get_outputs_task(std::function<void(const QVariantMap&)> on_finished_sequence);
  void start_sequence_task();
  void finished_sequence_task(std::function<void()> on_finished_sequence);
  void build_compute_node_task(ComponentDID compute_did, std::function<void(Entity* node, Compute* compute)> on_node_built);

  // Cookie Tasks.
  void get_all_cookies_task();
  void clear_all_cookies_task();
  void set_all_cookies_task();

  // Browser Tasks.
  void open_browser_task();
  void close_browser_task();
  void check_browser_is_open_task();
  void check_browser_size_task();

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
  Dep<AppComm> _app_comm;
  Dep<BaseFactory> _factory;
  Dep<NodeSelection> _node_selection;

  // Poll timer.
  QTimer _poll_timer;
  bool _show_browser;

  // Whether we're connected for receiving text messages from the socket.
  bool _connected;

  // State to bring up the web actions menu, and handle menu activations.
  QString _iframe;
  QVariantMap _click_pos;

  // State for hovering.
  bool _hovering;
  QVariantMap _hover_state;
  static const int kJitterSize;
  int _jitter;

  // Simple Request-Response Pair Tracking.
  bool _waiting_for_response;
  int _next_msg_id;  // This is one more than the max msg id issued out.
  int _expected_msg_id; // This is the next msg id we're expecting from a response message.
  Message _last_resp; // The last response message received. _last_resp[value] gets copied into _chain_state.
  QVariantMap _chain_state;  // The 'value' value from responses will get merged into this state overriding previous values.

  std::function<void()> _empty_stack_callback;

  // Task Management.
  TaskQueueStack _stack;
  friend class TaskContext;

};


class COMMS_EXPORT TaskContext {
 public:
  TaskContext(AppWorker* app_worker, std::function<void ()> on_finished_sequence = std::function<void()>()):
    app_worker(app_worker),
    on_finished_sequence(on_finished_sequence),
    stack_index(-1) {
    app_worker->queue_start_sequence(*this);
  }
  ~TaskContext(){
    app_worker->queue_finished_sequence(*this,on_finished_sequence);
  }
  AppWorker* app_worker;
  std::function<void ()> on_finished_sequence;
  size_t stack_index;
};

}

