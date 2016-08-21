#include "groupnodeshape.h"
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/lowerhierarchychange.h>
#include <base/objectmodel/basefactory.h>
#include <entities/entityids.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>

#include <algorithm>
#include <boost/math/constants/constants.hpp>

namespace ngs {

const float GroupNodeShape::indicator_bg_depth = 1.0f;
const float GroupNodeShape::indicator_fg_depth = 2.0f;
const float GroupNodeShape::indicator_offset = 140.0f;
const float GroupNodeShape::indicator_rotation = boost::math::constants::pi<float>()/4.0f;

const glm::vec2 GroupNodeShape::indicator_size(130.0f, 130.0f);
const glm::vec2 GroupNodeShape::indicator_border_size(10,10);

const std::array<unsigned char,4> GroupNodeShape::indicator_bg_color = { 255, 255, 255, 255 };
const std::array<unsigned char,4> GroupNodeShape::indicator_fg_color = { 255, 128, 171, 255 };

struct {
    bool operator()(const Dep<CompShape>& a, const Dep<CompShape>& b)
    {
        return a->get_pos().x < b->get_pos().x;
    }
} CompShapeCompare;

GroupNodeShape::GroupNodeShape(Entity* entity)
    : RectNodeShape(entity, kDID()),
      _factory(this),
      _lower_change(this) {
  get_dep_loader()->register_fixed_dep(_factory, "");
  get_dep_loader()->register_fixed_dep(_lower_change, ".");

  _marker_bg.state = 0;
  _marker_fg.state = 0;
}

GroupNodeShape::~GroupNodeShape() {
}

void GroupNodeShape::update_state() {
  RectNodeShape::update_state();

  const Polygon& bounds = get_bounds();
  glm::vec2 target_center = 0.5f * (bounds.vertices[0] + bounds.vertices[3]) - glm::vec2(indicator_offset, 0);
  glm::vec2 translate = target_center - glm::vec2(0, sqrt(2.0f)*indicator_size.x/2.0f);

  // Update our bg quad.
  _marker_bg.set_scale(indicator_size);
  _marker_bg.set_rotate(indicator_rotation);
  _marker_bg.set_translate(translate, indicator_bg_depth);
  _marker_bg.set_color(indicator_bg_color);
  if (is_selected()) {
    _marker_bg.state |= selected_transform_bitmask;
  } else {
    _marker_bg.state &= ~selected_transform_bitmask;
  }

  // Update our fg quad.
  _marker_fg.set_scale(indicator_size -  2.0f*indicator_border_size);
  _marker_fg.set_rotate(indicator_rotation);
  _marker_fg.set_translate(translate + glm::vec2(0, sqrt(2.0f)*indicator_border_size.y), indicator_fg_depth);
  _marker_fg.set_color(indicator_fg_color);
  if (is_selected()) {
    _marker_fg.state |= selected_transform_bitmask;
  } else {
    _marker_fg.state &= ~selected_transform_bitmask;
  }

  // Append our marker shapes onto the quads_cache.
  _quads_cache.insert(_quads_cache.end(), _marker_bg);
  _quads_cache.insert(_quads_cache.end(), _marker_fg);
}

HierarchyUpdate GroupNodeShape::update_hierarchy() {
  bool hierarchy_changed = false;

  // Make sure the inputs and outputs on this group match up
  // with the input and output nodes inside this group.
  Entity* inputs = get_entity("./inputs");
  Entity* outputs = get_entity("./outputs");
  BaseEntityInstancer* ei = _factory->get_entity_instancer();

  // Make sure all the input/outputs nodes in this group are represented by inputs/outputs.
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;
    size_t did = child->get_did();
    if (did == kInputNodeEntity) {
      if (inputs->has_child_name(child_name)) {
        continue;
      }
      Entity* in = ei->instance(inputs, child_name, kInputEntity);
      in->create_internals();
      in->initialize_deps();
      hierarchy_changed = true;
    } else if (did == kOutputNodeEntity) {
      if (outputs->has_child_name(child_name)) {
        continue;
      }
      Entity* out = ei->instance(outputs, child_name, kOutputEntity);
      out->create_internals();
      out->initialize_deps();
      hierarchy_changed = true;
    }
  }

   // Now remove any inputs/outputs that no longer have associated nodes in this group.
  {
    std::vector<Entity*> _inputs_to_destroy;
    for (auto &iter : inputs->get_children()) {
      const std::string& child_name = iter.first;
      if (!our_entity()->has_child_name(child_name)) {
        _inputs_to_destroy.push_back(iter.second);
      }
    }
    std::vector<Entity*> _outputs_to_destroy;
    for (auto &iter : outputs->get_children()) {
      const std::string& child_name = iter.first;
      if (!our_entity()->has_child_name(child_name)) {
        _outputs_to_destroy.push_back(iter.second);
      }
    }
    for (Entity* e : _inputs_to_destroy) {
      // This can leave dangling link shapes, which are cleaned up by subsequest passes of update_hierarchy().
      delete_ff(e);
      hierarchy_changed = true;
    }
    for (Entity* e : _outputs_to_destroy) {
      // This can leave dangling link shapes, which are cleaned up by subsequest passes of update_hierarchy().
      delete_ff(e);
      hierarchy_changed = true;
    }
  }

  // Now order the inputs.
  {
    std::vector<Dep<CompShape> > input_node_shapes;
    for (auto &iter : inputs->get_children()) {
      input_node_shapes.push_back(get_dep<CompShape>(our_entity()->get_child(iter.first)));
    }
    std::sort(input_node_shapes.begin(), input_node_shapes.end(), CompShapeCompare);

    _input_order.clear();
    for (size_t i = 0; i < input_node_shapes.size(); ++i) {
      _input_order[input_node_shapes[i]->get_name()] = i;
    }
  }

  // Now order the outputs.
  {
    std::vector<Dep<CompShape> > output_node_shapes;
    for (auto &iter : outputs->get_children()) {
      output_node_shapes.push_back(get_dep<CompShape>(our_entity()->get_child(iter.first)));
    }
    std::sort(output_node_shapes.begin(), output_node_shapes.end(), CompShapeCompare);

    _output_order.clear();
    for (size_t i = 0; i < output_node_shapes.size(); ++i) {
      _output_order[output_node_shapes[i]->get_name()] = i;
    }
  }

  if (hierarchy_changed) {
    return kChanged;
  }
  return kUnchanged;

}

size_t GroupNodeShape::get_input_order(const std::string& input_name) const {
  if (_input_order.count(input_name)) {
    return _input_order.at(input_name);
  }
  return 0;
}

size_t GroupNodeShape::get_output_order(const std::string& output_name) const {
  if (_output_order.count(output_name)) {
    return _output_order.at(output_name);
  }
  return 0;
}

}
