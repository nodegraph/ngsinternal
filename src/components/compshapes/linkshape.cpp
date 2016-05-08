#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>


#include <base/objectmodel/deploader.h>

// Boost.
#include <boost/math/constants/constants.hpp>


namespace ngs {

const float LinkShape::bg_depth = 3;
const float LinkShape::fg_depth = 4;
const std::array<unsigned char,4> LinkShape::bg_color = { 64, 64, 64, 255 };
const std::array<unsigned char,4> LinkShape::fg_color = { 100, 100, 100, 255 };

const glm::vec2 LinkShape::tri_size(100.0f, 80.0f);
const float LinkShape::body_height = 40.0f;
const float LinkShape::text_head_offset = 5.0f;
const float LinkShape::text_tail_offset = 5.0f;
const float LinkShape::border_width = 10;

LinkShape::LinkShape(Entity* entity)
    : CompShape(entity, kDID()),
      _input_compute(this),
      _input_shape(this),
      _output_shape(this),
      _interactive(false),
      _angle(0),
      _full_length(0),
      _body_length(0){

  _quads.resize(2);
  _bg_quad = &_quads[0];
  _fg_quad = &_quads[1];
  _tris.resize(2);
  _bg_tri = &_tris[0];
  _fg_tri = &_tris[1];
}

LinkShape::~LinkShape() {
}

bool LinkShape::update_deps() {
  start_method();

  // We can really only update our deps if we have an input compute,
  // as that tells us what the output compute is as well as the input and output shapes.
  if (_input_compute) {
    // Get the input shape.
    Dep<InputShape> input_shape(this);
    input_shape = get_dep<InputShape>(_input_compute->our_entity());

    // Get the output shape.
    Dep<OutputCompute> output_compute = _input_compute->get_output_compute();
    // This is always non-null as we only connect to the input compute if it's connected.
	//assert(output_compute);
	if (!output_compute) {
		return false;
	}
    
    Dep<OutputShape> output_shape(this);
    output_shape = get_dep<OutputShape>(output_compute->our_entity());

    // Return false if the dependencies haven't changed.
    if ((_input_shape == input_shape ) && (_output_shape == output_shape)) {
      return false;
    }

    // Otherwise return true.
    _input_shape = input_shape;
    _output_shape = output_shape;
    return true;
  }

  // Otherwise we're unable to update our deps.
  return false;
}

HierarchyUpdate LinkShape::update_hierarchy() {
  if (_interactive) {
    return kUnchanged;
  }
  // If the input compute has been set but the input and output shapes are null,
  // then we are a dangling link which needs to be destroyed.
  if (_input_compute) {
    update_deps();
    if ((!_input_shape) || (!_output_shape)) {
      return kRequestDestruction;
    } else {
      return kUnchanged;
    }
  }
  return kRequestDestruction;
}

void LinkShape::update_state() {
  if (_input_shape) {
    _head_pos = _input_shape->get_origin();
  }
  if (_output_shape) {
    _tail_pos = _output_shape->get_origin();
  }
  update_positioning_helper(_head_pos, _tail_pos);
  update_state_helper();
}

// Note it may seem that whenever _input_compute is null we are in
// in interactive mode. However this is not case.
// The input compute may get destroyed, and we don't wan't to be considered interactive.
// Instead we want the update hierarchy to destroy us.
void LinkShape::make_interactive() {
  unlink_input_compute();
  _interactive = true;
}

void LinkShape::link_input_compute(const Dep<InputCompute>& input_compute) {
  _input_compute = input_compute;
  _interactive = false;
}

void LinkShape::unlink_input_compute() {
  _input_compute.reset();
}

const Dep<InputCompute>& LinkShape::get_input_compute() const {
  return _input_compute;
}

// In interactive mode we can set the input shape.
void LinkShape::link_input_shape(const Dep<InputShape>& input_shape) {
  _input_shape = input_shape;
}
void LinkShape::unlink_input_shape() {
  _input_shape.reset();
}
const Dep<InputShape>& LinkShape::get_input_shape() const {
  return _input_shape;
}

// In interactive mode we can also set the output shape.
void LinkShape::link_output_shape(const Dep<OutputShape>& output_shape) {
  _output_shape = output_shape;
}
void LinkShape::unlink_output_shape() {
  _output_shape.reset();
}
const Dep<OutputShape>& LinkShape::get_output_shape() const {
  return _output_shape;
}

void LinkShape::update_positioning_helper(const glm::vec2& head_pos, const glm::vec2& tail_pos) {
  start_method();
  const glm::vec2 diff = head_pos - tail_pos;
  _dir = glm::normalize(diff);
  _perp = glm::vec2(-_dir.y, _dir.x);  // equivalent to dir rotated 90 degrees using right hand rule

  // Nudge the tips of the head shape off of the input and output shapes.
  const glm::vec2 front = head_pos - InputShape::plug_radius * _dir;
  const glm::vec2 back = tail_pos + OutputShape::plug_radius * _dir;
  _full_length = glm::length(front-back);
  _body_length = _full_length - tri_size.y;
  _angle = atan2(_dir.y, _dir.x);

  // Update our bounds.
  std::vector<glm::vec2>& verts = _bounds.vertices;
  verts.resize(7);
  verts[0] = front - tri_size.y * _dir - tri_size.x/2.0f * _perp;
  verts[1] = front;
  verts[2] = verts[0]+ tri_size.x * _perp;
  verts[3] = verts[2] - (tri_size.x - body_height)/2.0f * _perp;
  verts[4] = verts[3] - _body_length * _dir;
  verts[5] = verts[4] - body_height * _perp;
  verts[6] = verts[5] + _body_length * _dir;

  // Update our head and tail bounds.
  {
    std::vector<glm::vec2>& hb = _head_bounds.vertices;
    hb.resize(7);
    hb[0] = verts[0];
    hb[1] = verts[1];
    hb[2] = verts[2];
    hb[3] = verts[3];
    hb[4] = hb[3] - 0.5f * _body_length * _dir;
    hb[5] = hb[4] - body_height * _perp;
    hb[6] = hb[5] + 0.5f * _body_length * _dir;
    std::vector<glm::vec2>& tb = _tail_bounds.vertices;
    tb.resize(4);
    tb[0] = hb [4];
    tb[1] = tb [0] - 0.5f* _body_length * _dir;
    tb[2] = tb [1] - body_height * _perp;
    tb[3] = hb [2] + 0.5f * _body_length * _dir;
  }

  // Update our triangle.
  _bg_tri->set_scale(tri_size);
  _bg_tri->set_rotate(_angle + boost::math::constants::pi<float>() - boost::math::constants::pi<float>()/2.0f);
  _bg_tri->set_translate(verts[1], bg_depth);
  _bg_tri->set_color(bg_color);

  // h' = h - 3*border_width
  // w' = w - 2*sqrt(3)*border_width
  _fg_tri->set_scale(tri_size - glm::vec2(2 * sqrt(3.0f) * border_width, 3.0f * border_width));
  _fg_tri->set_rotate(_bg_tri->rotate);
  _fg_tri->set_translate(verts[1] - 2.0f*border_width*_dir, fg_depth);
  _fg_tri->set_color(fg_color);

  // Update our body.
  _bg_quad->set_scale(_body_length, body_height);
  _bg_quad->set_rotate(_angle + boost::math::constants::pi<float>());
  _bg_quad->set_translate(verts[3], bg_depth);
  _bg_quad->set_color(bg_color);

  _fg_quad->set_scale(_body_length, body_height - 2.0f * border_width);
  _fg_quad->set_rotate(_bg_quad->rotate);
  _fg_quad->set_translate(verts[3] + border_width * _dir - border_width * _perp, fg_depth);
  _fg_quad->set_color(fg_color);

}

void LinkShape::update_state_helper() {
  if (_interactive) {
    return;
  }
  if ((!_input_shape) || (!_output_shape)) {
    return;
  }
  if (_input_shape->is_selected() && _output_shape->is_selected()) {
    _bg_quad->state |= (selected_transform_bitmask);
    _fg_quad->state |= selected_transform_bitmask;
    _bg_tri->state |= (selected_transform_bitmask);
    _fg_tri->state |= selected_transform_bitmask;
  } else {
    _bg_quad->state &= ~(selected_transform_bitmask);
    _fg_quad->state &= ~selected_transform_bitmask;
    _bg_tri->state &= ~(selected_transform_bitmask);
    _fg_tri->state &= ~selected_transform_bitmask;
  }
}

const Polygon& LinkShape::get_bounds() const {
  start_method();
  return _bounds;
}

const std::vector<ShapeInstance>* LinkShape::get_tri_instances() const {
  start_method();
  return &_tris;
}

const std::vector<ShapeInstance>* LinkShape::get_quad_instances() const {
  start_method();
  return &_quads;
}

HitRegion LinkShape::hit_test(const glm::vec2& point) const {
  start_method();
  if (!simple_hit_test(point)) {
    return kMissed;
  }
  if (_head_bounds.contains(point)) {
    return kLinkHead;
  }
  if (_tail_bounds.contains(point)) {
    return kLinkTail;
  }
  return kShape;
}

void LinkShape::select(bool selected) {
  start_method();
  if (selected) {
    _bg_quad->state |= (selected_transform_bitmask|selected_color_bitmask);
    _fg_quad->state |= selected_transform_bitmask;
    _bg_tri->state |= (selected_transform_bitmask|selected_color_bitmask);
    _fg_tri->state |= selected_transform_bitmask;
  } else {
    _bg_quad->state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _fg_quad->state &= ~selected_transform_bitmask;
    _bg_tri->state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _fg_tri->state &= ~selected_transform_bitmask;
  }
}

bool LinkShape::is_selected() const {
  start_method();
  if (_bg_quad->state & selected_transform_bitmask) {
    return true;
  }
  return false;
}

void LinkShape::set_head_pos(const glm::vec2& pos) {
  start_method();
  _head_pos = pos;
}

const glm::vec2& LinkShape::get_head_pos() const {
  start_method();
  return _head_pos;
}

void LinkShape::set_tail_pos(const glm::vec2& pos) {
  start_method();
  _tail_pos = pos;
}

const glm::vec2& LinkShape::get_tail_position() const {
  start_method();
  return _tail_pos;
}

float LinkShape::get_angle() const {
  return _angle;
}

float LinkShape::get_full_length() const {
  return _full_length;
}

float LinkShape::get_body_length() const {
  return _body_length;
}

const glm::vec2& LinkShape::get_dir() const {
  return _dir;
}
const glm::vec2& LinkShape::get_perp() const {
  return _perp;
}

}
