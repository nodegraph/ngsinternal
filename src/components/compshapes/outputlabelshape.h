#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>

namespace ngs {

class Resources;
class OutputShape;
class LinkShape;

class COMPSHAPES_EXPORT OutputLabelShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, OutputLabelShape);

  static const float default_text_angle;

  OutputLabelShape(Entity* entity);
  virtual ~OutputLabelShape();

  // Our state.
  virtual void update_wires();
  virtual void update_state();

  // Our shape instances.
  virtual const std::vector<CharInstance> * get_char_instances() const;

 private:

  // Our fixed deps.
  Dep<Resources> _resources;
  Dep<OutputShape> _output_shape;

  // Our dynamic deps. These are not serialized, but gathered.
  DepUSet<LinkShape> _link_shapes;

  // Our char instances.
  std::vector<CharInstance> _chars;
  unsigned char _state;

  // Our text bounds.
  glm::vec2 _text_min;
  glm::vec2 _text_max;

};


}
