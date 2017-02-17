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
    NodeShape(entity, kDID()) {
  _bg_circle.state = 0;
  _fg_circle.state = 0;
}

DotNodeShape::~DotNodeShape() {
}

bool DotNodeShape::update_state() {
  internal();
  // Update our bounds.
  {
    std::vector<glm::vec2> &verts = _border.poly_bound_map[HitRegion::kNodeRegion].vertices;
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
  update_quads(start);

  // The edit and view text position relies on the edit and view quads.
  update_edit_view_text();

  // Update our accumulation caches.
  update_chars_cache();
  update_quads_cache();
  update_circles_cache();
  return true;
}

void DotNodeShape::set_pos(const glm::vec2& anchor) {
  external();
  _pos = anchor;
}

const glm::vec2& DotNodeShape::get_pos() const {
  external();
  return _pos;
}

void DotNodeShape::select(bool selected) {
  external();
  NodeShape::select(selected);
  if (selected) {
    _bg_circle.state |= (selected_transform_bitmask|selected_color_bitmask);
    _fg_circle.state |= selected_transform_bitmask;
  } else {
    _bg_circle.state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _fg_circle.state &= ~selected_transform_bitmask;
  }
}

void DotNodeShape::save(SimpleSaver& saver) const {
  external();
  CompShape::save(saver);
  saver.save(_pos.x);
  saver.save(_pos.y);
}
void DotNodeShape::load(SimpleLoader& loader) {
  external();
  CompShape::load(loader);
  loader.load(_pos.x);
  loader.load(_pos.y);
}

void DotNodeShape::update_circles_cache() {
  internal();
  NodeShape::update_circles_cache();
  _circles_cache.insert(_circles_cache.end(), _bg_circle);
  _circles_cache.insert(_circles_cache.end(), _fg_circle);
}

}
