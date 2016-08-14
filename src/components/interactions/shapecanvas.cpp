#include <components/interactions/shapecanvas.h>
#include <base/memoryallocator/taggednew.h>

#include <components/interactions/groupinteraction.h>

#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>

#include <base/device/deviceheadersgl.h>
#include <base/device/geometry/quad.h>
#include <base/device/program/pipeline.h>
#include <base/device/devicedebug.h>
#include <base/device/pipelinesetups/textpipelinesetup.h>
#include <base/device/pipelinesetups/quadpipelinesetup.h>
#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/objectmodel/entity.h>
#include <base/utils/archdebug.h>

#include <components/resources/resources.h>
#include <components/compshapes/compshapecollective.h>

#include <components/compshapes/inputnodeshape.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/outputnodeshape.h>
#include <components/entities/entityids.h>
#include <components/entities/componentids.h>
#include <components/interactions/viewcontrols.h>
#include <freetype-gl/texture-atlas.h>

#include <iostream>
#include <QtCore/qDebug>

namespace ngs {

ShapeCanvas::ShapeCanvas(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _factory(this),
      _resources(this),
      _ng_state(this),
      _current_interaction(this),
      _current_shape_collective(this),
      _quad_pipeline(NULL),
      _text_pipeline(NULL),
      _clear_depth(true),
      _clear_color(true) {
  get_dep_loader()->register_fixed_dep(_factory, "");
  get_dep_loader()->register_fixed_dep(_resources, "");
  get_dep_loader()->register_fixed_dep(_ng_state, "");
}

ShapeCanvas::~ShapeCanvas() {
  assert(!_quad_pipeline);
  assert(!_text_pipeline);
}

void ShapeCanvas::initialize_fixed_deps() {
  Component::initialize_fixed_deps();
  if (_group_stack.empty()) {
	  push_group(get_entity("root"));
  } else if (!_current_interaction) {
    _group_stack.clear();
    push_group(get_entity("root"));
  }
}

void ShapeCanvas::update_state() {
  // Note the open gl context must be bound/active for this update.

  // If the current group interaction is dirty, then load data from it.
  if (_current_interaction && dep_is_dirty(_current_interaction)) {

    //std::cerr << "updating shape canvas from interation\n";
    // Set the viewport.
    const ViewportParams& viewport = _current_interaction->get_viewport_params();

    glViewport(viewport.x,viewport.y,viewport.width,viewport.height);
    //qDebug() << "shape canvas viewport: " << viewport.x << "," << viewport.y << "," << viewport.width << "," << viewport.height << "\n";

    // Update our quad pipeline.
    _quad_pipeline->use();

    // Set uniforms.
    if (_current_interaction->is_panning_selection()) {
      _quad_pipeline->set_mvp(_current_interaction->get_last_model_view_project());
      _quad_pipeline->set_selected_mvp(_current_interaction->get_model_view_project());
    } else {
      _quad_pipeline->set_mvp(_current_interaction->get_model_view_project());
      _quad_pipeline->set_selected_mvp(_current_interaction->get_model_view_project());
    }

    // Update our text pipeline.
    _text_pipeline->use();
    if (_current_interaction->is_panning_selection()) {
      _text_pipeline->set_mvp(_current_interaction->get_last_model_view_project());
      _text_pipeline->set_selected_mvp(_current_interaction->get_model_view_project());
    } else {
      _text_pipeline->set_mvp(_current_interaction->get_model_view_project());
      _text_pipeline->set_selected_mvp(_current_interaction->get_model_view_project());
    }

  }

  // If the current comp shape collective is dirty, then load data from it.
  if (_current_shape_collective && dep_is_dirty(_current_shape_collective)) {
    // Load our shape instances.
    _quad_pipeline->load_quad_instances(_current_shape_collective->get_quads());
    _quad_pipeline->load_tri_instances(_current_shape_collective->get_tris());
    _quad_pipeline->load_circle_instances(_current_shape_collective->get_circles());
    // Load our text vertices.
    _text_pipeline->load_char_instances(_current_shape_collective->get_chars());
  }
}

void ShapeCanvas::initialize_gl() {
  start_method();
  // Create our drawing pipeline.
  if (!_quad_pipeline) {
    _quad_pipeline = new_ff QuadPipelineSetup(_resources->get_quad_vertex_shader(), _resources->get_quad_fragment_shader());
    _quad_pipeline->initialize_gl();
  }
  if (!_text_pipeline) {
    _text_pipeline = new_ff TextPipelineSetup(_resources->get_text_vertex_shader(), _resources->get_text_fragment_shader(),
                                              _resources->get_text_font(), _resources->get_distance_map(), _resources->get_distance_map_size());
    _text_pipeline->initialize_gl();
  }
}

void ShapeCanvas::uninitialize_gl() {
  delete_ff(_quad_pipeline);
  delete_ff(_text_pipeline);
}

bool ShapeCanvas::is_initialized_gl() {
  if (_quad_pipeline) {
    return true;
  }
  return false;
}

void ShapeCanvas::draw_gl() {
  start_method();

  // To deal macos artifacts when there is nothing to draw.
  if (!_quad_pipeline->has_instances()) {
    _quad_pipeline->use();
    _quad_pipeline->draw_garbage();
  }

  // Clear the depth.
  if (_clear_depth) {
    _quad_pipeline->clear_depth();
  }

  // Clear the color.
  if (_clear_color) {
    _quad_pipeline->clear_color();
  }

  // Draw our quad pipeline.
  _quad_pipeline->use();
  _quad_pipeline->draw();

  // Draw our text pipeline.
  _text_pipeline->use();
  _text_pipeline->draw();

  _quad_pipeline->disable();
  _text_pipeline->disable();
}

const Dep<GroupInteraction>& ShapeCanvas::get_current_interaction() const {
  start_method();
  return _current_interaction;
}

const Dep<CompShapeCollective>& ShapeCanvas::get_current_shape_collective() const {
  start_method();
  return _current_shape_collective;
}

void ShapeCanvas::push_group(Entity* group) {
  start_method();
  _group_stack.push_back(group);

  // Cache some values.
  Dep<GroupInteraction> prev_interaction = _current_interaction;
  Dep<CompShapeCollective> prev_shape_collective = _current_shape_collective;
  _current_interaction = get_dep<GroupInteraction>(group);
  _current_shape_collective = get_dep<CompShapeCollective>(group);

  // Copy over some settings to the new interaction.
  if (prev_interaction) {
    // Resize the window.
    const ViewportParams& params = prev_interaction->get_viewport_params();
    _current_interaction->resize_gl(params.width, params.height);
    // Set the mouse over info.
    _current_interaction->set_mouse_over_info(prev_interaction->get_mouse_over_info());
  }
}

void ShapeCanvas::pop_group() {
  start_method();

  // The last group should not be popped.
  if (_group_stack.size()<=1) {
    return;
  }

  // Otherwise pop it.
  _group_stack.pop_back();

  // Cache some values.
  Dep<GroupInteraction> prev_interaction = _current_interaction;
  Dep<CompShapeCollective> prev_shape_collective = _current_shape_collective;
  _current_interaction = get_dep<GroupInteraction>(get_current_group());
  _current_shape_collective = get_dep<CompShapeCollective>(get_current_group());

  // Copy over some settings to the new interaction.
  if (prev_interaction) {
    // Resize the window.
    const ViewportParams& params = prev_interaction->get_viewport_params();
    _current_interaction->resize_gl(params.width, params.height);
    // Set the mouse over info.
    _current_interaction->set_mouse_over_info(prev_interaction->get_mouse_over_info());
  }
}

void ShapeCanvas::surface_to_root() {
  while(_group_stack.size()>1) {
    pop_group();
  }
}

Entity* ShapeCanvas::get_current_group() const {
  return _group_stack.back();
}

void ShapeCanvas::surface() {
  start_method();
  pop_group();

  // The group we're surfacing to needs to have its inputs and output updated.
  get_app_root()->update_deps_and_hierarchy();
}

void ShapeCanvas::dive(Entity* selected) {
  start_method();
  // If the selected is a group dive into it.
  if (selected->has<GroupInteraction>()) {
    push_group(selected);
  }
}

void ShapeCanvas::dive() {
  start_method();
  const DepUSet<NodeShape>& selected = _ng_state->get_selected();
  // Dive into the first selected group.
  for (const Dep<NodeShape> &cs: selected) {
    if (cs->get_did() == kGroupNodeShape) {
      dive(cs->our_entity());
    }
  }
}

}
