#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>
#include <base/utils/polygon.h>

namespace ngs {

class NodeShape;
class OutputCompute;

class COMPSHAPES_EXPORT OutputShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, OutputShape);

  // Layering.
  static const float bg_depth;
  static const float fg_depth;
  static const std::array<unsigned char,4> bg_color;
  static const std::array<unsigned char,4> fg_color;
  // Plug Geometry.
  static const glm::vec2 plug_size;
  static const float plug_border_width;
  static const float plug_radius;
  static const float plug_offset;

  OutputShape(Entity* entity);
  virtual ~OutputShape();

  // Our state.
  virtual void update_state();

  // Our bounds.
  virtual const Polygon& get_bounds() const;

  // Our shape instances.
  virtual const std::vector<ShapeInstance>* get_tri_instances() const;

  // The center of our shape. Used by dependants for alignment.
  const glm::vec2& get_origin() const;

  const Dep<NodeShape>& get_node_shape() {return _node_shape;}

 private:

  // Our fixed deps.
  Dep<NodeShape> _node_shape;
  Dep<OutputCompute> _output_compute;

  // Our tri instances.
  std::vector<ShapeInstance> _tris;
  ShapeInstance* _bg_tri;
  ShapeInstance* _fg_tri;

  // Our bounds.
  Polygon _bounds;
  glm::vec2 _origin; // The center of the input shape. Can be derived from _bg_bounds, but cached for speed.

};


}
