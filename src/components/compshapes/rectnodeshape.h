#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/utils/polygon.h>
#include <components/compshapes/nodeshape.h>


// Shapes.
//#include <base/shapes/atomic/text.h>
//#include <base/shapes/atomic/rectangle.h>
//#include <base/shapes/atomic/arrow.h>

namespace ngs {

class Resources;
class NodeSelection;
class Compute;

class COMPSHAPES_EXPORT RectNodeShape: public NodeShape {
 public:

  COMPONENT_ID(CompShape, RectNodeShape);

  // Layering.
  static const float node_bg_depth;
  static const float node_fg_depth;
  static const float button_bg_depth;
  static const float button_fg_depth;
  static const std::array<unsigned char, 4> node_bg_color;
  static const std::array<unsigned char, 4> node_fg_color;


  // Node Geometry.
  static const glm::vec2 node_border_size;

  RectNodeShape(Entity* entity);
  RectNodeShape(Entity* entity, size_t did);
  virtual ~RectNodeShape();

  // Positioning.
  virtual void set_pos(const glm::vec2& pos);
  virtual const glm::vec2& get_pos() const;

  // Hit testing.
  virtual const Polygon& get_bounds() const;
  //virtual HitRegion hit_test(const glm::vec2& point) const;

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

  // Selection.
  virtual void select(bool selected);

 protected:
  // Our state.
  virtual void update_state();

 private:
  void update_node_quads();
  void update_quads_cache();

  void update_text();
  void update_chars_cache();

  // Our position.
  glm::vec2 _pos;

  // Our main color.
  std::array<unsigned char, 4> _color;

  // Our quad bounds.
  Polygon _bg_bounds;

  // Our text bounds.
  glm::vec2 _text_min;
  glm::vec2 _text_max;

  // Our char instances.
  std::vector<CharInstance> _node_name_chars;

  // Our quad instances.
  ShapeInstance _node_quad_bg;
  ShapeInstance _node_quad_fg;

};


}
