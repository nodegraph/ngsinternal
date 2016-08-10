#include "dotnodeshape.h"

#include <base/objectmodel/entity.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <base/device/geometry/circle.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

namespace ngs {

const float DotNodeShape::bg_depth = 10;
const float DotNodeShape::fg_depth = 11;
const std::array<unsigned char, 4> DotNodeShape::bg_color = { 64, 64, 64, 255 };
const std::array<unsigned char, 4> DotNodeShape::fg_color = { 100, 100, 100, 255 };

const float DotNodeShape::border_width = 10.0f;
const float DotNodeShape::radius = 50.0f;


DotNodeShape::DotNodeShape(Entity* entity):
    LinkableShape(entity, kDID()) {
  _bg_circle.state = 0;
  _fg_circle.state = 0;
}

DotNodeShape::~DotNodeShape() {
}

void DotNodeShape::update_state() {
  // Update our bounds.
  {
    std::vector<glm::vec2> &verts = _bounds.vertices;
    verts = Circle::get_samples(Circle::num_samples, radius, _pos);
  }

  // Node.
  _bg_circle.set_scale(radius,radius);
  _bg_circle.set_translate(_pos, bg_depth);
  _bg_circle.set_color(bg_color);

  _fg_circle.set_scale(radius-border_width, radius-border_width);
  _fg_circle.set_translate(_pos, fg_depth);
  _fg_circle.set_color(fg_color);

  // Update edit and view quads.
  glm::vec2 start(_pos.x + radius + 30, _pos.y - 75);
  update_edit_view_quads(start);

  // The edit and view text position relies on the edit and view quads.
  update_edit_view_text();

  // Update our accumulation caches.
  update_chars_cache();
  update_quads_cache();
  update_circles_cache();
}

void DotNodeShape::set_pos(const glm::vec2& anchor) {
  start_method();
  _pos = anchor;
}

const glm::vec2& DotNodeShape::get_pos() const {
  start_method();
  return _pos;
}

const Polygon& DotNodeShape::get_bounds() const {
  start_method();
  return _bounds;
}

void DotNodeShape::select(bool selected) {
  start_method();
  LinkableShape::select(selected);
  if (selected) {
    _bg_circle.state |= (selected_transform_bitmask|selected_color_bitmask);
    _fg_circle.state |= selected_transform_bitmask;
  } else {
    _bg_circle.state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _fg_circle.state &= ~selected_transform_bitmask;
  }
}

void DotNodeShape::save(SimpleSaver& saver) const {
  start_method();
  CompShape::save(saver);
  saver.save(_pos.x);
  saver.save(_pos.y);
}
void DotNodeShape::load(SimpleLoader& loader) {
  start_method();
  CompShape::load(loader);
  loader.load(_pos.x);
  loader.load(_pos.y);
}

size_t DotNodeShape::get_input_order(const std::string& input_name) const {
  if (input_name == "in") {
    return 0;
  }
  return -1;
}

size_t DotNodeShape::get_output_order(const std::string& output_name) const {
  if (output_name == "out") {
    return 0;
  }
  return -1;
}

void DotNodeShape::update_circles_cache() {
  LinkableShape::update_circles_cache();
  _circles_cache.insert(_circles_cache.end(), _bg_circle);
  _circles_cache.insert(_circles_cache.end(), _fg_circle);
}

}
