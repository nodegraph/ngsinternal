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
#include <entities/entityids.h>
#include <entities/componentids.h>
#include <components/interactions/viewcontrols.h>
#include <freetype-gl/texture-atlas.h>
#include <guicomponents/comms/browserworker.h>

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
  get_dep_loader()->register_fixed_dep(_factory, Path({}));
  get_dep_loader()->register_fixed_dep(_resources, Path({}));
  get_dep_loader()->register_fixed_dep(_ng_state, Path({}));
}

ShapeCanvas::~ShapeCanvas() {
  delete_ff(_text_pipeline);
  delete_ff(_quad_pipeline);
}

void ShapeCanvas::update_wires() {
  internal();
  if (_factory->no_current_group()) {
    push_group(get_entity(Path({"root"})));
  } else if (!_current_interaction) {
    _factory->clear_group_stack();
    push_group(get_entity(Path({"root"})));
  }
}

bool ShapeCanvas::update_state() {
  internal();
  // Note the open gl context must be bound/active for this update.
  if (!_quad_pipeline) {
    return true;
  }

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
  return true;
}

void ShapeCanvas::initialize_gl() {
  internal();
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
  internal();
  delete_ff(_quad_pipeline);
  delete_ff(_text_pipeline);
}

bool ShapeCanvas::is_initialized_gl_imp() const {
  internal();
  if (_quad_pipeline) {
    return true;
  }
  return false;
}

void ShapeCanvas::draw_gl() {
  external();

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

void ShapeCanvas::surface() {
  external();
  pop_group();

  // The group we're surfacing to needs to have its inputs and output updated.
  get_app_root()->clean_wires();
}

void ShapeCanvas::surface_to_root() {
  external();
  while(_factory->get_group_stack_depth()>1) {
    pop_group();
  }
}

void ShapeCanvas::dive(Entity* selected) {
  external();
  // If the selected is a group dive into it.
  if (selected->has<GroupInteraction>()) {
    push_group(selected);
  }
}

void ShapeCanvas::dive() {
  external();
  const DepUSet<NodeShape>& selected = _ng_state->get_selected();
  // Dive into the first selected group.
  for (const Dep<NodeShape> &cs: selected) {
    if (cs->get_did() == ComponentDID::kGroupNodeShape) {
      dive(cs->our_entity());
    }
  }
}

const Dep<GroupInteraction>& ShapeCanvas::get_current_interaction() const {
  external();
  return _current_interaction;
}

const Dep<CompShapeCollective>& ShapeCanvas::get_current_shape_collective() const {
  external();
  return _current_shape_collective;
}

Dep<BaseFactory> ShapeCanvas::get_factory() const {
  external();
  return _factory;
}

// -----------------------------------------------------------------------------------
// Private:
// -----------------------------------------------------------------------------------

void ShapeCanvas::push_group(Entity* group) {
  internal();
  _factory->push_group(group);

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
  internal();

  // The last group should not be popped.
  if (_factory->get_group_stack_depth()<=1) {
    return;
  }

  // Otherwise pop it.
  _factory->pop_group();

  // Cache some values.
  Dep<GroupInteraction> prev_interaction = _current_interaction;
  Dep<CompShapeCollective> prev_shape_collective = _current_shape_collective;
  _current_interaction = get_dep<GroupInteraction>(_factory->get_current_group());
  _current_shape_collective = get_dep<CompShapeCollective>(_factory->get_current_group());

  // Copy over some settings to the new interaction.
  if (prev_interaction) {
    // Resize the window.
    const ViewportParams& params = prev_interaction->get_viewport_params();
    _current_interaction->resize_gl(params.width, params.height);
    // Set the mouse over info.
    _current_interaction->set_mouse_over_info(prev_interaction->get_mouse_over_info());
  }
}

}
