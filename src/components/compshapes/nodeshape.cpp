#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <components/resources/resources.h>
#include <base/device/geometry/textlimits.h>
#include <components/compshapes/nodeshape.h>

namespace ngs {

const float NodeShape::marker_bg_depth = 10.0f;
const float NodeShape::marker_fg_depth = 11.0f;

const glm::vec2 NodeShape::marker_border_size = glm::vec2(10.0f, 10.0f);

const std::array<unsigned char,4> NodeShape::edit_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> NodeShape::edit_fg_color = { 100, 221, 23, 255};
const std::array<unsigned char,4> NodeShape::view_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> NodeShape::view_fg_color = { 224, 64, 251, 255 };
const std::array<unsigned char,4> NodeShape::processing_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> NodeShape::processing_fg_color = { 239, 108, 0, 255};


NodeShape::NodeShape(Entity* entity, ComponentDID did)
    : SelectableShape(entity, did),
      _resources(this),
      _shared_state(0),
      _show_edit_marker(false),
      _show_view_marker(false),
      _show_compute_marker(false){
  get_dep_loader()->register_fixed_dep(_resources, Path({}));

  _edit_quad_bg.state = 0;
  _edit_quad_fg.state = 0;
  _view_quad_bg.state = 0;
  _view_quad_fg.state = 0;
  _processing_quad_bg.state = 0;
  _processing_quad_fg.state = 0;
}

NodeShape::~NodeShape() {
}

HitRegion NodeShape::hit_test(const glm::vec2& point) const {
  external();
  if (!simple_hit_test(point)) {
    return kMissed;
  }
  return kNodeShapeRegion;
}

void NodeShape::select(bool selected) {
  external();
  SelectableShape::select(selected);

  if (selected) {
    _shared_state |= selected_transform_bitmask;
  } else {
    _shared_state &= ~selected_transform_bitmask;
  }

  _edit_quad_bg.state = _shared_state;
  _edit_quad_fg.state = _shared_state;
  _view_quad_bg.state = _shared_state;
  _view_quad_fg.state = _shared_state;
  _processing_quad_bg.state = _shared_state;
  _processing_quad_fg.state = _shared_state;

  // Update the chars to the shared state.
  for (CharInstance& ci: _edit_chars) {
    ci.set_state(_shared_state);
  }
  for (CharInstance& ci: _view_chars) {
    ci.set_state(_shared_state);
  }
  for (CharInstance& ci: _processing_chars) {
    ci.set_state(_shared_state);
  }
}

void NodeShape::show_edit_marker(bool on) {
  external();
  _show_edit_marker = on;
}
bool NodeShape::edit_marker_is_showing() const {
  external();
  return _show_edit_marker;
}

void NodeShape::show_view_marker(bool on) {
  external();
  _show_view_marker = on;
}
bool NodeShape::view_marker_is_showing() const {
  external();
  return _show_view_marker;
}

void NodeShape::show_compute_marker(bool on) {
  external();
  _show_compute_marker = on;
}
bool NodeShape::compute_marker_is_showing() const {
  external();
  return _show_compute_marker;
}

void NodeShape::update_quads(const glm::vec2& pen) {
  //glm::vec2 text_dim = _text_max - _text_min;
  glm::vec2 start = pen; //_pos + glm::vec2(text_dim.x + 80, -40);

  if (_show_edit_marker) {
    glm::vec2 size(150, 150);
    _edit_quad_bg.set_scale(size);
    _edit_quad_bg.set_rotate(0);
    _edit_quad_bg.set_translate(start, marker_bg_depth);
    _edit_quad_bg.set_color(edit_bg_color);

    _edit_quad_fg.set_scale(size - 2.0f * marker_border_size);
    _edit_quad_fg.set_rotate(0);
    _edit_quad_fg.set_translate(start + marker_border_size, marker_fg_depth);
    _edit_quad_fg.set_color(edit_fg_color);

    // Update the start pos.
    start.x += 160;
  }

  if (_show_view_marker) {
    glm::vec2 size(150, 150);
    _view_quad_bg.set_scale(size);
    _view_quad_bg.set_rotate(0);
    _view_quad_bg.set_translate(start, marker_bg_depth);
    _view_quad_bg.set_color(view_bg_color);

    _view_quad_fg.set_scale(size - 2.0f * marker_border_size);
    _view_quad_fg.set_rotate(0);
    _view_quad_fg.set_translate(start + marker_border_size, marker_fg_depth);
    _view_quad_fg.set_color(view_fg_color);

    // Update the start pos.
    start.x += 160;
  }



  if (_show_compute_marker) {
    glm::vec2 size(150, 150);
    _processing_quad_bg.set_scale(size);
    _processing_quad_bg.set_rotate(0);
    _processing_quad_bg.set_translate(start, marker_bg_depth);
    _processing_quad_bg.set_color(processing_bg_color);

    _processing_quad_fg.set_scale(size - 2.0f * marker_border_size);
    _processing_quad_fg.set_rotate(0);
    _processing_quad_fg.set_translate(start + marker_border_size, marker_fg_depth);
    _processing_quad_fg.set_color(processing_fg_color);

    // Update the start pos.
    start.x += 160;
  }
}

void NodeShape::update_quads_cache() {
  _quads_cache.clear();
  if (_show_edit_marker) {
    _quads_cache.insert(_quads_cache.end(), _edit_quad_bg);
    _quads_cache.insert(_quads_cache.end(), _edit_quad_fg);
  }
  if (_show_view_marker) {
    _quads_cache.insert(_quads_cache.end(), _view_quad_bg);
    _quads_cache.insert(_quads_cache.end(), _view_quad_fg);
  }
  if (_show_compute_marker) {
    _quads_cache.insert(_quads_cache.end(), _processing_quad_bg);
    _quads_cache.insert(_quads_cache.end(), _processing_quad_fg);
  }
}

void NodeShape::update_edit_view_text() {
  glm::vec2 extra_chars_min;
  glm::vec2 extra_chars_max;

  if (_show_edit_marker) {
    glm::vec2 pos(_edit_quad_bg.translate[0], _edit_quad_bg.translate[1]);
    pos += glm::vec2(40, 40);
    _resources->get_text_limits()->tessellate_to_instances("E", glm::vec2(0,0), 0, pos, _shared_state, _edit_chars, extra_chars_min, extra_chars_max);
  }
  if (_show_view_marker) {
    glm::vec2 pos(_view_quad_bg.translate[0], _view_quad_bg.translate[1]);
    pos += glm::vec2(40, 40);
    _resources->get_text_limits()->tessellate_to_instances("V", glm::vec2(0,0), 0, pos, _shared_state, _view_chars, extra_chars_min, extra_chars_max);
  }
  if (_show_compute_marker) {
    glm::vec2 pos(_processing_quad_bg.translate[0], _processing_quad_bg.translate[1]);
    pos += glm::vec2(40, 40);
    _resources->get_text_limits()->tessellate_to_instances("P", glm::vec2(0,0), 0, pos, _shared_state, _processing_chars, extra_chars_min, extra_chars_max);
  }
}

void NodeShape::update_chars_cache() {
  _chars_cache.clear();
  if (_show_edit_marker) {
    _chars_cache.insert(_chars_cache.end(), _edit_chars.begin(), _edit_chars.end());
  }
  if (_show_view_marker) {
    _chars_cache.insert(_chars_cache.end(), _view_chars.begin(), _view_chars.end());
  }
  if (_show_compute_marker) {
    _chars_cache.insert(_chars_cache.end(), _processing_chars.begin(), _processing_chars.end());
  }
}

void NodeShape::update_tris_cache() {
  _tris_cache.clear();
}

void NodeShape::update_circles_cache() {
  _circles_cache.clear();
}

}
