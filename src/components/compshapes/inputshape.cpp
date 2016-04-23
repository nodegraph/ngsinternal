#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/resources/resources.h>

#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/device/geometry/textlimits.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <boost/math/constants/constants.hpp>
#include <components/compshapes/inputshape.h>
#include <cmath>

namespace ngs {

const float InputShape::bg_depth = 1.0f;
const float InputShape::fg_depth = 2.0f;
const std::array<unsigned char,4> InputShape::bg_color = { 38, 38, 38, 255 };
const std::array<unsigned char,4> InputShape::fg_color = { 77, 77, 77, 255 };

const glm::vec2 InputShape::plug_size(70,70);
const glm::vec2 InputShape::plug_border_size(6,6);
const float InputShape::plug_radius = 50;
const float InputShape::plug_offset = 10;


InputShape::InputShape(Entity* entity)
    : CompShape(entity, kDID()),
      _node_shape(this) {
  get_dep_loader()->register_fixed_dep(_node_shape, "../..");

  _quads.resize(2);
  _bg_quad = &_quads[0];
  _fg_quad = &_quads[1];
}

InputShape::~InputShape() {
}

void InputShape::update_state() {

  size_t order = _node_shape->get_input_order(get_name());

  // Get the node bounds.
  const Polygon& bounds = _node_shape->get_bounds();

  // Get the node aa bounds.
  glm::vec2 node_min, node_max;
  bounds.get_aa_bounds(node_min, node_max);

  // Calculate the positioning.
  size_t num_plugs = get_num_inputs();
  float min_x(node_min.x);
  float max_x(node_max.x);
  float delta = (max_x - min_x) / (num_plugs + 1);
  float start = min_x + delta;

  // Update the origin, which is the center of the quad.
  _origin = glm::vec2 (start + order * delta, node_max.y + plug_offset + plug_size.y / 2.0f);

  // Update our bounds.
  std::vector<glm::vec2>& verts = _bounds.vertices;
  verts.resize(4);
  verts[0] = glm::vec2(start + order * delta - plug_size.x / 2.0f, node_max.y + plug_offset);
  verts[1] = verts[0] + glm::vec2(plug_size.x, 0);
  verts[2] = verts[1] + glm::vec2(0, plug_size.y);
  verts[3] = verts[2] + glm::vec2(-plug_size.x, 0);

  // Update our bg quad.
  _bg_quad->set_scale(plug_size);
  _bg_quad->set_translate(verts[0], bg_depth);
  _bg_quad->set_color(bg_color);
  if (_node_shape->is_selected()) {
    _bg_quad->state |= selected_transform_bitmask;
  } else {
    _bg_quad->state &= ~selected_transform_bitmask;
  }

  // Update our fg quad.
  _fg_quad->set_scale(plug_size - 2.0f* plug_border_size);
  _fg_quad->set_translate(verts[0] + plug_border_size, fg_depth);
  _fg_quad->set_color(fg_color);
  if (_node_shape->is_selected()) {
    _fg_quad->state |= selected_transform_bitmask;
  } else {
    _fg_quad->state &= ~selected_transform_bitmask;
  }

}

size_t InputShape::get_num_inputs() const {
  start_method();
  return get_entity("..")->get_children().size();
}

const Polygon& InputShape::get_bounds() const {
  start_method();
  return _bounds;
}

bool InputShape::is_selected() const {
  if (_bg_quad->state & selected_transform_bitmask) {
    return true;
  }
  return false;
}

const std::vector<ShapeInstance>* InputShape::get_quad_instances() const {
  start_method();
  return &_quads;
}

const glm::vec2& InputShape::get_origin() const {
  start_method();
  return _origin;
}

}
