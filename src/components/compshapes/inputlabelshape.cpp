#include <components/compshapes/inputlabelshape.h>
#include <components/resources/resources.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/linkshape.h>

#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <base/device/geometry/textlimits.h>

#include <components/entities/entityids.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace ngs {

const float InputLabelShape::default_text_angle = boost::math::constants::pi<float>()/4.0f;

InputLabelShape::InputLabelShape(Entity* entity)
    : CompShape(entity, kDID()),
      _factory(this),
      _resources(this),
      _input_shape(this),
      _link_shape(this),
      _state(0) {
  get_dep_loader()->register_fixed_dep(_factory, "");
  get_dep_loader()->register_fixed_dep(_resources, "");
  get_dep_loader()->register_fixed_dep(_input_shape, "..");
}

InputLabelShape::~InputLabelShape() {
}

bool InputLabelShape::update_deps() {
  start_method();
  std::vector<Entity*> dependants = _input_shape->get_dependants_by_did(kICompShape, kLinkShape);
  //assert(dependants.size() <= 1);

  // Note there should only be one link, but when dragging the head of the link onto
  // a connected input there will two links for a moment.

  if (dependants.size() > 0) {
    Dep<LinkShape> link_shape = get_dep<LinkShape>(dependants[dependants.size()-1]);
    if (link_shape == _link_shape) {
      // Return false if the dependencies haven't changed.
      return false;
    } else {
      _link_shape = link_shape;
      // Return true if the dependencies haven't changed.
      return true;
    }
  }

  // Otherwise.
  if (_link_shape) {
    _link_shape.reset();
    return true;
  }
  return false;
}

void InputLabelShape::update_state() {
  // Get the input shape's origin.
  const glm::vec2 &origin = _input_shape->get_origin();

  // Update our state.
  if (_input_shape->is_selected()) {
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

bool InputLabelShape::is_selected() const {
  if (_state & selected_transform_bitmask) {
    return true;
  }
  return false;
}

const std::vector<CharInstance> * InputLabelShape::get_char_instances() const {
  start_method();
  return &_chars;
}

}
