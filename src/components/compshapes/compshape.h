#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/objectmodel/component.h>
#include <components/entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <components/compshapes/hittypes.h>
#include <base/device/geometry/instancevertexattribute.h>

namespace ngs {

class Polygon;
class SelectableShape;
class LinkableShape;

// CompShapes are composites of other primitive shape instances.
// The shape instance include things like triangles, squares and letters.

class COMPSHAPES_EXPORT CompShape: public Component{
 public:

  COMPONENT_ID(CompShape, InvalidComponent);

  CompShape(Entity* entity, size_t did);
  virtual ~CompShape();

  // Positioning.
  virtual void set_pos(const glm::vec2& anchor);
  virtual const glm::vec2& get_pos() const;

  // Pannable.
  virtual bool is_pannable();

  // Hit testing.
  virtual const Polygon& get_bounds() const;
  virtual HitRegion hit_test(const glm::vec2& point) const;
  virtual bool simple_hit_test(const glm::vec2& point) const;

  // Our primitive instances.
  virtual const std::vector<ShapeInstance>* get_tri_instances() const;
  virtual const std::vector<ShapeInstance>* get_circle_instances() const;
  virtual const std::vector<ShapeInstance>* get_quad_instances() const;
  virtual const std::vector<CharInstance>* get_char_instances() const;

  // Our sub interfaces. Note all interfaces are still registered under the KICompShape IID.
  // To get the sub interface just do: get_dep<SelectableShape>(comp_shape->our_entity());
  virtual bool is_selectable() {return false;}
  virtual bool is_linkable(){return false;}

 protected:
  virtual void set_pannable(bool pannable);

 private:
  bool _pannable;
};

}
