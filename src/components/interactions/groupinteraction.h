#pragma once
#include <components/interactions/interactions_export.h>
#include <base/objectmodel/component.h>
#include <components/entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <base/device/devicebasictypesgl.h>

#include <components/compshapes/hittypes.h>
#include <components/interactions/viewcontrols.h>

#include <base/device/transforms/glmhelper.h>
#include <base/device/transforms/lens.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/device/transforms/trackball.h>
#include <base/device/transforms/viewparams.h>
#include <components/interactions/viewcontrols.h>
#include <vector>
#include <map>
#include <unordered_set>



namespace ngs {

class CompShape;
class MouseInfo;
class WheelInfo;

class OutputCompute;
class InputCompute;
class BaseFactory;

// The user interactions can get quite complex.
// The GroupInteraction holds an enum which encapsulates the current state.
// State transitions are noted by the change in the enum value of the current state.

class ShapeCanvas;
class CompShapeCollective;
class TrackBall;
class ViewportParams;
class Lens;
class NodeGraphSelection;
class DotNode;
class LinkShape;
class InputShape;
class OutputShape;
class LowerHierarchyChange;


class INTERACTIONS_EXPORT GroupInteraction: public Component {
 public:

  COMPONENT_ID(GroupInteraction, GroupInteraction);

  GroupInteraction(Entity* entity);
  virtual ~GroupInteraction();

  // Our state.
  virtual void initialize_fixed_deps();
  virtual void update_state();

  // -----------------------------------------------------------------------
  // Handle all the incoming input from the user.
  // -----------------------------------------------------------------------

  // Mouse Presses.
  virtual Dep<CompShape> pressed(const MouseInfo& mouse_info);
  virtual void accumulate_select(const MouseInfo& a, const MouseInfo& b);
  virtual void reset_state();
  virtual bool bg_hit(const MouseInfo& info);
  virtual bool node_hit(const MouseInfo& info);

  // Mouse Releases.
  virtual void released(const MouseInfo& mouse_info);

  // Mouse Moves.
  virtual void moved(const MouseInfo& mouse_info);

  // Wheel Moves.
  virtual void wheel_rolled(const WheelInfo& wheel_info);

  // Pinch Zoom.
  virtual void pinch_zoom(const glm::vec2& origin, float factor);

  // Wheter it's safe to save.
  virtual bool is_safe_to_save() const;

  // Long press.
  virtual const Dep<CompShape>& get_current_hit_shape() const;
  virtual void revert_to_pre_pressed_selection();
  virtual void toggle_selection_under_press(const Dep<CompShape>& hit_shape);

  // -----------------------------------------------------------------------
  // Query current interaction state.
  // -----------------------------------------------------------------------

  virtual const MouseInfo& get_mouse_over_info() {return _mouse_over_info;}
  virtual void set_mouse_over_info(const MouseInfo& info){_mouse_over_info = info;}

  // get the amount the selection has been dragged.
  virtual glm::vec2 get_drag_delta() const;

  // mouse
  virtual const glm::mat4 get_mouse_model_view() const;

  bool is_panning_selection() const;

  // -----------------------------------------------------------------------
  // Node Graph Framing.
  // -----------------------------------------------------------------------

  virtual void select_all();
  virtual void deselect_all();
  virtual void frame_all();
  virtual void frame_selected(const DepUSet<CompShape>& selected);
  virtual void centralize(const Dep<CompShape>& node);
  virtual glm::vec2 get_center_in_object_space() const;

  // Collapse/Expand Selection.
  void collapse_selected();
  void explode_selected();
  void collapse(const DepUSet<CompShape>& selected);
  void explode(const Dep<CompShape>& group);

  void resize_gl(GLsizei width, GLsizei height);
  const ViewportParams& get_viewport_params() const;
  const glm::mat4& get_model_view() const;
  const glm::mat4& get_last_model_view() const;
  const glm::mat4& get_projection() const;
  glm::mat4 get_model_view_project() const;
  glm::mat4 get_last_model_view_project() const;

  void update_mouse_info(MouseInfo& info) const {
    _view_controls.update_coord_spaces(info);
  }

  Entity* create_node(size_t did);

 private:

  Entity* create_link();
  void destroy_link(Entity* entity);
  bool has_link(Entity* entity) const;

  Dep<LinkShape> find_link(const Dep<InputCompute>& input_compute);

  ViewControls _view_controls;

  // Our fixed deps.
  Dep<BaseFactory> _factory;
  Dep<NodeGraphSelection> _selection;
  Dep<CompShapeCollective> _shape_collective;
  Dep<LowerHierarchyChange> _lower_change;

  // Interactive dragging state.
  Dep<LinkShape> _link_shape; // The head of the link we're dragging out.
  Dep<InputCompute> _link_input_compute;
  Dep<OutputCompute> _link_output_compute;

  // A cached reference to the links folder entity which houses all our links.
  Entity* _links_folder;

  // Auxiliary data of our finite machine.
  glm::vec2 _mouse_down_pos;
  glm::vec2 _mouse_up_pos;

  DepUnorderedMap<CompShape, glm::vec2> _mouse_down_node_positions;
  //std::unordered_map<Dep<CompShape>,glm::vec2> _mouse_down_node_positions; // Position of nodes at mouse down.

  // Nodes that were previously selected, before any changes to selection on mouse down.
  DepUSet<CompShape> _preselection;

  // These are our interaction states.
  // Note that we don't have one for panning and zooming, as that's available in all states by just using the middle mouse button.
  enum InteractionState {
    kBottomLeftCornerResizing,
    kBottomRightCornerResizing,
    kTopLeftCornerResizing,
    kTopRightCornerResizing,
    kLeftEdgeResizing,
    kRightEdgeResizing,
    kBottomEdgeResizing,
    kTopEdgeResizing,
    kNodeSelectionAndDragging, // when nodes are selected and we may be dragging them soon.
    kDraggingLinkTail, // when we're dragging the tail end of a link.
    kDraggingLinkHead, // when we're dragging the head end of a link.
    kTwoClickInputToOutput, // when we've clicked once on an output plug and are awaiting a click on an input plug to complete the link.
    kTwoClickOutputToInput, // when we've clicked once on an input plug and are awaiting a click on an output plug to complete the link.
  };
  InteractionState _state;

//  Entity* _resizing_selectable; // used when resizing a sticky note
//  glm::vec2 _mouse_down_aux; // used when resizing a sticky note
//  glm::vec2 _mouse_down_anchor; // used when resizing a sticky note

  // Current Mouse Tracking Info.
  MouseInfo _mouse_over_info;
  bool _mouse_is_down;

  // Hit info.
  Dep<CompShape> _hit_shape;
  HitRegion _hit_region;
  MouseInfo _hit_mouse_info;

  // Panning.
  bool _panning_selection;

};

}
