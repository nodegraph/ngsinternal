#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/comms/message.h>

#include <QtCore/QObject>

namespace ngs {

class FileModel;
class Compute;
class BaseFactory;
class TaskQueuer;
class TaskScheduler;
class BaseNodeGraphManipulator;

class GUICOMPUTES_EXPORT BrowserRecorder : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(BrowserRecorder, BrowserRecorder)

  explicit BrowserRecorder(Entity* parent);
  virtual ~BrowserRecorder();

  // ---------------------------------------------------------------------------------
  // Record Actions.
  // ---------------------------------------------------------------------------------

  // Browser actions.
  Q_INVOKABLE void record_open_browser();
  Q_INVOKABLE void record_close_browser();
  Q_INVOKABLE void record_release_browser();
  Q_INVOKABLE void record_is_browser_open();
  Q_INVOKABLE void record_resize_browser();
  Q_INVOKABLE void record_get_browser_size();
  Q_INVOKABLE void record_get_title();
  Q_INVOKABLE void record_destroy_current_tab();
  Q_INVOKABLE void record_open_tab();
  Q_INVOKABLE void record_accept_save_dialog();
  Q_INVOKABLE void record_download_video();
  Q_INVOKABLE void record_download_image();

  // Navigate the browser.
  Q_INVOKABLE void record_navigate_to(const QString& url);
  Q_INVOKABLE void record_navigate_refresh();
  Q_INVOKABLE void record_navigate_back();
  Q_INVOKABLE void record_navigate_forward();
  Q_INVOKABLE void record_get_url();

  // Record Mouse Actions.
  Q_INVOKABLE void record_click();
  Q_INVOKABLE void record_ctrl_click();
  Q_INVOKABLE void record_mouse_over();

  // Record Text Actions.
  Q_INVOKABLE void record_type_text(const QString& text);
  Q_INVOKABLE void record_type_enter();
  Q_INVOKABLE void record_type_password(const QString& text);

  // Record Element Actions.
  Q_INVOKABLE void record_select_from_dropdown(const QString& option_text);
  Q_INVOKABLE void record_scroll_down();
  Q_INVOKABLE void record_scroll_up();
  Q_INVOKABLE void record_scroll_right();
  Q_INVOKABLE void record_scroll_left();

  // Set Element.
  Q_INVOKABLE void record_get_all_elements();

  Q_INVOKABLE void record_filter_by_type();
  Q_INVOKABLE void record_filter_by_value();
  Q_INVOKABLE void record_filter_by_position();
  Q_INVOKABLE void record_filter_by_dimensions();
  Q_INVOKABLE void record_filter_by_viewport();
  Q_INVOKABLE void record_find_closest();
  Q_INVOKABLE void record_filter_to_leftmost();
  Q_INVOKABLE void record_filter_to_rightmost();
  Q_INVOKABLE void record_filter_to_topmost();
  Q_INVOKABLE void record_filter_to_bottommost();

  Q_INVOKABLE void record_filter_by_index();
  Q_INVOKABLE void record_join_elements();
  Q_INVOKABLE void record_highlight_elements();


  signals:
    void web_action_ignored();

 protected:
    virtual void initialize_wires();

 private:
    void on_empty_stack();
    void build_web_node(ComponentDID compute_did, const QVariantMap& chain_state);

  // Our fixed dependencies.
  Dep<TaskQueuer> _queuer;
  Dep<TaskScheduler> _scheduler;
  Dep<FileModel> _model;
  Dep<BaseNodeGraphManipulator> _manipulator;
};

}

