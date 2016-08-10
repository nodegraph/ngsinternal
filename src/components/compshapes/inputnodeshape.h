#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/nodeshape.h>

namespace ngs {

class COMPSHAPES_EXPORT InputNodeShape: public NodeShape {
 public:

  COMPONENT_ID(CompShape, InputNodeShape);

  static const float indicator_bg_depth;
  static const float indicator_fg_depth;
  static const float indicator_border_width;
  static const float indicator_offset;

  static const glm::vec2 indicator_size;

  static const std::array<unsigned char,4> indicator_bg_color;
  static const std::array<unsigned char,4> indicator_fg_color;

  InputNodeShape(Entity* entity);
  virtual ~InputNodeShape();

  // Our state.
  virtual void update_state();

 private:

  // Our indicator is simply a triangle to the left of the node.
  ShapeInstance _marker_bg;
  ShapeInstance _marker_fg;
};


}
