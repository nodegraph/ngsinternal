#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>
#include <base/utils/polygon.h>

namespace ngs {


class COMPSHAPES_EXPORT DotNodeShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, DotNodeShape);

  // Layering.
  static const float bg_depth;
  static const float fg_depth;
  static const std::array<unsigned char, 4> bg_color;
  static const std::array<unsigned char, 4> fg_color;
  // Node Geometry.
  static const float border_width;
  static const float radius;

  DotNodeShape(Entity* entity);
  virtual ~DotNodeShape();

  // Our state.
  virtual void update_state();

  // Positioning.
  virtual void set_pos(const glm::vec2& anchor);
  virtual const glm::vec2& get_pos() const;

  // Hit testing.
  virtual const Polygon& get_bounds() const;

  // Our instances.
  virtual const std::vector<ShapeInstance> * get_circle_instances() const {return &_circles;}

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 private:
  virtual void select(bool selected);
  virtual bool is_selected() const;

  // Our position.
  glm::vec2 _pos;

  // Our shape instances.
  std::vector<ShapeInstance> _circles;
  ShapeInstance* _bg_circle;
  ShapeInstance* _fg_circle;

  // Our bounds.
  Polygon _bounds;
};


}
