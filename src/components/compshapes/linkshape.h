#pragma once
#include <base/utils/polyborder.h>
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/selectableshape.h>

namespace ngs {

class InputCompute;
class InputShape;
class OutputShape;

class COMPSHAPES_EXPORT LinkShape: public SelectableShape {
 public:

  COMPONENT_ID(CompShape, LinkShape);

  // Layering.
  static const float bg_depth;
  static const float fg_depth;
  static const std::array<unsigned char,4> bg_color;
  static const std::array<unsigned char,4> fg_color;
  // Geometry.
  static const glm::vec2 tri_size;
  static const float body_height;
  static const float text_head_offset;
  static const float text_tail_offset;
  static const float border_width;

  LinkShape(Entity* entity);
  virtual ~LinkShape();

  virtual void start_moving();
  virtual void finished_moving();

  // In interactive mode we can set the input shape.
  virtual void link_input_shape(const Dep<InputShape>& input_shape);
  virtual void unlink_input_shape();
  virtual const Dep<InputShape>& get_input_shape() const;

  // In interactive mode we can also set the output shape.
  virtual void link_output_shape(const Dep<OutputShape>& output_shape);
  virtual void unlink_output_shape();
  virtual const Dep<OutputShape>& get_output_shape() const;

  // Our bounds.
  virtual const CompPolyBorder& get_border() const;
  HitRegion hit_test(const glm::vec2& point) const;

  // Our head position when not linked to an input shape.
  virtual void set_head_pos(const glm::vec2& pos);
  virtual const glm::vec2& get_head_pos() const;

  // Our tail position when not linked to an output shape.
  virtual void set_tail_pos(const glm::vec2& pos);
  virtual const glm::vec2& get_tail_position() const;

  // Our selected state.
  virtual void select(bool selected);

  // Returns the angle formed with our tail at the origin.
  // The return value is in the range from -pi to +pi.
  virtual float get_angle() const;
  virtual float get_full_length() const;
  virtual float get_body_length() const;
  virtual const glm::vec2& get_dir() const;
  virtual const glm::vec2& get_perp() const;

  // Our shape instances.
  virtual const std::vector<ShapeInstance>* get_tri_instances() const;
  virtual const std::vector<ShapeInstance>* get_quad_instances() const;

 protected:
  // Our state.
  virtual bool update_state();
  virtual bool should_destroy();

 private:
  virtual void update_positioning_helper(const glm::vec2& head_pos, const glm::vec2& tail_pos);
  virtual void update_state_helper();


  // Our dynamic deps.
  Dep<InputShape> _input_shape;
  Dep<OutputShape> _output_shape;

  // Our shape instances.
  std::vector<ShapeInstance> _quads;
  ShapeInstance _bg_quad;
  ShapeInstance _fg_quad;
  std::vector<ShapeInstance> _tris;
  ShapeInstance _bg_tri;
  ShapeInstance _fg_tri;

  // Our quad bounds.
  CompPolyBorder _border;

  // Our head and tail positions when we're not connected to an input or an output.
  glm::vec2 _head_pos;
  glm::vec2 _tail_pos;

  // Our text bounds.
  glm::vec2 _text_min;
  glm::vec2 _text_max;

  // In interactive mode, the update_hierarchy method won't destroy itself if either the input or output shape is not set.
  bool _interactive;

  // Cached orientation info.
  float _angle;
  float _full_length;
  float _body_length;
  glm::vec2 _dir;
  glm::vec2 _perp;
};

}
