#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/nodeshape.h>

namespace ngs {

class COMPSHAPES_EXPORT OutputNodeShape: public NodeShape {
 public:

  COMPONENT_ID(CompShape, OutputNodeShape);

  static const float indicator_bg_depth;
  static const float indicator_fg_depth;
  static const float indicator_offset;

  static const glm::vec2 indicator_size;
  static const glm::vec2 indicator_border_size;

  static const std::array<unsigned char,4> indicator_bg_color;
  static const std::array<unsigned char,4> indicator_fg_color;

  OutputNodeShape(Entity* entity);
  virtual ~OutputNodeShape();

  // Our state.
  virtual void update_state();

 private:

  // Our indicator is simply an extra quad to the left of the node.
  ShapeInstance* _bg_quad;
  ShapeInstance* _fg_quad;
};


}
