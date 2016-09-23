#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/rectnodeshape.h>

namespace ngs {

class BaseFactory;
class LowerHierarchyChange;
class GroupInteraction;

class COMPSHAPES_EXPORT GroupNodeShape: public RectNodeShape {
 public:

  COMPONENT_ID(CompShape, GroupNodeShape);

  static const float indicator_bg_depth;
  static const float indicator_fg_depth;
  static const float indicator_offset;
  static const float indicator_rotation;

  static const glm::vec2 indicator_size;
  static const glm::vec2 indicator_border_size;

  static const std::array<unsigned char,4> indicator_bg_color;
  static const std::array<unsigned char,4> indicator_fg_color;

  GroupNodeShape(Entity* entity);
  virtual ~GroupNodeShape();

  // Our state.
  virtual void update_state();

 private:

  // Our fixed deps.
  Dep<BaseFactory> _factory;
  Dep<LowerHierarchyChange> _lower_change;

  // Our indicator is simply a square rotated 45 degrees.
  ShapeInstance _marker_bg;
  ShapeInstance _marker_fg;

};


}
