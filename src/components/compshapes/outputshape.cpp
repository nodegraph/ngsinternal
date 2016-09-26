#include <components/computes/outputcompute.h>
#include <components/resources/resources.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/device/geometry/textlimits.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <cassert>
#include <boost/math/constants/constants.hpp>
#include <components/compshapes/nodeshape.h>
#include <components/compshapes/outputshape.h>

namespace ngs {

const float OutputShape::bg_depth = 1.0f;
const float OutputShape::fg_depth = 2.0f;
const std::array<unsigned char,4> OutputShape::bg_color = { 64, 64, 64, 255 };
const std::array<unsigned char,4> OutputShape::fg_color = { 100, 100, 100, 255 };

const glm::vec2 OutputShape::plug_size(120.0f, 120.0f *sqrt(3.0f)/2.0f); // The plug is an upside down isosceles triangle.
const float OutputShape::plug_border_width = 10.0f;
const float OutputShape::plug_radius = 50.0f;
const float OutputShape::plug_offset = 10.0f;


OutputShape::OutputShape(Entity* entity)
    : CompShape(entity, kDID()),
      _node_shape(this),
      _output_compute(this){
  get_dep_loader()->register_fixed_dep(_node_shape, "../..");
  get_dep_loader()->register_fixed_dep(_output_compute, ".");


}

OutputShape::~OutputShape() {
}

bool OutputShape::is_exposed() const {
  size_t index = _output_compute->get_exposed_output_index();
  if (index == -1) {
    return false;
  }
  return true;
}

void OutputShape::update_state() {
  std::cerr << "OutputShape update state\n";

  // If we're not exposed then clear out our shapes.
  if (!is_exposed()) {
    _tris.clear();
    return;
  }

  // Make sure our shapes are allocated.
  _tris.resize(2);
  ShapeInstance* bg = &_tris[0];
  ShapeInstance* fg = &_tris[1];

  size_t exposed_index = _output_compute->get_exposed_output_index();

  // Get the node bounds.
  const Polygon& bounds = _node_shape->get_bounds();

  // Get the node aa bounds.
  glm::vec2 node_min, node_max;
  bounds.get_aa_bounds(node_min, node_max);

  // Calculate the positioning.
  size_t num_exposed = _output_compute->get_num_exposed_outputs();

  float min_x(node_min.x);
  float max_x(node_max.x);
  float delta = (max_x - min_x) / (num_exposed + 1);
  float start = min_x + delta;

  // Update the origin, which is the center of the triangle.
  _origin = glm::vec2(start + exposed_index * delta, node_min.y - plug_offset - plug_size.y/2.0f );

  // Update our bounds.
  std::vector<glm::vec2>& verts = _bounds.vertices;
  verts.resize(3);
  verts[0] = glm::vec2(_origin.x, _origin.y - plug_size.y/2.0f );
  verts[1] = verts[0] + glm::vec2(-plug_size.x/2.0f, plug_size.y);
  verts[2] = verts[1] + glm::vec2(plug_size.x, 0);

  // Update our pannable state.
  set_pannable(_node_shape->is_selected());

  // Update our bg triangle.
  bg->set_scale(plug_size);
  bg->set_rotate(0);
  bg->set_translate(verts[0], bg_depth);
  bg->set_color(bg_color);
  if (_node_shape->is_selected()) {
    bg->state |= selected_transform_bitmask;
  } else {
    bg->state &= ~selected_transform_bitmask;
  }

  // Update our fg triangle.
  // h' = h - 3*border_width
  // w' = w - 2*sqrt(3)*border_width
  fg->set_scale(plug_size - glm::vec2(2.0f * sqrt(3.0f) * plug_border_width, 3.0f * plug_border_width));
  fg->set_rotate(0);
  fg->set_translate(verts[0] + glm::vec2(0, 2*plug_border_width), fg_depth);
  fg->set_color(fg_color);
  if (_node_shape->is_selected()) {
    fg->state |= selected_transform_bitmask;
  } else {
    fg->state &= ~selected_transform_bitmask;
  }
}

const Polygon& OutputShape::get_bounds() const {
  start_method();
  return _bounds;
}

const std::vector<ShapeInstance>* OutputShape::get_tri_instances() const {
  start_method();
  return &_tris;
}

const glm::vec2& OutputShape::get_origin() const {
  start_method();
  return _origin;
}

}

