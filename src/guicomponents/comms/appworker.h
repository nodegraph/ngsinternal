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

// Helper which wraps the input url with things like http://.
// Webdriver needs proper urls to navigate.
QUrl get_proper_url(const QString& input);

// This class communicates with the nodejs process.
class COMMS_EXPORT AppWorker : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(AppWorker, AppWorker)

  typedef std::function<void()> AppTask;

  explicit AppWorker(Entity* parent);
  virtual ~AppWorker();

  // Paths to resources.
  static QString get_app_bin_dir();
  static QString get_user_data_dir();
  Q_INVOKABLE QString get_smash_browse_url();

  // Crean a node's compute.
  void clean_compute(Dep<Compute>& compute);

  // Polling Control.
  // Polling can ensure the browser is open and of the expected dimensions.
  static const int kPollInterval;
  Q_INVOKABLE bool is_polling();
  Q_INVOKABLE void start_polling();
  Q_INVOKABLE void stop_polling();
  Q_INVOKABLE void make_browser_visible() {_show_browser = true;}

  // Task queue info.
  Q_INVOKABLE bool is_busy() {return !_queue.empty();}

  // ---------------------------------------------------------------------------------
  // Record Actions.
  // ---------------------------------------------------------------------------------

  // Record Browser Actions.
  Q_INVOKABLE void record_open_browser();
  Q_INVOKABLE void record_close_browser();
  Q_INVOKABLE void record_check_browser_is_open();
  Q_INVOKABLE void record_check_browser_size();

  // Record Navigate Actions.
  Q_INVOKABLE void record_navigate_to(const QString& url);
  Q_INVOKABLE void record_navigate_refresh();
  Q_INVOKABLE void record_switch_to_iframe();

  // Record Create Set By Matching Values.
  Q_INVOKABLE void record_create_set_by_matching_text_values();
  Q_INVOKABLE void record_create_set_by_matching_image_values();

  // Record Create Set By Type.
  Q_INVOKABLE void record_create_set_of_inputs();
  Q_INVOKABLE void record_create_set_of_selects();
  Q_INVOKABLE void record_create_set_of_images();
  Q_INVOKABLE void record_create_set_of_text();

  // Record Delete Set.
  Q_INVOKABLE void record_delete_set();

  // Record Shift Sets.
  Q_INVOKABLE void record_shift_to_text_above();
  Q_INVOKABLE void record_shift_to_text_below();
  Q_INVOKABLE void record_shift_to_text_on_left();
  Q_INVOKABLE void record_shift_to_text_on_right();

  Q_INVOKABLE void record_shift_to_images_above();
  Q_INVOKABLE void record_shift_to_images_below();
  Q_INVOKABLE void record_shift_to_images_on_left();
  Q_INVOKABLE void record_shift_to_images_on_right();

  Q_INVOKABLE void record_shift_to_inputs_above();
  Q_INVOKABLE void record_shift_to_inputs_below();
  Q_INVOKABLE void record_shift_to_inputs_on_left();
  Q_INVOKABLE void record_shift_to_inputs_on_right();

  Q_INVOKABLE void record_shift_to_selects_above();
  Q_INVOKABLE void record_shift_to_selects_below();
  Q_INVOKABLE void record_shift_to_selects_on_left();
  Q_INVOKABLE void record_shift_to_selects_on_right();

  Q_INVOKABLE void record_shift_to_iframes_above();
  Q_INVOKABLE void record_shift_to_iframes_below();
  Q_INVOKABLE void record_shift_to_iframes_on_left();
  Q_INVOKABLE void record_shift_to_iframes_on_right();

  // Record Expand Sets.
  Q_INVOKABLE void record_expand_above();
  Q_INVOKABLE void record_expand_below();
  Q_INVOKABLE void record_expand_left();
  Q_INVOKABLE void record_expand_right();

  // Record Mark Sets.
  Q_INVOKABLE void record_mark_set();
  Q_INVOKABLE void record_unmark_set();
  Q_INVOKABLE void record_merge_sets();

  // Record Shrink To One Side.
  Q_INVOKABLE void record_shrink_set_to_topmost();
  Q_INVOKABLE void record_shrink_set_to_bottommost();
  Q_INVOKABLE void record_shrink_set_to_leftmost();
  Q_INVOKABLE void record_shrink_set_to_rightmost();

  // Record Shrink Against Marked Sets.
  Q_INVOKABLE void record_shrink_above_of_marked();
  Q_INVOKABLE void record_shrink_below_of_marked();
  Q_INVOKABLE void record_shrink_above_and_below_of_marked();
  Q_INVOKABLE void record_shrink_left_of_marked();
  Q_INVOKABLE void record_shrink_right_of_marked();
  Q_INVOKABLE void record_shrink_left_and_right_of_marked();

  // Record Mouse Actions.
  Q_INVOKABLE void record_click();
  Q_INVOKABLE void record_mouse_over();
  Q_INVOKABLE void record_start_mouse_hover();
  Q_INVOKABLE void record_stop_mouse_hover();

  // Record Text Actions.
  Q_INVOKABLE void record_type_text(const QString& text);
  Q_INVOKABLE void record_type_enter();

  // Record Element Actions.
  Q_INVOKABLE void record_extract_text();
  Q_INVOKABLE void record_select_from_dropdown(const QString& option_text);
  Q_INVOKABLE void record_scroll_down();
  Q_INVOKABLE void record_scroll_up();
  Q_INVOKABLE void record_scroll_right();
  Q_INVOKABLE void record_scroll_left();

  // ---------------------------------------------------------------------------------
  // Queued Tasks.
  // ---------------------------------------------------------------------------------

  // Queue Framework Tasks.
  void queue_get_xpath();
  void queue_get_crosshair_info();
  void queue_merge_chain_state(const QVariantMap& map);
  void queue_build_compute_node(ComponentDID compute_did);
  void queue_finished(std::function<void(const QVariantMap&)> finalize_update);

  // Queue Cookie Tasks.
  Q_INVOKABLE void queue_get_all_cookies();
  Q_INVOKABLE void queue_clear_all_cookies();
  Q_INVOKABLE void queue_set_all_cookies();

  // Queue Browser Tasks.
  void queue_open_browser();
  void queue_close_browser();
  void queue_check_browser_is_open();
  void queue_check_browser_size();
  void queue_reset();

  // Queue Page Content Tasks.
  void queue_block_events();
  void queue_unblock_events();

  // Queue Navigate Tasks.
  void queue_navigate_to();
  void queue_navigate_refresh();
  void queue_switch_to_iframe();

  // Queue Set Tasks.
  void queue_update_overlays();
  void queue_create_set_by_matching_values();
  void queue_create_set_by_matching_type();
  void queue_delete_set();
  void queue_shift_set();
  void queue_expand_set();
  void queue_mark_set();
  void queue_unmark_set();
  void queue_merge_sets();
  void queue_shrink_set_to_side();
  void queue_shrink_against_marked();

  // Queue Perform Action Tasks.
  void queue_perform_mouse_action();
  void queue_perform_mouse_hover();
  void queue_perform_post_mouse_hover();
  void queue_perform_text_action();
  void queue_perform_element_action();

  // Queue other actions.
  void queue_start_mouse_hover();
  void queue_stop_mouse_hover();
  Q_INVOKABLE void queue_emit_option_texts(); // Used to extract options from dropdowns and emit back to qml.

signals:
  void show_web_action_menu();
  void show_iframe_menu();
  void web_action_ignored();
  void select_option_texts(QStringList option_texts);

 private slots:
  void on_text_received(const QString & text);
  void on_poll();

 protected:
  // Our state.
  virtual void initialize_wires();

 private:
  void reset_state();

  // Send Messages.
//  void send_json(const QString& json);
//  void send_map(const QVariantMap& map);

  // -----------------------------------------------------------------------------
  // Tasks grab input settings from the chain_state and build
  // messages to send out from those settings.
  // -----------------------------------------------------------------------------

  // Task Management.
  void queue_task(AppTask task, const std::string& about);
  void run_next_task();
  void handle_response_from_nodejs(const Message& sm);
  void handle_info_from_nodejs(const Message& msg);

  // Infrastructure Tasks.
  void send_msg_task(Message msg);
  void get_crosshair_info_task();
  void get_xpath_task();
  void merge_chain_state_task(const QVariantMap& map);
  void finished_task(std::function<void(const QVariantMap&)> finalize_update);
  void build_compute_node_task(ComponentDID compute_did);

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
  Dep<FileModel> _file_model;
  Dep<GraphBuilder> _graph_builder;
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
  bool _waiting_for_results;
  int _next_msg_id;  // This is one more than the max msg id issued out.
  int _expected_msg_id; // This is the next msg id we're expecting from a response message.
  Message _last_resp; // The last response message received. _last_resp[value] gets copied into _chain_state.
  QVariantMap _chain_state;  // The 'value' value from responses will get merged into this state overriding previous values.
  std::deque<AppTask> _queue;

  Dep<Compute> _compute;
};

}

