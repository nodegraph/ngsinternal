#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/objectmodel/component.h>
#include <components/entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <components/compshapes/hittypes.h>
#include <base/device/geometry/instancevertexattribute.h>

namespace ngs {

class Polygon;

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

  // Hit testing.
  virtual const Polygon& get_bounds() const;
  virtual HitRegion hit_test(const glm::vec2& point) const;
  virtual bool simple_hit_test(const glm::vec2& point) const;

  // Our primitive instances.
  virtual const std::vector<ShapeInstance>* get_tri_instances() const;
  virtual const std::vector<ShapeInstance>* get_circle_instances() const;
  virtual const std::vector<ShapeInstance>* get_quad_instances() const;
  virtual const std::vector<CharInstance>* get_char_instances() const;

  // Our selected state.
  virtual bool is_selected() const {return false;}

  // Input and Output Ordering.
  virtual size_t get_input_order(const std::string& input_name) const {return 0;}
  virtual size_t get_output_order(const std::string& output_name) const {return 0;}

 private:
  // The following methods should only be called by the NodeGraphSelection component.
  friend class NodeGraphSelection;


  // Selected State.
  virtual void select(bool selected) {}

  // Edit State.
  virtual void edit(bool on) {}
  virtual bool is_being_edited() const {return false;}

  // View State.
  virtual void view(bool on) {}
  virtual bool is_being_viewed() const {return false;}



};

}
