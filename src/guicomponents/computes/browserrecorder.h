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

  // Record Browser Actions.
  Q_INVOKABLE void record_open_browser();
  Q_INVOKABLE void record_close_browser();
  Q_INVOKABLE void record_is_browser_open();
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

