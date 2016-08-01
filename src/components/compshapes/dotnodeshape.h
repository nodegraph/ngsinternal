#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/linkableshape.h>
#include <base/utils/polygon.h>

namespace ngs {


class COMPSHAPES_EXPORT DotNodeShape: public LinkableShape {
 public:

  COMPONENT_ID(CompShape, DotNodeShape);

  // Layering.
  static const float bg_depth;
  static const float fg_depth;
  static const std::array<unsigned char, 4> bg_color;
  static const std::array<unsigned char, 4> fg_color;
  // Node Geometry.
  static const float border_width;
  static const float radius;

  DotNodeShape(Entity* entity);
  virtual ~DotNodeShape();

  // Our state.
  virtual void update_state();

  // Positioning.
  virtual void set_pos(const glm::vec2& anchor);
  virtual const glm::vec2& get_pos() const;

  // Hit testing.
  virtual const Polygon& get_bounds() const;

  // Our instances.
  virtual const std::vector<ShapeInstance> * get_circle_instances() const {return &_circles;}

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

  // Input and Output Ordering.
  virtual size_t get_input_order(const std::string& input_name) const;
  virtual size_t get_output_order(const std::string& output_name) const;

  // Num inputs and outputs.
  virtual size_t get_num_linkable_inputs() const {return 1;} // number of inputs with input plugs
  virtual size_t get_num_linkable_outputs() const {return 1;} // number of outputs with output plugs
  virtual size_t get_num_input_params() const {return 0;} // number of inputs without input plugs
  virtual size_t get_num_output_params() const {return 0;} // number of outputs without output plugs
  virtual size_t get_num_all_inputs() const {return 1;} // number of all inputs
  virtual size_t get_num_all_outputs() const {return 1;} // number of all outputs

 private:
  virtual void select(bool selected);
  virtual bool is_selected() const;

  // Our position.
  glm::vec2 _pos;

  // Our shape instances.
  std::vector<ShapeInstance> _circles;
  ShapeInstance* _bg_circle;
  ShapeInstance* _fg_circle;

  // Our bounds.
  Polygon _bounds;
};


}
