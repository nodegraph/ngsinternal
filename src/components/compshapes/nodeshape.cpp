#include "nodeshape.h"
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/device/geometry/textlimits.h>

#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/nodegraphselection.h>
#include <components/compshapes/outputshape.h>
#include <components/resources/resources.h>

#include <components/computes/compute.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

// Externals.
#include <freetype-gl/texture-font.h>

// STL.
#include <iostream>

namespace ngs {

const float NodeShape::node_bg_depth = 10.0f;
const float NodeShape::node_fg_depth = 11.0f;
const float NodeShape::button_bg_depth = 12.0f;
const float NodeShape::button_fg_depth = 13.0f;

const std::array<unsigned char, 4> NodeShape::node_bg_color = { 38, 38, 38, 255 };
const std::array<unsigned char, 4> NodeShape::node_fg_color = { 64, 64, 64, 255 };
const std::array<unsigned char, 4> NodeShape::right_bg_color = { 38, 38, 38, 255 };
const std::array<unsigned char, 4> NodeShape::right_fg_on_color = { 50, 255, 50, 255 };
const std::array<unsigned char, 4> NodeShape::right_fg_off_color = { 90, 90, 90, 255 };
const std::array<unsigned char, 4> NodeShape::left_bg_color = { 38, 38, 38, 255 };
const std::array<unsigned char, 4> NodeShape::left_fg_on_color = { 50, 50, 255, 255 };
const std::array<unsigned char, 4> NodeShape::left_fg_off_color = { 90, 90, 90, 255 };

const glm::vec2 NodeShape::node_border_size = glm::vec2(6.0f, 6.0f);
const glm::vec2 NodeShape::button_border_size = glm::vec2(6.0f, 6.0f);
const glm::vec2 NodeShape::button_offset = glm::vec2(12.0f, 12.0f);

NodeShape::NodeShape(Entity* entity)
    : CompShape(entity, kDID()),
      _resources(this),
      _node_compute(this),
      _being_edited(false),
      _being_viewed(false) {
  get_dep_loader()->register_fixed_dep(_resources, "");
  get_dep_loader()->register_fixed_dep(_node_compute, ".");
  init(6);
}

NodeShape::NodeShape(Entity* entity, size_t did, size_t num_extra_quads)
    : CompShape(entity, did),
      _resources(this),
      _node_compute(this),
      _being_edited(false),
      _being_viewed(false) {
  get_dep_loader()->register_fixed_dep(_resources, "");
  get_dep_loader()->register_fixed_dep(_node_compute, ".");
  init(6 + num_extra_quads);
}

NodeShape::~NodeShape() {
}

void NodeShape::init(size_t num_quads) {
  _quads.resize(num_quads);
  _bg_quad = &_quads[0];
  _fg_quad = &_quads[1];
  _left_bg_quad = &_quads[2];
  _left_fg_quad = &_quads[3];
  _right_bg_quad = &_quads[4];
  _right_fg_quad = &_quads[5];

  _color = node_bg_color;
}

void NodeShape::select(bool selected) {
  start_method();
  if (selected) {
    _bg_quad->state |= (selected_transform_bitmask|selected_color_bitmask);
    _fg_quad->state |= selected_transform_bitmask;
    _left_bg_quad->state |= selected_transform_bitmask;
    _left_fg_quad->state |= selected_transform_bitmask;
    _right_bg_quad->state |= selected_transform_bitmask;
    _right_fg_quad->state |= selected_transform_bitmask;
  } else {
    _bg_quad->state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _fg_quad->state &= ~selected_transform_bitmask;
    _left_bg_quad->state &= ~selected_transform_bitmask;
    _left_fg_quad->state &= ~selected_transform_bitmask;
    _right_bg_quad->state &= ~selected_transform_bitmask;
    _right_fg_quad->state &= ~selected_transform_bitmask;
  }

  // Update the current chars to the selected state.
  // We use the bg quad's state as the canonical state.
  for (CharInstance& ci: _chars) {
    ci.set_state(_fg_quad->state);
  }
}

bool NodeShape::is_selected() const {
  start_method();
  if (_bg_quad->state & selected_transform_bitmask) {
    return true;
  }
  return false;
}

void NodeShape::edit(bool on) {
  start_method();
  _being_edited = on;
  if (on) {
    _right_bg_quad->set_color(right_bg_color);
  } else {
    _right_bg_quad->set_color(node_bg_color);
  }
}

bool NodeShape::is_being_edited() const {
  start_method();
  return _being_edited;
}

void NodeShape::view(bool on) {
  start_method();
  _being_viewed = on;
  if (on) {
    _left_bg_quad->set_color(left_bg_color);
  } else {
    _left_bg_quad->set_color(node_bg_color);
  }
}

bool NodeShape::is_being_viewed() const {
  start_method();
  return _being_viewed;
}

void NodeShape::set_pos(const glm::vec2& pos) {
  start_method();
  _pos = pos;
}

const glm::vec2& NodeShape::get_pos() const {
  start_method();
  return _pos;
}

const Polygon& NodeShape::get_bounds() const {
  start_method();
  return _bg_bounds;
}

HitRegion NodeShape::hit_test(const glm::vec2& point) const {
  start_method();
  if (!simple_hit_test(point)) {
    return kMissed;
  }
  if (_right_bounds.contains(point)) {
    return kEditButton;
  }
  if (_left_bounds.contains(point)) {
    return kViewButton;
  }
  return kShape;
}

void NodeShape::update_state() {
  // Update our text.
  update_text();

  // Bg Quad.
  glm::vec2 text_dim = _text_max - _text_min;
  glm::vec2 bg_min = _pos-glm::vec2(200,40);
  glm::vec2 bg_max = _pos+glm::vec2(text_dim.x+200,110);
  glm::vec2 bg_dim = bg_max-bg_min;

  glm::vec2 left_min = bg_min + button_offset;
  glm::vec2 left_dim = glm::vec2(bg_dim.y-2.0f * button_offset.y);
  glm::vec2 left_max = left_min + left_dim;

  glm::vec2 right_max = bg_max - button_offset;
  glm::vec2 right_dim = left_dim;
  glm::vec2 right_min = right_max - right_dim;

  // Update our bounds.
  {
    std::vector<glm::vec2>& verts = _bg_bounds.vertices;
    verts.resize(4);
    verts[0] = bg_min;
    verts[1] = verts[0] + glm::vec2(bg_dim.x,0);
    verts[2] = verts[1] + glm::vec2(0, bg_dim.y);
    verts[3] = verts[2] + glm::vec2(-bg_dim.x, 0);
  }
  {
    std::vector<glm::vec2>& verts = _left_bounds.vertices;
    verts.resize(4);
    verts[0] = left_min;
    verts[1] = verts[0] + glm::vec2(left_dim.x,0);
    verts[2] = verts[1] + glm::vec2(0, left_dim.y);
    verts[3] = verts[2] + glm::vec2(-left_dim.x, 0);
  }
  {
    std::vector<glm::vec2>& verts = _right_bounds.vertices;
    verts.resize(4);
    verts[0] = right_min;
    verts[1] = verts[0] + glm::vec2(right_dim.x,0);
    verts[2] = verts[1] + glm::vec2(0, right_dim.y);
    verts[3] = verts[2] + glm::vec2(-right_dim.x, 0);
  }

  // Node.
  _bg_quad->set_scale(bg_dim);
  _bg_quad->set_translate(bg_min, node_bg_depth);
  _bg_quad->set_color(node_bg_color);

  _fg_quad->set_scale(bg_dim - 2.0f * node_border_size);
  _fg_quad->set_translate(bg_min + node_border_size, node_fg_depth);
  _fg_quad->set_color(node_fg_color);

  // Left Button.
  _left_bg_quad->set_scale(left_dim);
  _left_bg_quad->set_translate(left_min, button_bg_depth);
  _left_bg_quad->set_color(left_bg_color);

  _left_fg_quad->set_scale(left_dim - 2.0f*button_border_size);
  _left_fg_quad->set_translate(left_min + button_border_size, button_fg_depth);
  if (is_being_viewed()) {
    _left_fg_quad->set_color(left_fg_on_color);
  } else {
    _left_fg_quad->set_color(left_fg_off_color);
  }

  // Right Button.
  _right_bg_quad->set_scale(right_dim);
  _right_bg_quad->set_translate(right_min, button_bg_depth);
  _right_bg_quad->set_color(right_bg_color);

  _right_fg_quad->set_scale(right_dim - 2.0f*button_border_size);
  _right_fg_quad->set_translate(right_min + button_border_size, button_fg_depth);
  if (is_being_viewed()) {
    _right_fg_quad->set_color(right_fg_on_color);
  } else {
    _right_fg_quad->set_color(right_fg_off_color);
  }
}

void NodeShape::update_text() {
  start_method();
  const std::string& name = our_entity()->get_name();
  const glm::vec2 &anchor = _pos;
  _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(0,0), 0, anchor, _fg_quad->state, _chars, _text_min, _text_max);
}

size_t NodeShape::get_input_order(const std::string& input_name) const {
  return _node_compute->get_input_order(input_name);
}

size_t NodeShape::get_output_order(const std::string& output_name) const {
  return _node_compute->get_output_order(output_name);
}

void NodeShape::save(SimpleSaver& saver) const {
  start_method();
  CompShape::save(saver);
  // Position.
  saver.save(_pos.x);
  saver.save(_pos.y);
  // Color.
  saver.save(_color[0]);
  saver.save(_color[1]);
  saver.save(_color[2]);
  saver.save(_color[3]);
}
void NodeShape::load(SimpleLoader& loader) {
  start_method();
  CompShape::load(loader);
  // Position.
  loader.load(_pos.x);
  loader.load(_pos.y);
  // Color.
  loader.load(_color[0]);
  loader.load(_color[1]);
  loader.load(_color[2]);
  loader.load(_color[3]);
}

}
