#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <components/resources/resources.h>
#include <base/device/geometry/textlimits.h>
#include <components/compshapes/nodeshape.h>

namespace ngs {

// -------------------------------------------------------------------------------------------
// NodeMarker.
// -------------------------------------------------------------------------------------------

const float NodeMarker::_bg_depth = 10.0f;
const float NodeMarker::_fg_depth = 11.0f;
const glm::vec2 NodeMarker::_border_size = glm::vec2(10.0f, 10.0f);

NodeMarker::NodeMarker():
  _show_marker(false) {
  _bg_quad.state = 0;
  _fg_quad.state = 0;
}

NodeMarker::~NodeMarker() {
}

void NodeMarker::set_bg_color(const std::array<unsigned char,4>& bg_color) {
  _bg_color = bg_color;
}

void NodeMarker::set_fg_color(const std::array<unsigned char,4>& fg_color) {
  _fg_color = fg_color;
}

void NodeMarker::set_letter(const std::string& letter) {
 _letter = letter;
}

void NodeMarker::set_state(unsigned char state) {
  // Update the quads.
  _bg_quad.state = state;
  _fg_quad.state = state;
  // Update the chars.
  for (CharInstance& ci: _chars) {
    ci.set_state(state);
  }
}

void NodeMarker::update_quads(glm::vec2& start) {
  if (_show_marker) {
    glm::vec2 size(150, 150);
    _bg_quad.set_scale(size);
    _bg_quad.set_rotate(0);
    _bg_quad.set_translate(start, _bg_depth);
    _bg_quad.set_color(_bg_color);

    _fg_quad.set_scale(size - 2.0f * _border_size);
    _fg_quad.set_rotate(0);
    _fg_quad.set_translate(start + _border_size, _fg_depth);
    _fg_quad.set_color(_fg_color);

    // Update the start pos.
    start.x += 160;
  }
}

void NodeMarker::update_quads_cache(std::vector<ShapeInstance>& quads_cache) {
  if (_show_marker) {
    quads_cache.insert(quads_cache.end(), _bg_quad);
    quads_cache.insert(quads_cache.end(), _fg_quad);
  }
}

void NodeMarker::update_chars(Resources* resources, unsigned char state) {
  glm::vec2 extra_chars_min;
  glm::vec2 extra_chars_max;
  if (_show_marker) {
    glm::vec2 pos(_bg_quad.translate[0], _bg_quad.translate[1]);
    pos += glm::vec2(40, 40);
    resources->get_text_limits()->tessellate_to_instances(_letter, glm::vec2(0,0), 0, pos, state, _chars, extra_chars_min, extra_chars_max);
  }
}

void NodeMarker::update_chars_cache(std::vector<CharInstance>& chars_cache) {
  if (_show_marker) {
    chars_cache.insert(chars_cache.end(), _chars.begin(), _chars.end());
  }
}


// -------------------------------------------------------------------------------------------
// NodeShape.
// -------------------------------------------------------------------------------------------

NodeShape::NodeShape(Entity* entity, ComponentDID did)
    : SelectableShape(entity, did),
      _resources(this),
      _shared_state(0) {
  get_dep_loader()->register_fixed_dep(_resources, Path( { }));
  _edit_marker.set_letter("E");
  _edit_marker.set_bg_color( { 255, 255, 255, 255 });
  _edit_marker.set_fg_color( { 100, 221, 23, 255 });

  _view_marker.set_letter("V");
  _view_marker.set_bg_color( { 255, 255, 255, 255 });
  _view_marker.set_fg_color( { 224, 64, 251, 255 });

  _compute_marker.set_letter("P");
  _compute_marker.set_bg_color( { 255, 255, 255, 255 });
  _compute_marker.set_fg_color( { 239, 108, 0, 255 });

  _clean_marker.set_letter("C");
  _clean_marker.set_bg_color( { 255, 255, 255, 255 });
  _clean_marker.set_fg_color( { 100, 221, 23, 255 });

  _error_marker.set_letter("X");
  _error_marker.set_bg_color( { 255, 255, 255, 255 });
  _error_marker.set_fg_color( { 183, 28, 28, 255 });
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

  _edit_marker.set_state(_shared_state);
  _view_marker.set_state(_shared_state);
  _compute_marker.set_state(_shared_state);
  _clean_marker.set_state(_shared_state);
  _error_marker.set_state(_shared_state);
}

void NodeShape::show_edit_marker(bool on) {
  external();
  _edit_marker.show(on);
}
bool NodeShape::edit_marker_is_showing() const {
  external();
  return _edit_marker.is_showing();
}

void NodeShape::show_view_marker(bool on) {
  external();
  _view_marker.show(on);
}
bool NodeShape::view_marker_is_showing() const {
  external();
  return _view_marker.is_showing();
}

void NodeShape::show_compute_marker(bool on) {
  external();
  _compute_marker.show(on);
}
bool NodeShape::compute_marker_is_showing() const {
  external();
  return _compute_marker.is_showing();
}

void NodeShape::show_clean_marker(bool on) {
  external();
  _clean_marker.show(on);
}
bool NodeShape::clean_marker_is_showing() const {
  external();
  return _clean_marker.is_showing();
}

void NodeShape::show_error_marker(bool on) {
  external();
  _error_marker.show(on);
}
bool NodeShape::error_marker_is_showing() const {
  external();
  return _error_marker.is_showing();
}

void NodeShape::update_quads(const glm::vec2& pen) {
  glm::vec2 start = pen;
  _edit_marker.update_quads(start);
  _view_marker.update_quads(start);
  _compute_marker.update_quads(start);
  _clean_marker.update_quads(start);
  _error_marker.update_quads(start);
}

void NodeShape::update_quads_cache() {
  _quads_cache.clear();
  _edit_marker.update_quads_cache(_quads_cache);
  _view_marker.update_quads_cache(_quads_cache);
  _compute_marker.update_quads_cache(_quads_cache);
  _clean_marker.update_quads_cache(_quads_cache);
  _error_marker.update_quads_cache(_quads_cache);
}

void NodeShape::update_edit_view_text() {
  _edit_marker.update_chars(_resources.get(), _shared_state);
  _view_marker.update_chars(_resources.get(), _shared_state);
  _compute_marker.update_chars(_resources.get(), _shared_state);
  _clean_marker.update_chars(_resources.get(), _shared_state);
  _error_marker.update_chars(_resources.get(), _shared_state);
}

void NodeShape::update_chars_cache() {
  _chars_cache.clear();
  _edit_marker.update_chars_cache(_chars_cache);
  _view_marker.update_chars_cache(_chars_cache);
  _compute_marker.update_chars_cache(_chars_cache);
  _clean_marker.update_chars_cache(_chars_cache);
  _error_marker.update_chars_cache(_chars_cache);
}

void NodeShape::update_tris_cache() {
  _tris_cache.clear();
}

void NodeShape::update_circles_cache() {
  _circles_cache.clear();
}

}
