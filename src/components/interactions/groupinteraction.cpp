#include <components/interactions/groupinteraction.h>
#include <components/interactions/shapecanvas.h>
#include <base/objectmodel/entityids.h>

#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/lens.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/device/transforms/trackball.h>
#include <base/device/transforms/viewparams.h>
#include <base/device/transforms/wheelinfo.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputlabelshape.h>
#include <components/compshapes/outputlabelshape.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/entity.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/nodeshape.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/interactions/viewcontrols.h>
#include <keyboard_codes_posix.h>
#include <iostream>

namespace ngs {

GroupInteraction::GroupInteraction(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _factory(this),
      _selection(this),
      _shape_collective(this),
      _manipulator(this),
      _link_shape(this),
      _links_folder(NULL),
      _mouse_is_down(false),
      _state(kNodeSelectionAndDragging),
      _hit_region(HitRegion::kMissedRegion),
      _panning_selection(false) {
  get_dep_loader()->register_fixed_dep(_factory, Path());
  get_dep_loader()->register_fixed_dep(_selection, Path());
  get_dep_loader()->register_fixed_dep(_shape_collective, Path({"."}));
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

GroupInteraction::~GroupInteraction(){
}

bool GroupInteraction::update_state() {
  internal();
  _links_folder = get_entity(Path({".",kLinksFolderName}));
  return true;
}

void GroupInteraction::update_shape_collective() {
  internal(); // Wire cleaning is kept orthogonal from dependency dirtiness propagation.
  our_entity()->clean_wires(); // We need to clean_wires in order for the input and output labels to update their angle/orientation.
}

void GroupInteraction::clean_dead_links() {
  internal(); // Wire cleaning is kept orthogonal from dependency dirtiness propagation.
  our_entity()->clean_dead_entities();
}

glm::vec2 GroupInteraction::get_drag_delta() const {
  external();
  MouseInfo current_info = _mouse_over_info;
  _view_controls.update_coord_spaces_with_last_model_view(current_info);
  // Compute the delta vector.
  return (const glm::vec2&) current_info.object_space_pos.xy() - _mouse_down_pos;
}

void GroupInteraction::revert_to_pre_pressed_selection() {
  external();

  // If we clicked a link shape, we don't offer this option.
  if (_link_shape) {
    return;
  }

  // Clear out the current selection.
  _selection->clear_selection();
  _mouse_down_node_positions.clear();
  // Restore the selection which existed previous to any mouse down changes.
  for (const Dep<NodeShape>& s : _preselection) {
    _selection->select(s);
  }

  // Stop all intermediate dragging variables and make sure we're in the
  // default select/drag mode where nothing has happenedd yet.
  _view_controls.track_ball.stop_tracking();
  _mouse_down_node_positions.clear();
  _state = kNodeSelectionAndDragging;
  _panning_selection = false;

  // Reset our interactive dragging state.
  _link_shape.reset();
}

void GroupInteraction::toggle_selection_under_press(const Dep<NodeShape>& hit_shape) {
  external();
  // Flip the selection.
  if (_selection->is_selected(hit_shape)) {
    _selection->toggle_selected(hit_shape);
    _mouse_down_node_positions.erase(hit_shape);
  } else {
    _selection->select(hit_shape);
    _mouse_down_node_positions[hit_shape] = hit_shape->get_pos();
  }
}

Dep<LinkShape> GroupInteraction::create_link() {
  external();
  Entity* group = our_entity();
  Entity* link = _manipulator->create_link(group);
  Dep<LinkShape> link_shape = get_dep<LinkShape>(link);
  // New link created, so we need to clean the wires. (Note a wire is differnt from a link. See Component.h)
  update_shape_collective();
  return link_shape;
}

void GroupInteraction::destroy_link(Entity* link) {
  external();
  delete_ff(link);
}

bool GroupInteraction::has_link(Entity* entity) const {
  external();
  Entity* head = entity->has_entity(Path({".","link"}));
  if (head) {
    return true;
  }
  return false;
}

void GroupInteraction::reset_state() {
  external();

  // Links may need to be cleaned up.
  get_app_root()->clean_dead_entities();

  // Reset our finite state machine
  if (_link_shape) {
    Entity* e = _link_shape->our_entity();
    delete_ff(e);
  }

  // Transition to the default state.
  _state = kNodeSelectionAndDragging;
  _panning_selection = false;

  // Reset our interactive dragging state.
  _link_shape.reset();

  // Stop the tracking.
  _view_controls.track_ball.stop_tracking();
}

Entity* GroupInteraction::entity_hit(const MouseInfo& info) const {
  external();
  MouseInfo updated_info = info;
  _view_controls.update_coord_spaces(updated_info);
  // Do a hit test.
  HitRegion region;
  Entity* e = _shape_collective->hit_test(updated_info.object_space_pos.xy(), region);
  return e;
}

bool GroupInteraction::bg_hit(const MouseInfo& info) const {
  external();
  if (!entity_hit(info))  {
    return true;
  }
  return false;
}

bool GroupInteraction::node_hit(const MouseInfo& info) const {
  external();
  MouseInfo updated_info = info;
  _view_controls.update_coord_spaces(updated_info);

  // Do a hit test.
  HitRegion region;
  Entity* e = _shape_collective->hit_test(updated_info.object_space_pos.xy(), region);
  if (!e) {
    return false;
  }
  if (region == HitRegion::kNodeRegion) {
    return true;
  }
  return false;
}

void GroupInteraction::accumulate_select(const MouseInfo& a, const MouseInfo& b) {
  external();
  MouseInfo ua = a;
  MouseInfo ub = b;
  _view_controls.update_coord_spaces(ua);
  _view_controls.update_coord_spaces(ub);

  // Do a hit test.
  HitRegion region;
  Dep<CompShape> cs_a(this);
  {
    Entity* cs_e = _shape_collective->hit_test(ua.object_space_pos.xy(), region);
    if (cs_e) {
      cs_a = get_dep<CompShape>(cs_e);
    }
  }
  Dep<CompShape> cs_b(this);
  {
    Entity* cs_e = _shape_collective->hit_test(ub.object_space_pos.xy(), region);
    if (cs_e) {
      cs_b = get_dep<CompShape>(cs_e);
    }
  }


  if (cs_a && cs_a->is_linkable()) {
    Dep<NodeShape> ls = get_dep<NodeShape>(cs_a->our_entity());
    _selection->toggle_selected(ls);
  } else if (cs_b && cs_b->is_linkable()) {
    Dep<NodeShape> ls = get_dep<NodeShape>(cs_b->our_entity());
    _selection->toggle_selected(ls);
  }
}

Dep<NodeShape> GroupInteraction::pressed(const MouseInfo& mouse_info, HitRegion& region) {
  external();
  // Record the selection state, before anything changes.
  _preselection = _selection->get_selected();

  // Update the mouse info.
  MouseInfo updated_mouse_info = mouse_info;
  _view_controls.update_coord_spaces(updated_mouse_info);

  // Do a hit test.
  region = HitRegion::kMissedRegion;
  Entity* comp_shape_entity = _shape_collective->hit_test(updated_mouse_info.object_space_pos.xy(), region);

  // Record the mouse down position.
  _mouse_down_pos = updated_mouse_info.object_space_pos.xy();
  _mouse_is_down = true;

  // Trackball.
  if (updated_mouse_info.middle_button)
  {
//     // Set the trackball's pivot and start tracking.
//    _track_ball.set_pivot(updated_mouse_info.object_space_pos);
//    _track_ball.start_tracking(updated_mouse_info.screen_pos_gl, updated_mouse_info.camera_space_pos, updated_mouse_info.object_space_pos, _viewport, _lens.get_projection());
//    _mouse_over_info = updated_mouse_info;
//
//    // Remain in whatever state we're in.
//    // Trackball use is allowed in all states.

  } else if (updated_mouse_info.left_button) {

    // Set the trackball's pivot and start tracking.
    // We now start mouse tracking on any left mouse click.
    // Left mouse dragging is now used to pan the entire node graph as well individual nodes.
   _view_controls.track_ball.set_pivot(updated_mouse_info.object_space_pos);
   _view_controls.start_tracking(updated_mouse_info);
   _mouse_over_info = updated_mouse_info;

    switch (region) {
      case HitRegion::kInputRegion: {
        if ((_state != kDraggingLinkHead) && (_state != kTwoClickOutputToInput) && (_state != kDraggingLinkTail) && (_state != kTwoClickInputToOutput)) {
          // Get the input compute.
          Dep<InputCompute> input_compute = get_dep<InputCompute>(comp_shape_entity);
          Dep<InputShape> input_shape = get_dep<InputShape>(comp_shape_entity);

          // Unlink the input compute from its output compute.
          input_compute->unlink_output_compute();

          // Unlink the link shapes from its input compute.
          Entity* link_entity = input_shape->find_link_entity();
          if (link_entity) {
            _link_shape = get_dep<LinkShape>(link_entity);
            _link_shape->start_moving();
            // Clear the output shape but leave the input shape linked.
            _link_shape->unlink_output_shape();
          } else {
            _link_shape = create_link();
            // Link the output shape.
            _link_shape->link_input_shape(input_shape);
          }

          // Update all deps.
          //get_app_root()->initialize_wires();

          // Set the initial tail pos.
          const glm::vec2& plug_center = input_shape->get_origin();
          glm::vec2 tail_pos = plug_center + glm::vec2(0, 1);
          _link_shape->set_tail_pos(tail_pos);

          // Transition to tail dragging mode.
          _state = kDraggingLinkTail;

          // Clear any existing selections.
          _selection->clear_selection();
          _mouse_down_node_positions.clear();
        }
        break;
      }
      case HitRegion::kOutputRegion: {
        if ((_state != kDraggingLinkTail) && (_state != kTwoClickInputToOutput) && (_state != kDraggingLinkHead) && (_state != kTwoClickOutputToInput)) {
          // Get the output compute.
          Dep<OutputShape> output_shape = get_dep<OutputShape>(comp_shape_entity);

          // We always create a new link when clicking on output plugs, because output plugs can have multiple links (unlike input links).
          _link_shape = create_link();
          _link_shape->link_output_shape(output_shape);

          // Set the intial head pos.
          const glm::vec2 &plug_center = output_shape->get_origin();
          glm::vec2 head_pos = plug_center + glm::vec2(0, -1);
          _link_shape->set_head_pos(head_pos);

          // Transition to head dragging mode.
          _state = kDraggingLinkHead;

          // Clear any existing selections.
          _selection->clear_selection();
          _mouse_down_node_positions.clear();
        }
        break;
      }
      case HitRegion::kNodeRegion: {
        // Otherwise if we have any node that is selected.
        if (_state == kNodeSelectionAndDragging) {
          Dep<NodeShape> node_shape = get_dep<NodeShape>(comp_shape_entity);
          // If the node is not already selected, then make it the sole selection.
          if (!_selection->is_selected(node_shape)) {
            // Clear out the current selection.
            _selection->clear_selection();
            _mouse_down_node_positions.clear();

            // Add this to the selection.
            _selection->select(node_shape);
            _mouse_down_node_positions[node_shape] = node_shape->get_pos();
          }

          // Record all selected node positions.
          const DepUSet<NodeShape>& selected = _selection->get_selected();
          for (const Dep<NodeShape>& d : selected) {
            _mouse_down_node_positions[d] = d->get_pos();
          }

          // We get here if we might be starting to pan some nodes.
          _panning_selection = true;
          // Set the trackball's pivot and start tracking.
          _view_controls.track_ball.set_pivot(updated_mouse_info.object_space_pos);
          _view_controls.start_tracking(updated_mouse_info);
          _mouse_over_info = updated_mouse_info;

          return node_shape;
        }
        break;
      }
      case HitRegion::kLinkHeadRegion: {
        if (_state == kNodeSelectionAndDragging) {
          _link_shape = get_dep<LinkShape>(comp_shape_entity);
          Dep<InputCompute> input_compute = get_dep<InputCompute>(_link_shape->get_input_shape()->our_entity());

          // Break the link.
          input_compute->unlink_output_compute();

          // Put the link in interactive mode.
          _link_shape->start_moving();
          _link_shape->unlink_input_shape();

          // Transition.
          _state = kDraggingLinkHead;

          // Clear any existing selections.
          _selection->clear_selection();
          _mouse_down_node_positions.clear();
        }
        break;
      }
      case HitRegion::kLinkTailRegion: {
        if (_state == kNodeSelectionAndDragging) {
          _link_shape = get_dep<LinkShape>(comp_shape_entity);
          Dep<InputCompute> input_compute = get_dep<InputCompute>(_link_shape->get_input_shape()->our_entity());

          // Break the link.
          input_compute->unlink_output_compute();

          // Put the link in interactive mode.
          _link_shape->start_moving();
          _link_shape->unlink_output_shape();

          // Transition.
          _state = kDraggingLinkTail;

          // Clear any existing selections.
          _selection->clear_selection();
          _mouse_down_node_positions.clear();
        }
        break;
      }
      case HitRegion::kMissedRegion: {
        // Clear our selection.
        //_ng_state->clear_selection();
        //_mouse_down_node_positions.clear();

        // Reset our finite state machine
        if (_link_shape) {
          Entity* e = _link_shape->our_entity();
          delete_ff(e);
        }

        // Reset our interactive dragging state.
        _link_shape.reset();

        _state = kNodeSelectionAndDragging;
        break;
      }
    }
  } else if (updated_mouse_info.right_button) {

    if (region == HitRegion::kNodeRegion) {
      Dep<NodeShape> shape = get_dep<NodeShape>(comp_shape_entity);
      return shape;
    }
//    Entity* comp_shape_entity;
//    if (comp_shape) {
//      comp_shape_entity = comp_shape->our_entity();
//      if (comp_shape_entity->get_did() == kGroupNodeEntity) {
//        //dive(comp_shape_entity);
//      }
//    } else {
//      // If nothing was hit, then we surface up to our parent group.
//      //surface();
//    }
  }

  return Dep<NodeShape>(NULL);
}

void GroupInteraction::released(const MouseInfo& mouse_info) {
  external();
  _mouse_is_down = false;

  MouseInfo updated_mouse_info = mouse_info;
  _view_controls.update_coord_spaces(updated_mouse_info);

  // Trackball.
  if (updated_mouse_info.middle_button)
  {
//    // Stop the trackball tracking.
//    _track_ball.stop_tracking();

  } else if (updated_mouse_info.left_button) {

    // Panning a selection of nodes.
    if ((_state == kNodeSelectionAndDragging) && (_panning_selection)) {

      // Get our selections movement delta.
      glm::vec2 delta = get_drag_delta();

      // Adjust all the selcted nodes with this delta.
      const DepUSet<NodeShape>& selected = _selection->get_selected();
      for(const Dep<NodeShape>& cs: selected) {
        cs->set_pos(_mouse_down_node_positions[cs]+delta);
      }
      // Stop the track ball tracking mode.
      _view_controls.track_ball.stop_tracking();
      _panning_selection=false;
      // Reset the model view to before we starting panning our selection.
      _view_controls.track_ball.revert_to_last_model_view();
      return;
    }

    HitRegion region = HitRegion::kMissedRegion;
    Dep<CompShape> comp_shape(this);
    {
      Entity* e = _shape_collective->hit_test(updated_mouse_info.object_space_pos.xy(), region);
      if (e) {
        comp_shape = get_dep<CompShape>(e);
      }
    }

    Entity* comp_shape_entity = NULL;

    if (comp_shape) {
      comp_shape_entity = comp_shape->our_entity();
    }

    switch (region) {
      case HitRegion::kInputRegion: {
        if ((_state == kDraggingLinkHead) || (_state == kTwoClickOutputToInput)) {
          Dep<InputCompute> input_compute = get_dep<InputCompute>(comp_shape_entity);
          Dep<InputShape> input_shape = get_dep<InputShape>(comp_shape_entity);
          Dep<OutputCompute> output_compute = get_dep<OutputCompute>(_link_shape->get_output_shape()->our_entity());

          // Remove any existing link shapes on this input.
          // There is only one link per input.
          Entity* old_link_entity = input_shape->find_link_entity();
          if (old_link_entity) {
            // Remove any existing compute connection on this input compute.
            input_compute->unlink_output_compute();
            // Destroy the link.
            delete_ff(old_link_entity);
          }

          // Try to connect the input and output computes.
          if (!input_compute->link_output_compute(output_compute)) {
            // Otherwise destroy the link and selection.
            reset_state();
          }

          // Update the link shape's input shape.
          _link_shape->link_input_shape(input_shape);
          _link_shape->finished_moving();

          // Transition to default state.
          _state = kNodeSelectionAndDragging;

          // Reset our interactive dragging state.
          _link_shape.reset();

        } else if (_state == kDraggingLinkTail) {

          // Here we've pressed and released the mouse on the same input plug.
          // Transition to the two click input to output state.
          _state = kTwoClickInputToOutput;

        }
        break;
      }
      case HitRegion::kOutputRegion: {
        if ((_state == kDraggingLinkTail) || (_state == kTwoClickInputToOutput)) {
          Dep<OutputCompute> output_compute = get_dep<OutputCompute>(comp_shape_entity);
          Dep<OutputShape> output_shape = get_dep<OutputShape>(comp_shape_entity);
          Dep<InputCompute> input_compute = get_dep<InputCompute>(_link_shape->get_input_shape()->our_entity());

          // Try to connect the input and output computes.
          if (!input_compute->link_output_compute(output_compute)) {
            // Otherwise destroy the link and selection.
            reset_state();
          }

          // Update the link shape's output shape.
          _link_shape->link_output_shape(output_shape);
          _link_shape->finished_moving();

          // Transition to the default state.
          _state = kNodeSelectionAndDragging;

          // Reset our interactive dragging state.
          _link_shape.reset();

        } else if (_state == kDraggingLinkHead) {
          // Here we've pressed and released the mouse on the same output plug.
          // Transition to the two click output to input state.
          _state = kTwoClickOutputToInput;
        }
        break;
      }
      case HitRegion::kNodeRegion: {  // This is supposed to find nodes.
        if (_state == kDraggingLinkTail) {
          _state = kTwoClickInputToOutput;
        } else if (_state == kDraggingLinkHead) {
          _state = kTwoClickOutputToInput;
        }
        break;
      }
      case HitRegion::kLinkHeadRegion: {
        if (_state == kDraggingLinkHead) {
          _state = kTwoClickOutputToInput;
        } else if (_state == kTwoClickOutputToInput) {
          reset_state();
        }
        break;
      }
      case HitRegion::kLinkTailRegion: {
        if (_state == kDraggingLinkTail) {
          _state = kTwoClickInputToOutput;
        } else if (_state == kTwoClickInputToOutput) {
          reset_state();
        }
        break;
      }
      case HitRegion::kMissedRegion: {
        if ((_state == kDraggingLinkHead) || (_state == kTwoClickOutputToInput) || (_state == kDraggingLinkTail) || (_state == kTwoClickInputToOutput)) {
          reset_state();
        }
        // Stop the tracking.
        _view_controls.track_ball.stop_tracking();
        break;
      }
    }
  }
  //our_entity()->clean_dead_entities();
  //update_shape_collective();
}

void GroupInteraction::moved(const MouseInfo& mouse_info) {
  external();
  _mouse_over_info = mouse_info;
  _view_controls.update_coord_spaces(_mouse_over_info);

  // Trackball.
  if (_mouse_over_info.middle_button) {
    // Determine the type of track ball action to take.
    TrackBall::Mode action = TrackBall::TRANSLATE;
    if (_mouse_over_info.middle_button) {
      if (_mouse_over_info.control_modifier) {
        action = TrackBall::ZOOM;
      } else if (_mouse_over_info.shift_modifier) {
        action = TrackBall::DOLLY;
      } else {
        action = TrackBall::TRANSLATE;
      }
    } else if (_mouse_over_info.left_button) {
      action = TrackBall::ROTATE;
    } else if (_mouse_over_info.right_button) {

    }
    // Adjust the trackball accordingly.
    _view_controls.track_ball.track(_mouse_over_info.screen_pos_gl, action);
  } else if (_mouse_over_info.left_button) {
    if (_state == kNodeSelectionAndDragging) {
      _view_controls.track_ball.track(_mouse_over_info.screen_pos_gl, TrackBall::TRANSLATE);
    }
    else if ((_state == kDraggingLinkHead) || (_state == kTwoClickOutputToInput)) {
      _link_shape->set_head_pos(_mouse_over_info.object_space_pos.xy());
    } else if ((_state == kDraggingLinkTail) || (_state == kTwoClickInputToOutput)) {
      _link_shape->set_tail_pos(_mouse_over_info.object_space_pos.xy());
    }
  } else if (!(_mouse_over_info.left_button || _mouse_over_info.middle_button || _mouse_over_info.right_button)) {
    // No button mouse moves.
    if (_state == kTwoClickOutputToInput) {
      _link_shape->set_head_pos(_mouse_over_info.object_space_pos.xy());
    } else if (_state == kTwoClickInputToOutput) {
      _link_shape->set_tail_pos(_mouse_over_info.object_space_pos.xy());
    }
  }
}

void GroupInteraction::wheel_rolled(const WheelInfo& info) {
  external();
  //_mouse_over_info
  TrackBall::Mode action = TrackBall::ZOOM;
  // Adjust the trackball accordingly.
  _view_controls.track_ball.wheel_zoom(_mouse_over_info.camera_space_pos, 30*info.y);
}

void GroupInteraction::pinch_zoom(const glm::vec2& origin, float factor) {
  external();
  MouseInfo info = get_vec2_info(origin);
  _view_controls.update_coord_spaces(info);

  _mouse_over_info = info;

  _view_controls.track_ball.pinch_zoom(info.camera_space_pos, factor);
}

void GroupInteraction::finalize_pinch_zoom() {
  external();
  _view_controls.track_ball.finalize_pinch_zoom();
}

const glm::mat4 GroupInteraction::get_mouse_model_view() const {
  external();
  glm::mat4 m(1);
  m[3][0] = _mouse_over_info.screen_pos_gl.x;
  m[3][1] = _mouse_over_info.screen_pos_gl.y;
  m[3][2] = -500.0f;
  return m;
}

bool GroupInteraction::is_panning_selection() const {
  external();
  return _panning_selection;
}

void GroupInteraction::edit(const Dep<NodeShape>& comp_shape) {
  external();
  _selection->set_edit_node(comp_shape);
}

void GroupInteraction::view(const Dep<NodeShape>& comp_shape) {
  external();
  _selection->set_view_node(comp_shape);
}

void GroupInteraction::process(const Dep<NodeShape>& comp_shape) {
  external();
  _selection->set_processing_node(comp_shape);
}

void GroupInteraction::select(const Dep<NodeShape>& comp_shape) {
  external();
  _selection->select(comp_shape);
}

void GroupInteraction::deselect(const Dep<NodeShape>& comp_shape) {
  external();
  _selection->deselect(comp_shape);
}

void GroupInteraction::select_all() {
  external();
  Entity* group_node = get_entity(Path({"."}));
  DepUSet<NodeShape> set;
  for (const auto &iter: group_node->get_children()) {
    Entity* child = iter.second;
    Dep<CompShape> comp_shape = get_dep<CompShape>(child);
    if (comp_shape && comp_shape->is_linkable()) {
      Dep<NodeShape> ls = get_dep<NodeShape>(child);
      set.insert(ls);
    }
  }
  _selection->select(set);
}

void GroupInteraction::deselect_all() {
  external();
  _selection->clear_all();
}

void GroupInteraction::frame_all() {
  external();
  // Make sure the shapes have been tessellated.
  _shape_collective->clean_state();
  // Get the bounds.
  glm::vec2 min;
  glm::vec2 max;
  _shape_collective->get_aa_bounds(min,max);
  // Frame the bounds..
  _view_controls.frame(min, max);
}

void GroupInteraction::frame_selected(const DepUSet<NodeShape>& selected) {
  external();
  if (selected.empty()) {
    return;
  }
  // Make sure the shapes have been tessellated.
  _shape_collective->clean_state();
  // Get the bounds.
  glm::vec2 min;
  glm::vec2 max;
  _shape_collective->get_aa_bounds(selected,min,max);
  // Frame the bounds.
  _view_controls.frame(min, max);
}

void GroupInteraction::centralize(const Dep<NodeShape>& node) {
  external();
  glm::vec2 center = _view_controls.get_center_in_object_space();
  node->set_pos(center);
}

glm::vec2 GroupInteraction::get_center_in_object_space() const {
  external();
  return _view_controls.get_center_in_object_space();
}

void GroupInteraction::collapse_selected() {
  external();
  const DepUSet<NodeShape>& selected = _selection->get_selected();
  collapse(selected);
}

void GroupInteraction::explode_selected() {
  external();
  const DepUSet<NodeShape>& selected = _selection->get_selected();
  if (selected.empty()) {
    return;
  }
  explode((*selected.begin())->our_entity());
}

void GroupInteraction::collapse(const DepUSet<NodeShape>& selected) {
  external();
  if (selected.empty()) {
    return;
  }

  // The parent of all the selected entities.
  Entity* parent = (*selected.begin())->get_entity(Path({".."}));
  assert(parent == our_entity());

  // Determine where to place the collapsed group node.
  glm::vec2 collapse_center;
  {
    glm::vec2 min;
    glm::vec2 max;
    _shape_collective->get_aa_bounds(selected, min, max);
    collapse_center = 0.5f * (min + max);
  }

  // Copy the selected nodes.
  std::unordered_set<Entity*> selected_entities;
  for (const auto &s: selected) {
    selected_entities.insert(s->our_entity());
  }
  std::string raw_copy = our_entity()->copy_to_string(selected_entities);

  // Create a group node to hold the collapsed nodes.
  Entity* collapsed_node = _factory->instance_entity(our_entity(), EntityDID::kGroupNodeEntity, "collapsed");
  collapsed_node->create_internals();
  collapsed_node->initialize_wires();

  // Position it at the center of the collapsed nodes.
  Dep<NodeShape> collapsed_node_cs = get_dep<NodeShape>(collapsed_node);
  collapsed_node_cs->set_pos(collapse_center);

  // Paste the nodes into the group node.
  collapsed_node->paste_from_string(raw_copy);

  // Center the pasted nodes, by subtracting the collapse_center from each.
  // This will center the nodes at (0,0).
  {
    for (auto &iter: collapsed_node->get_children()) {
      Dep<CompShape> cs = get_dep<CompShape>(iter.second);
      if (cs && cs->is_linkable()) {
        glm::vec2 pos = cs->get_pos();
        pos -= collapse_center;
        cs->set_pos(pos);
      }
    }
  }

  // Destroy the selected nodes.
  for (Entity* e: selected_entities) {
    delete_ff(e);
  }

  // Clean out dead links and update the shape collective.
  clean_dead_links();
  update_shape_collective();

  // Select the collapsed node.
  _selection->clear_selection();
  _selection->select(collapsed_node_cs);
}

void GroupInteraction::explode(Entity* e) {
  external();
  Dep<NodeShape> cs = get_dep<NodeShape>(e);

  // Determine the exploding center.
  glm::vec2 min, max;
  cs->get_border().get_aa_bounds(min, max);
  glm::vec2 exploding_center = 0.5f * (min + max);

  Entity* cs_entity = cs->our_entity();

  // Select all the nodes in the group, this includes the input and output nodes.
  std::unordered_set<Entity*> nodes;
  DepUSet<NodeShape> shapes;
  for (auto &iter: cs_entity->get_children()) {
    Dep<CompShape> cs = get_dep<CompShape>(iter.second);
    if (cs && cs->is_linkable()) {
      nodes.insert(iter.second);
      shapes.insert(get_dep<NodeShape>(iter.second));
    }
  }

  // Determine the center of the nodes in the exploding group.
  CompShapeCollective::get_aa_bounds(shapes, min, max);
  glm::vec2 nodes_center = 0.5f * (min + max);

  // Copy the nodes.
  std::string raw_copy = cs_entity->copy_to_string(nodes);

  // Paste these nodes outside the exploding group.
  Entity* parent = cs->get_entity(Path({".."}));
  parent->paste_from_string(raw_copy);

  // Get the pasted nodes.
  const std::unordered_set<Entity*>& pasted = parent->get_last_pasted();

  // Destroy the exploding group.
  delete_ff(cs_entity);
  //get_app_root()->initialize_wires();

  // Center the pasted nodes around the exploding center.
  glm::vec2 offset = exploding_center - nodes_center;
  for (Entity* p : pasted) {
    Dep<CompShape> cs = get_dep<CompShape>(p);
    if (cs) {
      glm::vec2 pos = cs->get_pos();
      pos += offset;
      cs->set_pos(pos);
    }
  }

  // Select the exploded contents.
  _selection->clear_selection();
  for (Entity* p : pasted) {
    Dep<NodeShape> cs = get_dep<NodeShape>(p);
    _selection->select(cs);
  }

  // Clean out dead links and update the shape collective.
  clean_dead_links();
  update_shape_collective();
}

//void GroupInteraction::explode(const Dep<NodeShape>& cs) {
//  external();
//  // Determine the exploding center.
//  glm::vec2 min, max;
//  cs->get_border().get_aa_bounds(min, max);
//  glm::vec2 exploding_center = 0.5f * (min + max);
//
//  Entity* cs_entity = cs->our_entity();
//
//  // Select all the nodes in the group, this includes the input and output nodes.
//  std::unordered_set<Entity*> nodes;
//  DepUSet<NodeShape> shapes;
//  for (auto &iter: cs_entity->get_children()) {
//    Dep<CompShape> cs = get_dep<CompShape>(iter.second);
//    if (cs && cs->is_linkable()) {
//      nodes.insert(iter.second);
//      shapes.insert(get_dep<NodeShape>(iter.second));
//    }
//  }
//
//  // Determine the center of the nodes in the exploding group.
//  CompShapeCollective::get_aa_bounds(shapes, min, max);
//  glm::vec2 nodes_center = 0.5f * (min + max);
//
//  // Copy the nodes.
//  std::string raw_copy = cs_entity->copy_to_string(nodes);
//
//  // Paste these nodes outside the exploding group.
//  Entity* parent = cs->get_entity(Path({".."}));
//  parent->paste_from_string(raw_copy);
//
//  // Get the pasted nodes.
//  const std::unordered_set<Entity*>& pasted = parent->get_last_pasted();
//
//  // Destroy the exploding group.
//  delete_ff(cs_entity);
//  //get_app_root()->initialize_wires();
//
//  // Center the pasted nodes around the exploding center.
//  glm::vec2 offset = exploding_center - nodes_center;
//  for (Entity* p : pasted) {
//    Dep<CompShape> cs = get_dep<CompShape>(p);
//    if (cs) {
//      glm::vec2 pos = cs->get_pos();
//      pos += offset;
//      cs->set_pos(pos);
//    }
//  }
//
//  // Select the exploded contents.
//  _selection->clear_selection();
//  for (Entity* p : pasted) {
//    Dep<NodeShape> cs = get_dep<NodeShape>(p);
//    _selection->select(cs);
//  }
//
//  // Clean out dead links and update the shape collective.
//  clean_dead_links();
//  update_shape_collective();
//}

void GroupInteraction::resize_gl(GLsizei width, GLsizei height) {
  external();
  _view_controls.resize(width, height);
}

const ViewportParams& GroupInteraction::get_viewport_params() const {
  external();
  return _view_controls.viewport;
}

const glm::mat4& GroupInteraction::get_model_view() const {
  external();
  return _view_controls.get_model_view();
}
const glm::mat4& GroupInteraction::get_last_model_view() const{
  external();
  return _view_controls.get_last_model_view();
}
const glm::mat4& GroupInteraction::get_projection() const{
  external();
  return _view_controls.get_projection();
}
glm::mat4 GroupInteraction::get_model_view_project() const{
  external();
  return _view_controls.get_model_view_project();
}
glm::mat4 GroupInteraction::get_last_model_view_project() const{
  external();
  return _view_controls.get_last_model_view_project();
}

void GroupInteraction::link(Entity* upstream, Entity* downstream) {
  Entity* link = _manipulator->connect_plugs(downstream, upstream);
  _link_shape = get_dep<LinkShape>(link);

  // Transition to default state.
  _state = kNodeSelectionAndDragging;

  // Reset our interactive dragging state.
  _link_shape.reset();
}

}
