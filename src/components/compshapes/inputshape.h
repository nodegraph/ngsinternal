#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>
#include <base/utils/polygon.h>

namespace ngs {

class NodeShape;
class BaseInputs;

class COMPSHAPES_EXPORT InputShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, InputShape);

  // Layering.
  static const float bg_depth;
  static const float fg_depth;
  static const std::array<unsigned char,4> bg_color;
  static const std::array<unsigned char,4> fg_color;
  // Plug Geometry.
  static const glm::vec2 plug_size;
  static const glm::vec2 plug_border_size;
  static const float plug_radius;
  static const float plug_offset;

  InputShape(Entity* entity);
  virtual ~InputShape();

  bool is_exposed() const;
  virtual HitRegion hit_test(const glm::vec2& point) const;

  // Our bounds.
  virtual const Polygon& get_bounds() const;

  // Our shape instances.
  virtual const std::vector<ShapeInstance>* get_quad_instances() const;

  // The center of our shape. Used by dependants for alignment.
  const glm::vec2& get_origin() const;

  // Find our link. Note we don't have dependency on the link, as the link depends on us and it would create a dependency cycle.
  Entity* find_link_entity() const;

  const Dep<NodeShape>& get_node_shape() const {external(); return _node_shape;}

 protected:

  // Our state.
  virtual void update_state();

 private:

  // Our fixed deps.
  Dep<NodeShape> _node_shape;
  Dep<BaseInputs> _inputs;

  // Our quad instances.
  std::vector<ShapeInstance> _quads;

  // Our quad bounds.
  Polygon _bounds;
  glm::vec2 _origin; // The center of the input shape. Can be derived from _bg_bounds, but cached for speed.
};

}
