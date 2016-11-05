#include <components/compshapes/outputlabelshape.h>
#include <components/resources/resources.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/linkshape.h>

#include <entities/componentids.h>

#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/device/geometry/textlimits.h>

#include <cassert>
#include <boost/math/constants/constants.hpp>


namespace ngs {

const float OutputLabelShape::default_text_angle = -boost::math::constants::pi<float>() / 4.0f;

OutputLabelShape::OutputLabelShape(Entity* entity)
    : CompShape(entity, kDID()),
      _resources(this),
      _output_shape(this),
      _state(0) {
  get_dep_loader()->register_fixed_dep(_resources, Path({}));
  get_dep_loader()->register_fixed_dep(_output_shape, Path({".."}));
}

OutputLabelShape::~OutputLabelShape() {
}

void OutputLabelShape::update_wires() {
  internal();

  DepUSet<LinkShape> deps;
  std::unordered_set<Entity*> dependants = _output_shape->get_dependants_by_did(ComponentIID::kICompShape, ComponentDID::kLinkShape);

  // Update our wires.
  _link_shapes.clear();
  for (Entity* e: dependants) {
    Dep<LinkShape> link_shape = get_dep<LinkShape>(e);
    _link_shapes.insert(link_shape);
  }
}

bool OutputLabelShape::update_state() {
  internal();

  // If the output shape is not exposed then clear out our shapes.
  if (!_output_shape->is_exposed()) {
    _chars.clear();
    return true;
  }

  // Remove any link shapes that have been destroyed.
  DepUSet<LinkShape>::iterator iter = _link_shapes.begin();
  while(iter != _link_shapes.end()) {
    if (!*iter) {
      iter = _link_shapes.erase(iter);
    } else {
      ++iter;
    }
  }

  // Get the output shape's origin.
  const glm::vec2 &origin = _output_shape->get_origin();

  // Update our pannable state.
  set_pannable(_output_shape->is_pannable());

  // Update our state.
  if (_output_shape->is_pannable()) {
    _state |= selected_transform_bitmask;
  } else {
    _state &= ~selected_transform_bitmask;
  }

  // Get our name.
  const std::string& name = get_entity(Path({".."}))->get_name();

  // Find the link shape with the smallest angle.
  // This finds the left most link coming out of the output shape.
  float min = 9999.0;
  Dep<LinkShape> min_link(this);
  for (const Dep<LinkShape>& ls: _link_shapes) {
    if (ls->get_angle() < min) {
      min = ls->get_angle();
      min_link = ls;
    }
  }

  // Tessellate our text.
  if (!min_link) {
    // If we're not connected, we position the text at 45 degrees.
    _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(OutputShape::plug_radius, 0),
                                                           default_text_angle, origin, _state, _chars, _text_min,
                                                           _text_max);
  } else {
    // Otherwise we display the text along the link.
    const CompPolyBorder &bound = min_link->get_border();
    float vector_angle = min_link->get_angle();
    glm::vec2 origin = bound.poly_bound_map.at(HitRegion::kLinkTailRegion).vertices[2];
    float pi2 = boost::math::constants::pi<float>()*0.5f;
    // Note the range of the angle goes from -pi to pi.
    if ((vector_angle <= -pi2) || (vector_angle >= pi2)) {
      float text_angle = vector_angle + boost::math::constants::pi<float>();
      _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(0,0), text_angle,
                                                             origin, _state, _chars, _text_min, _text_max);

      // Adjust the text based on the text dimensions.
      glm::vec2 text_dim = _text_max - _text_min;
      for (CharInstance &c: _chars) {
        c.translate1[0] -= text_dim.x;
      }
    } else {
      float text_angle = vector_angle;
      _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(0, 0), text_angle, origin, _state, _chars,
                                                             _text_min, _text_max);
      // Adjust the text based on the text dimensions.
      glm::vec2 text_dim = _text_max - _text_min;
      for (CharInstance &c: _chars) {
        c.translate1[1] -= text_dim.y;
      }
    }
  }
  return true;
}

const std::vector<CharInstance> * OutputLabelShape::get_char_instances() const {
  external();
  return &_chars;
}

}

