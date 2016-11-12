#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <components/compshapes/hitregion.h>
#include <base/device/geometry/instancevertexattribute.h>

namespace ngs {

class CompPolyBorder;
class SelectableShape;
class NodeShape;

// CompShapes are composites of other primitive shape instances.
// The shape instance include things like triangles, squares and letters.

class COMPSHAPES_EXPORT CompShape: public Component{
 public:

  COMPONENT_ID(CompShape, InvalidComponent);

  CompShape(Entity* entity, ComponentDID did);
  virtual ~CompShape();

  // Visibility.
  bool is_visible() {return _visible;}
  void set_visible(bool visible) {_visible = visible;}

  // Positioning.
  virtual void set_pos(const glm::vec2& anchor);
  virtual const glm::vec2& get_pos() const;

  // Pannable.
  virtual bool is_pannable() const;

  // Hit testing.
  virtual const CompPolyBorder& get_border() const;
  virtual HitRegion hit_test(const glm::vec2& point) const;

  // Our primitive instances.
  virtual const std::vector<ShapeInstance>* get_tri_instances() const;
  virtual const std::vector<ShapeInstance>* get_circle_instances() const;
  virtual const std::vector<ShapeInstance>* get_quad_instances() const;
  virtual const std::vector<CharInstance>* get_char_instances() const;

  // Our sub interfaces. Note all interfaces are still registered under the KICompShape IID.
  // To get the sub interface just do: get_dep<SelectableShape>(comp_shape->our_entity());
  virtual bool is_selectable() const {return false;}
  virtual bool is_linkable() const {return false;}

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 protected:
  virtual void set_pannable(bool pannable);

 private:
  bool _pannable;
  bool _visible;
};

}
