#pragma once
#include <components/compshapes/compshapes_export.h>
#include <entities/componentids.h>
#include <components/compshapes/hitregion.h>
#include <base/device/geometry/instancevertexattribute.h>
#include <base/device/geometry/objectvertexattribute.h>
#include <base/device/transforms/glmhelper.h>
#include <base/objectmodel/component.h>


#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace ngs {

class CompShape;
class NodeShape;
class CompPolyBorder;

class DotNode;

class COMPSHAPES_EXPORT CompShapeCollective: public Component {
 public:

  static void get_aa_bounds(const DepUSet<CompShape>& shapes, glm::vec2& min, glm::vec2& max);
  static void get_aa_bounds(const DepUSet<NodeShape>& shapes, glm::vec2& min, glm::vec2& max);

  COMPONENT_ID(CompShapeCollective, CompShapeCollective);

  CompShapeCollective(Entity* entity);
  virtual ~CompShapeCollective();

  // Shape Instances.
  const std::vector<ShapeInstance>& get_quads() const {external(); return _quads;}
  const std::vector<ShapeInstance>& get_tris() const {external(); return _tris;}
  const std::vector<ShapeInstance>& get_circles() const {external(); return _circles;}
  const std::vector<CharInstance>& get_chars() const {external(); return _chars;}

  // Hit Testing.
  virtual Entity* hit_test(const glm::vec2& point, HitRegion& type) const;

  // Axis Aligned Bounds.
  void get_aa_bounds(glm::vec2& min, glm::vec2& max) const; // bounds for all nodes and links in the group.

  Dep<CompShape> get_lowest(const std::unordered_set<Entity*>& ignore = std::unordered_set<Entity*>());

  //virtual void clean_dependencies(); // Multi threaded override.
 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();

 private:

  void add(const Dep<CompShape>& c);
  void remove(const Dep<CompShape>& c);

  void collect_comp_shapes();
  void collect_comp_shapes(Entity* entity);

  void collect_quad_instances(std::vector<ShapeInstance>& quads);
  void collect_tri_instances(std::vector<ShapeInstance>& quads);
  void collect_circle_instances(std::vector<ShapeInstance>& quads);
  void collect_char_instances(std::vector<CharInstance>& chars);

  // Our dynamic deps.
  // Our set of comp shapes is not serialized because the update_state can gather them up.
  DepUSet<CompShape> _comp_shapes;

  // Our shape instances.
  std::vector<ShapeInstance> _quads;
  std::vector<ShapeInstance> _tris;
  std::vector<ShapeInstance> _circles;
  std::vector<CharInstance> _chars;

};

}





