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
class NodeJSWorker;
class TaskScheduler;

// This class communicates with the nodejs process.
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
  Q_INVOKABLE void record_is_browser_open();
  Q_INVOKABLE void record_check_browser_size();

  // Navigate the browser.
  Q_INVOKABLE void record_navigate_to(const QString& url);
  Q_INVOKABLE void record_navigate_refresh();
  Q_INVOKABLE void record_navigate_back();
  Q_INVOKABLE void record_navigate_forward();

  // Find the first element by values.
  Q_INVOKABLE void record_find_element_by_text();
  Q_INVOKABLE void record_find_element_by_images();

  // Find the first element by type.
  Q_INVOKABLE void record_find_element_by_text_type();
  Q_INVOKABLE void record_find_element_by_image_type();
  Q_INVOKABLE void record_find_element_by_input_type();
  Q_INVOKABLE void record_find_element_by_select_type();

  // Shift the element.
  Q_INVOKABLE void record_shift_to_text_type_above();
  Q_INVOKABLE void record_shift_to_text_type_below();
  Q_INVOKABLE void record_shift_to_text_type_on_left();
  Q_INVOKABLE void record_shift_to_text_type_on_right();

  Q_INVOKABLE void record_shift_to_image_type_above();
  Q_INVOKABLE void record_shift_to_image_type_below();
  Q_INVOKABLE void record_shift_to_image_type_on_left();
  Q_INVOKABLE void record_shift_to_image_type_on_right();

  Q_INVOKABLE void record_shift_to_input_type_above();
  Q_INVOKABLE void record_shift_to_input_type_below();
  Q_INVOKABLE void record_shift_to_input_type_on_left();
  Q_INVOKABLE void record_shift_to_input_type_on_right();

  Q_INVOKABLE void record_shift_to_select_type_above();
  Q_INVOKABLE void record_shift_to_select_type_below();
  Q_INVOKABLE void record_shift_to_select_type_on_left();
  Q_INVOKABLE void record_shift_to_select_type_on_right();

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

  signals:
    void web_action_ignored();

 protected:
    virtual void initialize_wires();

 private:
    void on_empty_stack();
    void build_web_node(ComponentDID compute_did, const QVariantMap& chain_state);

  // Our fixed dependencies.
  Dep<NodeJSWorker> _worker;
  Dep<TaskScheduler> _scheduler;
  Dep<FileModel> _model;
};

}

