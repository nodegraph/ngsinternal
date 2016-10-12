#include "outputnodeshape.h"

namespace ngs {

const float OutputNodeShape::indicator_bg_depth = 1.0f;
const float OutputNodeShape::indicator_fg_depth = 2.0f;
const float OutputNodeShape::indicator_offset = 140.0f;

const glm::vec2 OutputNodeShape::indicator_size(130.0f, 130.0f);
const glm::vec2 OutputNodeShape::indicator_border_size(10,10);

const std::array<unsigned char,4> OutputNodeShape::marker_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> OutputNodeShape::marker_fg_color = { 255, 128, 171, 255 };


OutputNodeShape::OutputNodeShape(Entity* entity)
    : RectNodeShape(entity, kDID()) {
  _marker_bg.state = 0;
  _marker_fg.state = 0;
}

OutputNodeShape::~OutputNodeShape() {
}

bool OutputNodeShape::update_state() {
  internal();
  RectNodeShape::update_state();

  const Polygon& bounds = get_bounds();
  glm::vec2 center = 0.5f * (bounds.vertices[0] + bounds.vertices[3]) - glm::vec2(indicator_offset, 0);
  glm::vec2 pos = center - 0.5f * indicator_size;

  // Update our bg quad.
  _marker_bg.set_scale(indicator_size);
  _marker_bg.set_rotate(0);
  _marker_bg.set_translate(pos, indicator_bg_depth);
  _marker_bg.set_color(marker_bg_color);
  if (is_selected()) {
    _marker_bg.state |= selected_transform_bitmask;
  } else {
    _marker_bg.state &= ~selected_transform_bitmask;
  }

  // Update our fg qued.
  _marker_fg.set_scale(indicator_size -  2.0f*indicator_border_size);
  _marker_fg.set_rotate(0);
  _marker_fg.set_translate(pos + indicator_border_size, indicator_fg_depth);
  _marker_fg.set_color(marker_fg_color);
  if (is_selected()) {
    _marker_fg.state |= selected_transform_bitmask;
  } else {
    _marker_fg.state &= ~selected_transform_bitmask;
  }

  // Append our marker shapes onto the quads_cache.
  _quads_cache.insert(_quads_cache.end(), _marker_bg);
  _quads_cache.insert(_quads_cache.end(), _marker_fg);

  return true;
}

}
