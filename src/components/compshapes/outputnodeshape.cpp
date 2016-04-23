#include "outputnodeshape.h"

namespace ngs {

const float OutputNodeShape::indicator_bg_depth = 1.0f;
const float OutputNodeShape::indicator_fg_depth = 2.0f;
const float OutputNodeShape::indicator_offset = 140.0f;

const glm::vec2 OutputNodeShape::indicator_size(130.0f, 130.0f);
const glm::vec2 OutputNodeShape::indicator_border_size(6,6);

const std::array<unsigned char,4> OutputNodeShape::indicator_bg_color = { 38, 38, 38, 255 };
const std::array<unsigned char,4> OutputNodeShape::indicator_fg_color = { 255, 128, 171, 255 };


OutputNodeShape::OutputNodeShape(Entity* entity)
    : NodeShape(entity, kDID(), 2) {
  _bg_quad = &_quads[6];
  _fg_quad = &_quads[7];
}

OutputNodeShape::~OutputNodeShape() {
}

void OutputNodeShape::update_state() {
  NodeShape::update_state();

  const Polygon& bounds = get_bounds();
  glm::vec2 center = 0.5f * (bounds.vertices[0] + bounds.vertices[3]) - glm::vec2(indicator_offset, 0);
  glm::vec2 pos = center - 0.5f * indicator_size;

  // Update our bg quad.
  _bg_quad->set_scale(indicator_size);
  _bg_quad->set_rotate(0);
  _bg_quad->set_translate(pos, indicator_bg_depth);
  _bg_quad->set_color(indicator_bg_color);
  if (is_selected()) {
    _bg_quad->state |= selected_transform_bitmask;
  } else {
    _bg_quad->state &= ~selected_transform_bitmask;
  }

  // Update our fg qued.
  _fg_quad->set_scale(indicator_size -  2.0f*indicator_border_size);
  _fg_quad->set_rotate(0);
  _fg_quad->set_translate(pos + indicator_border_size, indicator_fg_depth);
  _fg_quad->set_color(indicator_fg_color);
  if (is_selected()) {
    _fg_quad->state |= selected_transform_bitmask;
  } else {
    _fg_quad->state &= ~selected_transform_bitmask;
  }
}

}
