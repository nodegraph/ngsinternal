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

const std::array<unsigned char, 4> NodeShape::node_bg_color = { 64, 64, 64, 255 };
const std::array<unsigned char, 4> NodeShape::node_fg_color = { 100, 100, 100, 255 };

const std::array<unsigned char,4> NodeShape::edit_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> NodeShape::edit_fg_color = { 100, 221, 23, 255};
const std::array<unsigned char,4> NodeShape::view_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> NodeShape::view_fg_color = { 224, 64, 251, 255 };

const glm::vec2 NodeShape::node_border_size = glm::vec2(10.0f, 10.0f);

NodeShape::NodeShape(Entity* entity)
    : LinkableShape(entity, kDID()),
      _resources(this),
      _color(node_bg_color),
      _being_edited(false),
      _being_viewed(false) {
  get_dep_loader()->register_fixed_dep(_resources, "");

  _node_quad_bg.state = 0;
  _node_quad_fg.state = 0;
  _edit_quad_bg.state = 0;
  _edit_quad_fg.state = 0;
  _view_quad_bg.state = 0;
  _view_quad_fg.state = 0;
}

NodeShape::NodeShape(Entity* entity, size_t did)
    : LinkableShape(entity, did),
      _resources(this),
      _color(node_bg_color),
      _being_edited(false),
      _being_viewed(false) {
  get_dep_loader()->register_fixed_dep(_resources, "");

  _node_quad_bg.state = 0;
  _node_quad_fg.state = 0;
  _edit_quad_bg.state = 0;
  _edit_quad_fg.state = 0;
  _view_quad_bg.state = 0;
  _view_quad_fg.state = 0;
}

NodeShape::~NodeShape() {
}

void NodeShape::select(bool selected) {

  std::cerr << "node shape select: " << selected << "\n";

  start_method();
  if (selected) {
    _node_quad_bg.state |= (selected_transform_bitmask|selected_color_bitmask);
    _node_quad_fg.state |= selected_transform_bitmask;
    _edit_quad_bg.state |= selected_transform_bitmask;
    _edit_quad_fg.state |= selected_transform_bitmask;
    _view_quad_bg.state |= selected_transform_bitmask;
    _view_quad_fg.state |= selected_transform_bitmask;
  } else {
    _node_quad_bg.state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _node_quad_fg.state &= ~selected_transform_bitmask;
    _edit_quad_bg.state &= ~selected_transform_bitmask;
    _edit_quad_fg.state &= ~selected_transform_bitmask;
    _view_quad_bg.state &= ~selected_transform_bitmask;
    _view_quad_fg.state &= ~selected_transform_bitmask;
  }

  // Update the current chars to the selected state.
  // We use the bg quad's state as the canonical state.
  for (CharInstance& ci: _node_name_chars) {
    ci.set_state(_node_quad_fg.state);
  }
  for (CharInstance& ci: _edit_chars) {
    ci.set_state(_node_quad_fg.state);
  }
  for (CharInstance& ci: _view_chars) {
    ci.set_state(_node_quad_fg.state);
  }
}

bool NodeShape::is_selected() const {
  start_method();
  if (_node_quad_bg.state & selected_transform_bitmask) {
    return true;
  }
  return false;
}

void NodeShape::edit(bool on) {
  start_method();
  _being_edited = on;
}
bool NodeShape::is_being_edited() const {
  start_method();
  return _being_edited;
}

void NodeShape::view(bool on) {
  start_method();
  _being_viewed = on;
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
  return kShape;
}

void NodeShape::update_state() {
  // Update our chars.
  update_text();

  // Update our quads.
  update_node_quads();
  update_edit_view_quads();

  // The edit and view text position relies on the edit and view quads.
  update_edit_view_text();

  // Update our accumulation caches.
  update_chars_cache();
  update_quads_cache();

  // We have no triangles.
  _tris_cache.clear();
}

void NodeShape::update_node_quads() {
  // Bg Quad.
  glm::vec2 text_dim = _text_max - _text_min;
  glm::vec2 bg_min = _pos-glm::vec2(70,40);
  glm::vec2 bg_max = _pos+glm::vec2(text_dim.x+70,110);
  glm::vec2 bg_dim = bg_max-bg_min;

  // Update our bounds.
  {
    std::vector<glm::vec2>& verts = _bg_bounds.vertices;
    verts.resize(4);
    verts[0] = bg_min;
    verts[1] = verts[0] + glm::vec2(bg_dim.x,0);
    verts[2] = verts[1] + glm::vec2(0, bg_dim.y);
    verts[3] = verts[2] + glm::vec2(-bg_dim.x, 0);
  }

  // Node.
  _node_quad_bg.set_scale(bg_dim);
  _node_quad_bg.set_rotate(0);
  _node_quad_bg.set_translate(bg_min, node_bg_depth);
  _node_quad_bg.set_color(node_bg_color);

  _node_quad_fg.set_scale(bg_dim - 2.0f * node_border_size);
  _node_quad_fg.set_rotate(0);
  _node_quad_fg.set_translate(bg_min + node_border_size, node_fg_depth);
  _node_quad_fg.set_color(node_fg_color);
}

void NodeShape::update_edit_view_quads() {

  glm::vec2 text_dim = _text_max - _text_min;
  glm::vec2 start = _pos + glm::vec2(text_dim.x + 80, -40);

  if (_being_edited) {
    glm::vec2 size(150, 150);
    _edit_quad_bg.set_scale(size);
    _edit_quad_bg.set_rotate(0);
    _edit_quad_bg.set_translate(start, node_bg_depth);
    _edit_quad_bg.set_color(edit_bg_color);

    _edit_quad_fg.set_scale(size - 2.0f * node_border_size);
    _edit_quad_fg.set_rotate(0);
    _edit_quad_fg.set_translate(start + node_border_size, node_fg_depth);
    _edit_quad_fg.set_color(edit_fg_color);

    // Update the start pos.
    start.x += 160;
  }

  if (_being_viewed) {
    glm::vec2 size(150, 150);
    _view_quad_bg.set_scale(size);
    _view_quad_bg.set_rotate(0);
    _view_quad_bg.set_translate(start, node_bg_depth);
    _view_quad_bg.set_color(view_bg_color);

    _view_quad_fg.set_scale(size - 2.0f * node_border_size);
    _view_quad_fg.set_rotate(0);
    _view_quad_fg.set_translate(start + node_border_size, node_fg_depth);
    _view_quad_fg.set_color(view_fg_color);
  }

}

void NodeShape::update_quads_cache() {
  size_t size = 6; // Potentially overallocate for speed. 2 each for node, edit and view visuals.
  _quads_cache.clear();
  _quads_cache.reserve(size);

  _quads_cache.insert(_quads_cache.end(), _node_quad_bg);
  _quads_cache.insert(_quads_cache.end(), _node_quad_fg);
  if (_being_edited) {
    _quads_cache.insert(_quads_cache.end(), _edit_quad_bg);
    _quads_cache.insert(_quads_cache.end(), _edit_quad_fg);
  }
  if (_being_viewed) {
    _quads_cache.insert(_quads_cache.end(), _view_quad_bg);
    _quads_cache.insert(_quads_cache.end(), _view_quad_fg);
  }
}

void NodeShape::update_text() {
  start_method();
  const std::string& name = our_entity()->get_name();
  const glm::vec2 &anchor = _pos;
  _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(0,0), 0, anchor, _node_quad_fg.state, _node_name_chars, _text_min, _text_max);
}

void NodeShape::update_edit_view_text() {
  start_method();
  glm::vec2 extra_chars_min;
  glm::vec2 extra_chars_max;
  glm::vec2 text_dim = _text_max - _text_min;

  if (_being_edited) {
    glm::vec2 pos(_edit_quad_bg.translate[0], _edit_quad_bg.translate[1]);
    pos += glm::vec2(40, 40);
    _resources->get_text_limits()->tessellate_to_instances("E", glm::vec2(0,0), 0, pos, _node_quad_fg.state, _edit_chars, extra_chars_min, extra_chars_max);
  }
  if (_being_viewed) {
    glm::vec2 pos(_view_quad_bg.translate[0], _view_quad_bg.translate[1]);
    pos += glm::vec2(40, 40);
    _resources->get_text_limits()->tessellate_to_instances("V", glm::vec2(0,0), 0, pos, _node_quad_fg.state, _view_chars, extra_chars_min, extra_chars_max);
  }
}

void NodeShape::update_chars_cache() {
  size_t size = _node_name_chars.size() + _edit_chars.size(); _view_chars.size();
  _chars_cache.clear();
  _chars_cache.reserve(size);

  _chars_cache.insert(_chars_cache.end(), _node_name_chars.begin(), _node_name_chars.end());
  if (_being_edited) {
    _chars_cache.insert(_chars_cache.end(), _edit_chars.begin(), _edit_chars.end());
  }
  if (_being_viewed) {
    _chars_cache.insert(_chars_cache.end(), _view_chars.begin(), _view_chars.end());
  }
}

void NodeShape::push_input_name(const std::string& input_name) {
  _linkable_input_names.push_back(input_name);
}

void NodeShape::push_output_name(const std::string& output_name) {
  _linkable_output_names.push_back(output_name);
}

size_t NodeShape::get_input_order(const std::string& input_name) const {
  for (size_t i=0; i<_linkable_input_names.size(); ++i) {
    if (_linkable_input_names[i] == input_name) {
      return i;
    }
  }
  return 0;
}

size_t NodeShape::get_output_order(const std::string& output_name) const {
  for (size_t i=0; i<_linkable_output_names.size(); ++i) {
    if (_linkable_output_names[i] == output_name) {
      return i;
    }
  }
  return 0;
}

size_t NodeShape::get_num_linkable_inputs() const {
  return _linkable_input_names.size();
}

size_t NodeShape::get_num_linkable_outputs() const {
  return _linkable_output_names.size();
}

size_t NodeShape::get_num_input_params() const {
  return get_num_all_inputs() - get_num_linkable_inputs();
}

size_t NodeShape::get_num_output_params() const {
  return get_num_all_outputs() - get_num_linkable_outputs();
}

size_t NodeShape::get_num_all_inputs() const {
  start_method();
  return get_entity("./inputs")->get_children().size();
}

size_t NodeShape::get_num_all_outputs() const {
  start_method();
  return get_entity("./outputs")->get_children().size();
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
  // Ordered input names.
  saver.save_vector(_linkable_input_names);
  // Ordered output names.
  saver.save_vector(_linkable_output_names);
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
  // Ordered input names.
  loader.load_vector(_linkable_input_names);
  // Ordered output names.
  loader.load_vector(_linkable_output_names);
}

}
