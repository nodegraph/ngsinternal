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

// Helper which wraps the input url with things like http://.
// Webdriver needs proper urls to navigate.
QUrl get_proper_url(const QString& input);

// This class communicates with the nodejs process.
class COMMS_EXPORT AppWorker : public QObject, Component {
Q_OBJECT
 public:
  COMPONENT_ID(AppWorker, AppWorker)

  typedef std::function<void()> AppTask;

  explicit AppWorker(Entity* parent);
  virtual ~AppWorker();

  // Our state.
  virtual void initialize_fixed_deps();

  static QString get_app_bin_dir();
  static QString get_user_data_dir();
  Q_INVOKABLE QString get_smash_browse_url();

  // Polling to socket connections open and browser open.
  static const int kPollInterval;
  Q_INVOKABLE bool is_polling();
  Q_INVOKABLE void start_polling();
  Q_INVOKABLE void stop_polling();
  Q_INVOKABLE void make_browser_visible() {_show_browser = true;}

  // Task queue info.
  Q_INVOKABLE bool is_busy() {return !_queue.empty();}

  // ---------------------------------------------------------------------------------
  // Browser Actions.
  // ---------------------------------------------------------------------------------


  Q_INVOKABLE void open_browser();
  Q_INVOKABLE void close_browser();
  Q_INVOKABLE void check_browser_is_open();
  Q_INVOKABLE void check_browser_size();

  // Helpers to shutdown app.
  Q_INVOKABLE void shutdown();
  Q_INVOKABLE void reset();

  // ---------------------------------------------------------------------------------
  // Web Actions. These calls will put information into the _last_resp member.
  //              They are bound to make MessageHandlers, and can make use of _last_resp.
  // ---------------------------------------------------------------------------------

  // Test features.
  Q_INVOKABLE void get_all_cookies();
  Q_INVOKABLE void clear_all_cookies();
  Q_INVOKABLE void set_all_cookies();
  Q_INVOKABLE void update_overlays();
  Q_INVOKABLE void cache_frame();
  Q_INVOKABLE void load_cached_frame();

  // Navigation.
  Q_INVOKABLE void navigate_to(const QString& url);
  Q_INVOKABLE void navigate_refresh();
  Q_INVOKABLE void switch_to_iframe();

  // Create set by matching values.
  Q_INVOKABLE void create_set_by_matching_text();
  Q_INVOKABLE void create_set_by_matching_images();

  // Create set by type.
  Q_INVOKABLE void create_set_of_inputs();
  Q_INVOKABLE void create_set_of_selects();
  Q_INVOKABLE void create_set_of_images();
  Q_INVOKABLE void create_set_of_text();

  // Delete set.
  Q_INVOKABLE void delete_set();

  // Shift sets.
  Q_INVOKABLE void shift_to_text_above();
  Q_INVOKABLE void shift_to_text_below();
  Q_INVOKABLE void shift_to_text_on_left();
  Q_INVOKABLE void shift_to_text_on_right();

  Q_INVOKABLE void shift_to_images_above();
  Q_INVOKABLE void shift_to_images_below();
  Q_INVOKABLE void shift_to_images_on_left();
  Q_INVOKABLE void shift_to_images_on_right();

  Q_INVOKABLE void shift_to_inputs_above();
  Q_INVOKABLE void shift_to_inputs_below();
  Q_INVOKABLE void shift_to_inputs_on_left();
  Q_INVOKABLE void shift_to_inputs_on_right();

  Q_INVOKABLE void shift_to_selects_above();
  Q_INVOKABLE void shift_to_selects_below();
  Q_INVOKABLE void shift_to_selects_on_left();
  Q_INVOKABLE void shift_to_selects_on_right();

  Q_INVOKABLE void shift_to_iframes_above();
  Q_INVOKABLE void shift_to_iframes_below();
  Q_INVOKABLE void shift_to_iframes_on_left();
  Q_INVOKABLE void shift_to_iframes_on_right();

  // Expand sets.
  Q_INVOKABLE void expand_above();
  Q_INVOKABLE void expand_below();
  Q_INVOKABLE void expand_left();
  Q_INVOKABLE void expand_right();

  // Mark sets.
  Q_INVOKABLE void mark_set();
  Q_INVOKABLE void unmark_set();
  Q_INVOKABLE void merge_sets();

  // Shrink to one side.
  Q_INVOKABLE void shrink_set_to_topmost();
  Q_INVOKABLE void shrink_set_to_bottommost();
  Q_INVOKABLE void shrink_set_to_leftmost();
  Q_INVOKABLE void shrink_set_to_rightmost();

  // Shrink against marked sets.
  Q_INVOKABLE void shrink_above_of_marked();
  Q_INVOKABLE void shrink_below_of_marked();
  Q_INVOKABLE void shrink_above_and_below_of_marked();
  Q_INVOKABLE void shrink_left_of_marked();
  Q_INVOKABLE void shrink_right_of_marked();
  Q_INVOKABLE void shrink_left_and_right_of_marked();

  // Block/Unblock events in the browser content scripts.
  Q_INVOKABLE void block_events();
  Q_INVOKABLE void unblock_events();

  // Perform web actions.
  Q_INVOKABLE void click();
  Q_INVOKABLE void mouse_over();
  Q_INVOKABLE void start_mouse_hover();
  Q_INVOKABLE void stop_mouse_hover();
  Q_INVOKABLE void type_text(const QString& text);
  Q_INVOKABLE void type_enter();
  Q_INVOKABLE void extract_text();
  Q_INVOKABLE void get_option_texts(); // Results will be emitted by signal.
  Q_INVOKABLE void select_from_dropdown(const QString& option_text);
  Q_INVOKABLE void scroll_down();
  Q_INVOKABLE void scroll_up();
  Q_INVOKABLE void scroll_right();
  Q_INVOKABLE void scroll_left();

  // These are meant to be called by Compute components.
  void send_msg(Message& msg);  // msg is not const because it gets tagged with an id.
  void send_action_msg(Message& msg);

signals:
  void show_web_action_menu();
  void show_iframe_menu();
  void web_action_ignored();
  void select_option_texts(QStringList option_texts);

 private slots:
  void on_text_received(const QString & text);
  void on_poll();

 private:
  void reset_state();

  // Send Messages.
  void send_json(const QString& json);
  void send_map(const QVariantMap& map);


  // Task Management.
  void queue_task(AppTask task, const std::string& about);

  // Tasks. Our members which get bound into tasks.
  void send_msg_task(Message msg);
  void get_crosshair_info_task();
  void create_set_by_matching_text_task();
  void create_set_by_matching_images_task();
  void delete_set_task();
  template<WrapType WRAP_TYPE, Direction DIR>
  void shift_set_task();
  void expand_task(Direction dir, const QVariantMap& match_criteria);
  void mark_set_task();
  void unmark_set_task();
  void shrink_set_to_side_task(Direction dir);
  void shrink_against_marked_task(QVariantList dirs);
  void perform_action_task(ActionType action, QVariantMap extra_args);
  void start_hover_task();
  void hover_task();
  void post_hover_task();
  void emit_option_texts_task();
  void reset_task();

  // Handle messages.
  void run_next_task();
  void handle_response_from_nodejs(const Message& sm);
  void handle_info_from_nodejs(const Message& msg);

  // Our fixed dependencies.
  Dep<AppComm> _app_comm;
  Dep<FileModel> _file_model;

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
  QVariantMap _hover_args;
  static const int kJitterSize;
  int _jitter;

  // Simple Request-Response Pair Tracking.
  bool _waiting_for_results;
  int _next_msg_id;  // This is one more than the max msg id issued out.
  int _expected_msg_id; // This is the next msg id we're expecting from a response message.
  Message _last_resp;  // Our last response. This can be used by handlers in our queue.
  std::deque<AppTask> _queue;

};

}

