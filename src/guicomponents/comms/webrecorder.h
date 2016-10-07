#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/comms/message.h>

#include <functional>

namespace ngs {

class FileModel;
class Compute;
class BaseFactory;
class WebWorker;
class TaskScheduler;

// This class communicates with the nodejs process.
class COMMS_EXPORT WebRecorder : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(WebRecorder, WebRecorder)

  explicit WebRecorder(Entity* parent);
  virtual ~WebRecorder();

  // Asynchronous Component Cleaning.
  void clean_compute(Entity* compute_entity);
  void clean_compute(Dep<Compute>& compute);
  void continue_cleaning_compute();

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
    void on_node_built(Entity* node, Compute* compute);
    void on_finished_sequence();

  // Our fixed dependencies.
  Dep<WebWorker> _app_worker;
  Dep<TaskScheduler> _task_queue;
  Dep<BaseFactory> _factory;
  Dep<FileModel> _file_model;

  // Compute that is currently being cleaned.
  Dep<Compute> _compute;

  std::function<void(Entity*,Compute*)> _on_node_built;
  std::function<void()> _on_finished_sequence;
  std::function<void()> _on_empty_stack;
};

}

