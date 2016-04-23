#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>
#include <base/utils/polygon.h>


// Shapes.
//#include <base/shapes/atomic/text.h>
//#include <base/shapes/atomic/rectangle.h>
//#include <base/shapes/atomic/arrow.h>

namespace ngs {

class Resources;
class NodeGraphSelection;
class Compute;

class COMPSHAPES_EXPORT NodeShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, NodeShape);

  // Layering.
  static const float node_bg_depth;
  static const float node_fg_depth;
  static const float button_bg_depth;
  static const float button_fg_depth;
  static const std::array<unsigned char, 4> node_bg_color;
  static const std::array<unsigned char, 4> node_fg_color;
  static const std::array<unsigned char, 4> right_bg_color;
  static const std::array<unsigned char, 4> right_fg_on_color;
  static const std::array<unsigned char, 4> right_fg_off_color;
  static const std::array<unsigned char, 4> left_bg_color;
  static const std::array<unsigned char, 4> left_fg_on_color;
  static const std::array<unsigned char, 4> left_fg_off_color;
  // Node Geometry.
  static const glm::vec2 node_border_size;
  static const glm::vec2 button_border_size;
  static const glm::vec2 button_offset;

  NodeShape(Entity* entity);
  NodeShape(Entity* entity, size_t did, size_t num_extra_quads);
  virtual ~NodeShape();

  // Our state.
  virtual void update_state();

  // Positioning.
  virtual void set_pos(const glm::vec2& pos);
  virtual const glm::vec2& get_pos() const;

  // Hit testing.
  virtual const Polygon& get_bounds() const;
  virtual HitRegion hit_test(const glm::vec2& point) const;

  // Our shape instancing.
  virtual const std::vector<ShapeInstance>* get_quad_instances() const {return &_quads;}
  virtual const std::vector<CharInstance> * get_char_instances() const {return &_chars;}

  // Input and Output Ordering.
  virtual size_t get_input_order(const std::string& input_name) const;
  virtual size_t get_output_order(const std::string& output_name) const;

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 protected:
  std::vector<ShapeInstance> _quads;
  virtual void select(bool selected);
  virtual bool is_selected() const;
  virtual void edit(bool on);
  virtual bool is_being_edited() const;
  virtual void view(bool on);
  virtual bool is_being_viewed() const;

 private:
  void init(size_t num_quads);

  void update_text();

  // Our fixed deps.
  Dep<Resources> _resources;
  Dep<Compute> _node_compute;

  // Our position.
  glm::vec2 _pos;

  // Our main color.
  std::array<unsigned char, 4> _color;

  // Ard edit and view state.
  bool _being_edited;
  bool _being_viewed;

  // Our quad instances.
  ShapeInstance* _bg_quad;
  ShapeInstance* _fg_quad;
  ShapeInstance* _left_bg_quad;
  ShapeInstance* _left_fg_quad;
  ShapeInstance* _right_bg_quad;
  ShapeInstance* _right_fg_quad;

  // Our quad bounds.
  Polygon _bg_bounds;
  Polygon _left_bounds;
  Polygon _right_bounds;

  // Our text bounds.
  glm::vec2 _text_min;
  glm::vec2 _text_max;

  // Our text vertices. These are vertices of the text quads.
  //std::vector<PosTexVertex> _text_vertices;
  std::vector<CharInstance> _chars;

};


}
