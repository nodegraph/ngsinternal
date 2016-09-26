#include <components/compshapes/inputlabelshape.h>
#include <components/resources/resources.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/linkshape.h>

#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <base/device/geometry/textlimits.h>

#include <entities/entityids.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace ngs {

const float InputLabelShape::default_text_angle = boost::math::constants::pi<float>()/4.0f;

InputLabelShape::InputLabelShape(Entity* entity)
    : CompShape(entity, kDID()),
      _resources(this),
      _input_shape(this),
      _link_shape(this),
      _state(0) {
  get_dep_loader()->register_fixed_dep(_resources, "");
  get_dep_loader()->register_fixed_dep(_input_shape, "..");
}

InputLabelShape::~InputLabelShape() {
}

void InputLabelShape::gather_wires() {
  std::unordered_set<Entity*> dependants = _input_shape->get_dependants_by_did(kICompShape, kLinkShape);
  //assert(dependants.size() <= 1);

  // Note there should only be one link, but when dragging the head of the link onto
  // a connected input there will two links for a moment.
  if (dependants.size() > 0) {
    Entity* dependant = *(dependants.begin());
    // If the link shape component owner is the same, then the link shape likely the same.
    if (_link_shape.get() && _link_shape.get()->our_entity() != dependant) {
      _link_shape = get_dep<LinkShape>(dependant);
    }
  } else {
    _link_shape.reset();
  }
}

void InputLabelShape::update_state() {
  std::cerr << "InputLabelShape update state\n";

  // If the input shape is not exposed then clear out our shapes.
  if (!_input_shape->is_exposed()) {
    _chars.clear();
    return;
  }

  // Get the input shape's origin.
  const glm::vec2 &origin = _input_shape->get_origin();

  // Update our pannable state.
  set_pannable(_input_shape->is_pannable());

  // Update our state.
  if (_input_shape->is_pannable()) {
    _state |= selected_transform_bitmask;
  } else {
    _state &= ~selected_transform_bitmask;
  }

  // Get our name.
  const std::string& name = get_entity("..")->get_name();

  // Tessellate our text.
  if (!_link_shape) {
    // If we're not connected, we position the text at 45 degrees.
    _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(InputShape::plug_radius, 0),
                                                           default_text_angle, origin, _state, _chars, _text_min,
                                                           _text_max);
  } else {
    // Otherwise we display the text along the link.
    const Polygon &bound = _link_shape->get_bounds();
    float vector_angle = _link_shape->get_angle();
    glm::vec2 origin = bound.vertices[3];
    float pi2 = boost::math::constants::pi<float>()*0.5f;
    // Note the range of the angle goes from -pi to pi.
    if ((vector_angle <= -pi2) || (vector_angle >= pi2)) {
      float text_angle = vector_angle + boost::math::constants::pi<float>();
      _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(0, 0), text_angle, origin, _state, _chars,
                                                             _text_min, _text_max);
      // Adjust the text based on the text dimensions.
      glm::vec2 text_dim = _text_max - _text_min;
      for (CharInstance &c: _chars) {
        c.translate1[1] -= (text_dim.y);
      }
    } else {
      float text_angle = vector_angle;
      _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(0, 0), text_angle, origin, _state, _chars,
                                                             _text_min, _text_max);
      // Adjust the text based on the text dimensions.
      glm::vec2 text_dim = _text_max - _text_min;
      for (CharInstance &c: _chars) {
        c.translate1[0] -= text_dim.x;
      }
    }
  }
}

const std::vector<CharInstance> * InputLabelShape::get_char_instances() const {
  start_method();
  return &_chars;
}

}
