#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/resources/resources.h>

#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/device/geometry/textlimits.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <components/compshapes/topology.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/nodeshape.h>


#include <cmath>
#include <boost/math/constants/constants.hpp>

namespace ngs {

const float InputShape::bg_depth = 1.0f;
const float InputShape::fg_depth = 2.0f;
const std::array<unsigned char,4> InputShape::bg_color = { 64, 64, 64, 255 };
const std::array<unsigned char,4> InputShape::fg_color = { 100, 100, 100, 255 };

const glm::vec2 InputShape::plug_size(100,100);
const glm::vec2 InputShape::plug_border_size(10,10);
const float InputShape::plug_radius = 80;
const float InputShape::plug_offset = 10;


InputShape::InputShape(Entity* entity)
    : CompShape(entity, kDID()),
      _node_shape(this),
      _inputs(this) {
  get_dep_loader()->register_fixed_dep(_node_shape, Path({"..",".."}));
  get_dep_loader()->register_fixed_dep(_inputs, Path({"..",".."}));
}

InputShape::~InputShape() {
}

bool InputShape::is_exposed() const {
  external();
  size_t index = _inputs->get_exposed_index(get_name());
  if (index == -1) {
    return false;
  }
  return true;
}

HitRegion InputShape::hit_test(const glm::vec2& point) const {
  external();
  if (simple_hit_test(point)) {
    return HitRegion::kInputShapeRegion;
  }
  return HitRegion::kMissedRegion;
}

bool InputShape::update_state() {
  internal();
  size_t exposed_index = _inputs->get_exposed_index(get_name());
  if(exposed_index == -1) {
    _quads.resize(0); // Empty out our quads if we're not visible.
    return true;
  }

  // Make sure the shapes are allocated.
  _quads.resize(2);
  ShapeInstance* bg = &_quads[0];
  ShapeInstance* fg = &_quads[1];

  // Get the node bounds.
  const CompPolyBounds& bounds = _node_shape->get_bounds();

  // Get the node aa bounds.
  glm::vec2 node_min, node_max;
  bounds.get_aa_bounds(node_min, node_max);

  // Calculate the positioning.
  size_t num_exposed = _inputs->get_num_exposed();

  float min_x(node_min.x);
  float max_x(node_max.x);
  float delta = (max_x - min_x) / (num_exposed + 1);
  float start = min_x + delta;

  // Update the origin, which is the center of the quad.
  _origin = glm::vec2 (start + exposed_index * delta, node_max.y + plug_offset + plug_size.y / 2.0f);

  // Update our bounds.
  std::vector<glm::vec2>& verts = _bounds.poly_bound_map[HitRegion::kInputShapeRegion].vertices;
  verts.resize(4);
  verts[0] = glm::vec2(start + exposed_index * delta - plug_size.x / 2.0f, node_max.y + plug_offset);
  verts[1] = verts[0] + glm::vec2(plug_size.x, 0);
  verts[2] = verts[1] + glm::vec2(0, plug_size.y);
  verts[3] = verts[2] + glm::vec2(-plug_size.x, 0);

  // Update our pannable state.
  set_pannable(_node_shape->is_selected());

  // Update our bg quad.
  bg->set_scale(plug_size);
  bg->set_translate(verts[0], bg_depth);
  bg->set_color(bg_color);
  if (_node_shape->is_selected()) {
    bg->state |= selected_transform_bitmask;
  } else {
    bg->state &= ~selected_transform_bitmask;
  }

  // Update our fg quad.
  fg->set_scale(plug_size - 2.0f* plug_border_size);
  fg->set_translate(verts[0] + plug_border_size, fg_depth);
  fg->set_color(fg_color);
  if (_node_shape->is_selected()) {
    fg->state |= selected_transform_bitmask;
  } else {
    fg->state &= ~selected_transform_bitmask;
  }
  return true;
}

const CompPolyBounds& InputShape::get_bounds() const {
  external();
  return _bounds;
}

const std::vector<ShapeInstance>* InputShape::get_quad_instances() const {
  external();
  return &_quads;
}

const glm::vec2& InputShape::get_origin() const {
  external();
  return _origin;
}

// We (the InputShape instance) can't grab a dep to the LinkShape as it will create a cycle.
// The link shape has a dependency on us. This is why we return the entity of the link shape.
Entity* InputShape::find_link_entity() const {
  external();
  std::unordered_set<Entity*> dependants = get_dependants_by_did(ComponentIID::kICompShape, ComponentDID::kLinkShape);
  assert(dependants.size() <= 1);
  if (dependants.size()>0) {
    return *dependants.begin();
  }
  return NULL;
}

}
