#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/nodeshape.h>

namespace ngs {

class BaseFactory;
class LowerHierarchyChange;
class GroupInteraction;

class COMPSHAPES_EXPORT GroupNodeShape: public NodeShape {
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
  virtual HierarchyUpdate update_hierarchy();

  // Input and Output Ordering.
  virtual size_t get_input_order(const std::string& input_name) const;
  virtual size_t get_output_order(const std::string& output_name) const;

 private:


  // Our fixed deps.
  Dep<BaseFactory> _factory;
  Dep<LowerHierarchyChange> _lower_change;

  // Our indicator is simply a square rotated 45 degrees.
  ShapeInstance* _bg_quad;
  ShapeInstance* _fg_quad;

  std::unordered_map<std::string, size_t> _input_order;
  std::unordered_map<std::string, size_t> _output_order;
};


}
