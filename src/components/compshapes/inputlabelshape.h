#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>

namespace ngs {

class BaseFactory;
class Resources;
class InputShape;
class LinkShape;

class COMPSHAPES_EXPORT InputLabelShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, InputLabelShape);

  static const float default_text_angle;

  InputLabelShape(Entity* entity);
  virtual ~InputLabelShape();

  // Our shape instances.
  virtual const std::vector<CharInstance> * get_char_instances() const;

 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();

 private:

  // Our fixed deps.
  Dep<Resources> _resources;
  Dep<InputShape> _input_shape;

  // Our dynamic deps.
  Dep<LinkShape> _link_shape; // This is not serialized but gathered.

  // Our char instances.
  std::vector<CharInstance> _chars;
  unsigned char _state;

  // Our text bounds.
  glm::vec2 _text_min;
  glm::vec2 _text_max;
};

}
