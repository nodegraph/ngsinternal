#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/comms/message.h>

#include <deque>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>



class QProcess;
class QWebSocket;

namespace ngs {

class FileModel;

// Workaround: As of Qt 5.4 QtQuick does not expose QUrl::fromUserInput.
class COMMS_EXPORT Utils : public QObject {
    Q_OBJECT
public:
    Utils(QObject* parent = 0) : QObject(parent) { }
    Q_INVOKABLE static QUrl url_from_input(const QString& input);
};

// This class communicates with the nodejs process.
class COMMS_EXPORT AppComm : public QObject, Component {
Q_OBJECT
 public:
  COMPONENT_ID(AppComm, AppComm)

  static const int kPollInterval;

  static QString get_app_bin_dir();
  static QString get_user_data_dir();

  explicit AppComm(Entity* parent);
  virtual ~AppComm();

  Q_INVOKABLE QVariantMap build_msg_from_json(const QString& json); // Initialize from a json string.
  Q_INVOKABLE QVariantMap build_copied_msg(const Message& other);

  Q_INVOKABLE QVariantMap build_request_msg(RequestType rt, const QVariantMap& args = QVariantMap(), const QString& xpath = "");
  Q_INVOKABLE QVariantMap build_response_msg(bool success, const QVariant& value = QVariant());
  Q_INVOKABLE QVariantMap build_info_msg(InfoType it);

  // Returns false when it is busy processing a previous command.
  // When false is returned the command will be dropped.
  // You may call the method again to try again.
  bool handle_request_from_app(const Message& sm);
  bool handle_request_from_app(const QString& json);
  Q_INVOKABLE bool handle_request_from_app(const QVariantMap& map);
  Q_INVOKABLE QString get_smash_browse_url();

  // Polling to keep browser open and of the right size.
  Q_INVOKABLE bool is_polling();
  Q_INVOKABLE void start_polling();
  Q_INVOKABLE void stop_polling();

  // Nodejs process.
  Q_INVOKABLE bool nodejs_is_connected();
  Q_INVOKABLE bool nodejs_is_running(); // Whether the nodejs process is running.

  // Show or hide browser as part of polling.
  Q_INVOKABLE void show_browser() {_show_browser = true;}
  Q_INVOKABLE void hide_browser() {_show_browser = false;}

  // Open or close browser outside of polling.
  Q_INVOKABLE void open_browser();
  Q_INVOKABLE void close_browser();

  // ---------------------------------------------------------------------------------
  // Web Actions.
  // ---------------------------------------------------------------------------------

  // Test features.
  Q_INVOKABLE void get_all_cookies();
  Q_INVOKABLE void clear_all_cookies();
  Q_INVOKABLE void set_all_cookies();
  Q_INVOKABLE void update_overlays();

  // Helpers
  Q_INVOKABLE int get_set_index();
  Q_INVOKABLE int get_overlay_index();
  Q_INVOKABLE QStringList get_option_texts();

  Q_INVOKABLE void navigate_to(const QString& url);
  Q_INVOKABLE void navigate_refresh();

  Q_INVOKABLE void create_set_by_matching_text();
  Q_INVOKABLE void create_set_by_matching_images();

  Q_INVOKABLE void create_set_of_inputs();
  Q_INVOKABLE void create_set_of_selects();
  Q_INVOKABLE void create_set_of_images();
  Q_INVOKABLE void create_set_of_text();

  Q_INVOKABLE void delete_set();

  Q_INVOKABLE void shift_set(WrapType wrap_type, Direction dir);

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

  Q_INVOKABLE void expand(Direction dir, const QVariantMap& match_criteria);
  Q_INVOKABLE void expand_above();
  Q_INVOKABLE void expand_below();
  Q_INVOKABLE void expand_left();
  Q_INVOKABLE void expand_right();

  Q_INVOKABLE void mark_set();
  Q_INVOKABLE void unmark_set();
  Q_INVOKABLE void merge_sets();

  Q_INVOKABLE void shrink_set_to_side(Direction dir);
  Q_INVOKABLE void shrink_set_to_topmost();
  Q_INVOKABLE void shrink_set_to_bottommost();
  Q_INVOKABLE void shrink_set_to_leftmost();
  Q_INVOKABLE void shrink_set_to_rightmost();

  Q_INVOKABLE void shrink_against_marked(const QVariantList& dirs);
  Q_INVOKABLE void shrink_above_of_marked();
  Q_INVOKABLE void shrink_below_of_marked();
  Q_INVOKABLE void shrink_above_and_below_of_marked();
  Q_INVOKABLE void shrink_left_of_marked();
  Q_INVOKABLE void shrink_right_of_marked();
  Q_INVOKABLE void shrink_left_and_right_of_marked();

  Q_INVOKABLE void perform_action(ActionType action);
  Q_INVOKABLE void perform_action(const QVariantMap& args);
  Q_INVOKABLE void perform_click();
  Q_INVOKABLE void type_text(const QString& text);
  Q_INVOKABLE void type_enter();
  Q_INVOKABLE void extract_text();
  Q_INVOKABLE void select_from_dropdown(const QString& option_text);

  Q_INVOKABLE void scroll_down();
  Q_INVOKABLE void scroll_up();
  Q_INVOKABLE void scroll_right();
  Q_INVOKABLE void scroll_left();




signals:
  // Fired on completion of a command.
  void command_finished(const QVariantMap& msg);
  void nodejs_connected();
  void show_web_action_menu(const QVariantMap& msg);

 private slots:
  // Slots for our timer.
  void on_poll();

  // Slots for handling nodejs process.
  void on_nodejs_started();
  void on_nodejs_error(QProcess::ProcessError error);

  // Slots for handling socket connection to the nodejs process.
  void on_connected();
  void on_disconnected();
  void on_error(QAbstractSocket::SocketError error);
  void on_ssl_error(const QList<QSslError>& errors);
  void on_state_changed(QAbstractSocket::SocketState);
  void on_json_received(const QString & json);

 private:

  // Our fixed dependencies.
  Dep<FileModel> _file_model;

  // Nodejs process.
  void start_nodejs();
  void stop_nodejs();

  // Nodejs socket connection state.
  void connect_to_nodejs();

  // Check browser state.
  void check_browser_is_open();
  void check_browser_size();

  // Handles requests originating from clicking items in the browser's context menu.
  void handle_request_from_nodejs(const Message& sm);

  // Handles responses from the browser's content script, after completing actions/tasks.
  void handle_response_from_nodejs(const Message& sm);
  void handle_info_from_nodejs(const Message& msg);

  //Our nodejs process.
  QProcess* _process;
  QWebSocket* _websocket;

  //Command execution state.
  bool _waiting_for_results;

  // Poll timer.
  QTimer _poll_timer;

  // Browser.
  bool _show_browser;

  // The nodejs server's port number for us, the app.
  QString _nodejs_port;
  QRegExp _nodejs_port_regex;

  // Our current iframe. This should actually come from data flowing through graph.
  QString _extracted_text;

  // Our last show menu message.
  Message _show_menu_msg;

};

}

