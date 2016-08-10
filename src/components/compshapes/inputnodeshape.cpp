#include "inputnodeshape.h"

namespace ngs {

const float InputNodeShape::indicator_bg_depth = 1.0f;
const float InputNodeShape::indicator_fg_depth = 2.0f;
const float InputNodeShape::indicator_border_width = 10.0f;
const float InputNodeShape::indicator_offset = 140.0f;

const glm::vec2 InputNodeShape::indicator_size(160.0f, 160.0f *sqrt(3.0f)/2.0f);

const std::array<unsigned char,4> InputNodeShape::indicator_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> InputNodeShape::indicator_fg_color = { 255, 128, 171, 255 };

InputNodeShape::InputNodeShape(Entity* entity)
    : NodeShape(entity, kDID()) {
  _marker_bg.state = 0;
  _marker_fg.state = 0;
}

InputNodeShape::~InputNodeShape() {
}

void InputNodeShape::update_state() {
  NodeShape::update_state();

  const Polygon& bounds = get_bounds();
  glm::vec2 center = 0.5f * (bounds.vertices[0] + bounds.vertices[3]) - glm::vec2(indicator_offset, 0);

  // In an isosceles triangle the center lies at 1/sqrt(3) from one of the flat sides,
  // or at 2/sqrt(3) from one of the tips of the triangle.

  glm::vec2 pos = center;
  pos.y -= indicator_size.y/2.0f;

  // Update our bg triangle.
  _marker_bg.set_scale(indicator_size);
  _marker_bg.set_rotate(0);
  _marker_bg.set_translate(pos, indicator_bg_depth);
  _marker_bg.set_color(indicator_bg_color);
  if (is_selected()) {
    _marker_bg.state |= selected_transform_bitmask;
  } else {
    _marker_bg.state &= ~selected_transform_bitmask;
  }

  // Update our fg triangle.
  // h' = h - 3*border_width
  // w' = w - 2*sqrt(3)*border_width
  _marker_fg.set_scale(indicator_size - glm::vec2(2.0f * sqrt(3.0f) * indicator_border_width, 3.0f * indicator_border_width));
  _marker_fg.set_rotate(0);
  _marker_fg.set_translate(pos + glm::vec2(0, 2*indicator_border_width), indicator_fg_depth);
  _marker_fg.set_color(indicator_fg_color);
  if (is_selected()) {
    _marker_fg.state |= selected_transform_bitmask;
  } else {
    _marker_fg.state &= ~selected_transform_bitmask;
  }

  // Append our marker shapes onto the quads_cache.
  _tris_cache.insert(_tris_cache.end(), _marker_bg);
  _tris_cache.insert(_tris_cache.end(), _marker_fg);
}


}
