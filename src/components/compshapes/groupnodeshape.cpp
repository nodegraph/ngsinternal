#include "groupnodeshape.h"
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <entities/entityids.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>

#include <algorithm>
#include <boost/math/constants/constants.hpp>

namespace ngs {

const float GroupNodeShape::indicator_bg_depth = 1.0f;
const float GroupNodeShape::indicator_fg_depth = 2.0f;
const float GroupNodeShape::indicator_offset = 140.0f;
const float GroupNodeShape::indicator_rotation = boost::math::constants::pi<float>()/4.0f;

const glm::vec2 GroupNodeShape::indicator_size(130.0f, 130.0f);
const glm::vec2 GroupNodeShape::indicator_border_size(10,10);

const std::array<unsigned char,4> GroupNodeShape::indicator_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> GroupNodeShape::indicator_fg_color = { 255, 128, 171, 255 };



GroupNodeShape::GroupNodeShape(Entity* entity)
    : RectNodeShape(entity, kDID()),
      _factory(this) {
  get_dep_loader()->register_fixed_dep(_factory, Path({}));

  _marker_bg.state = 0;
  _marker_fg.state = 0;
}

GroupNodeShape::~GroupNodeShape() {
}

bool GroupNodeShape::update_state() {
  internal();
  RectNodeShape::update_state();

  // The following updates the marker shapes.
  const CompPolyBounds& bounds = get_bounds();
  const PolyBounds& poly_bound = bounds.poly_bound_map.at(HitRegion::kNodeShapeRegion);
  glm::vec2 target_center = 0.5f * (poly_bound.vertices[0] + poly_bound.vertices[3]) - glm::vec2(indicator_offset, 0);
  glm::vec2 translate = target_center - glm::vec2(0, sqrt(2.0f)*indicator_size.x/2.0f);

  // Update our bg quad.
  _marker_bg.set_scale(indicator_size);
  _marker_bg.set_rotate(indicator_rotation);
  _marker_bg.set_translate(translate, indicator_bg_depth);
  _marker_bg.set_color(indicator_bg_color);
  if (is_selected()) {
    _marker_bg.state |= selected_transform_bitmask;
  } else {
    _marker_bg.state &= ~selected_transform_bitmask;
  }

  // Update our fg quad.
  _marker_fg.set_scale(indicator_size -  2.0f*indicator_border_size);
  _marker_fg.set_rotate(indicator_rotation);
  _marker_fg.set_translate(translate + glm::vec2(0, sqrt(2.0f)*indicator_border_size.y), indicator_fg_depth);
  _marker_fg.set_color(indicator_fg_color);
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
