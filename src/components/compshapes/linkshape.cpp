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
    : SelectableShape(entity, kDID()),
      _input_shape(this),
      _output_shape(this),
      _interactive(false),
      _angle(0),
      _full_length(0),
      _body_length(0) {

  get_dep_loader()->register_dynamic_dep(_input_shape);
  get_dep_loader()->register_dynamic_dep(_output_shape);

  _bg_quad.state = 0;
  _fg_quad.state = 0;
  _bg_tri.state = 0;
  _fg_tri.state = 0;
}

LinkShape::~LinkShape() {
}

bool LinkShape::should_destroy() {
  internal();
  // When interactively dragging the link, we don't allow hierarchy updates.
  if (_interactive) {
    return false;
  }

  // If either the input or output shape is not set then this is a dangling link that needs to be destroyed.
  if ((!_input_shape) || (!_output_shape)) {
    return true;
  }
  return false;
}

bool LinkShape::update_state() {
  internal();
  if (_input_shape) {
    _head_pos = _input_shape->get_origin();
  }
  if (_output_shape) {
    _tail_pos = _output_shape->get_origin();
  }
  update_state_helper();
  update_positioning_helper(_head_pos, _tail_pos);
  return true;
}

void LinkShape::start_moving() {
  external();
  _interactive = true;
  select(true);
}

void LinkShape::finished_moving() {
  external();
  _interactive = false;
  select(false);
}

// In interactive mode we can set the input shape.
void LinkShape::link_input_shape(const Dep<InputShape>& input_shape) {
  external();
  _input_shape = input_shape;
}
void LinkShape::unlink_input_shape() {
  external();
  _input_shape.reset();
}
const Dep<InputShape>& LinkShape::get_input_shape() const {
  external();
  return _input_shape;
}

// In interactive mode we can also set the output shape.
void LinkShape::link_output_shape(const Dep<OutputShape>& output_shape) {
  external();
  _output_shape = output_shape;
}
void LinkShape::unlink_output_shape() {
  external();
  _output_shape.reset();
}
const Dep<OutputShape>& LinkShape::get_output_shape() const {
  external();
  return _output_shape;
}

void LinkShape::update_positioning_helper(const glm::vec2& head_pos, const glm::vec2& tail_pos) {
  internal();

  if (!is_visible()) {
    _quads.clear();
    _tris.clear();
    return;
  }

  const glm::vec2 diff = head_pos - tail_pos;
  _dir = glm::normalize(diff);
  _perp = glm::vec2(-_dir.y, _dir.x);  // equivalent to dir rotated 90 degrees using right hand rule

  // Nudge the tips of the head shape off of the input and output shapes.
  const glm::vec2 front = head_pos - InputShape::plug_radius * _dir;
  const glm::vec2 back = tail_pos + OutputShape::plug_radius * _dir;
  _full_length = glm::length(front-back);
  _body_length = _full_length - tri_size.y;
  _angle = atan2(_dir.y, _dir.x);

//  // Update our bounds.
//  std::vector<glm::vec2>& verts = _bounds.vertices;
//  verts.resize(7);
//  verts[0] = front - tri_size.y * _dir - tri_size.x/2.0f * _perp;
//  verts[1] = front;
//  verts[2] = verts[0]+ tri_size.x * _perp;
//  verts[3] = verts[2] - (tri_size.x - body_height)/2.0f * _perp;
//  verts[4] = verts[3] - _body_length * _dir;
//  verts[5] = verts[4] - body_height * _perp;
//  verts[6] = verts[5] + _body_length * _dir;

  // Update our head and tail bounds.

    std::vector<glm::vec2>& hb = _border.poly_bound_map[HitRegion::kLinkHeadRegion].vertices;
    hb.resize(7);
    hb[0] = front - tri_size.y * _dir - tri_size.x/2.0f * _perp;
    hb[1] = front;
    hb[2] = hb[0]+ tri_size.x * _perp;
    hb[3] = hb[2] - (tri_size.x - body_height)/2.0f * _perp;
    hb[4] = hb[3] - 0.5f * _body_length * _dir;
    hb[5] = hb[4] - body_height * _perp;
    hb[6] = hb[5] + 0.5f * _body_length * _dir;
    std::vector<glm::vec2>& tb = _border.poly_bound_map[HitRegion::kLinkTailRegion].vertices;
    tb.resize(4);
    tb[0] = hb [4];
    tb[1] = tb [0] - 0.5f* _body_length * _dir;
    tb[2] = tb [1] - body_height * _perp;
    tb[3] = hb [2] + 0.5f * _body_length * _dir;


  // Update our triangle.
  _bg_tri.set_scale(tri_size);
  _bg_tri.set_rotate(_angle + boost::math::constants::pi<float>() - boost::math::constants::pi<float>()/2.0f);
  _bg_tri.set_translate(hb[1], bg_depth);
  _bg_tri.set_color(bg_color);

  // h' = h - 3*border_width
  // w' = w - 2*sqrt(3)*border_width
  _fg_tri.set_scale(tri_size - glm::vec2(2 * sqrt(3.0f) * border_width, 3.0f * border_width));
  _fg_tri.set_rotate(_bg_tri.rotate);
  _fg_tri.set_translate(hb[1] - 2.0f*border_width*_dir, fg_depth);
  _fg_tri.set_color(fg_color);

  // Update our body.
  _bg_quad.set_scale(_body_length, body_height);
  _bg_quad.set_rotate(_angle + boost::math::constants::pi<float>());
  _bg_quad.set_translate(hb[3], bg_depth);
  _bg_quad.set_color(bg_color);

  _fg_quad.set_scale(_body_length, body_height - 2.0f * border_width);
  _fg_quad.set_rotate(_bg_quad.rotate);
  _fg_quad.set_translate(hb[3] + border_width * _dir - border_width * _perp, fg_depth);
  _fg_quad.set_color(fg_color);

  _quads.resize(2);
  _quads[0] = _bg_quad;
  _quads[1] = _fg_quad;

  _tris.resize(2);
  _tris[0] = _bg_tri;
  _tris[1] = _fg_tri;
}

void LinkShape::update_state_helper() {
  internal();
//  if (_interactive) {
//    return;
//  }
  if ((!_input_shape) || (!_output_shape)) {
    return;
  }

  // Update our pannable state.
  bool pannable = _input_shape->is_pannable() && _output_shape->is_pannable();
  set_pannable(pannable);

  std::cerr << "is pannable: " << pannable << "\n";

  if (pannable) {
    _bg_quad.state |= (selected_transform_bitmask);
    _fg_quad.state |= selected_transform_bitmask;
    _bg_tri.state |= (selected_transform_bitmask);
    _fg_tri.state |= selected_transform_bitmask;
  } else {
    _bg_quad.state &= ~(selected_transform_bitmask);
    _fg_quad.state &= ~selected_transform_bitmask;
    _bg_tri.state &= ~(selected_transform_bitmask);
    _fg_tri.state &= ~selected_transform_bitmask;
  }
}

const CompPolyBorder& LinkShape::get_border() const {
  external();
  return _border;
}

const std::vector<ShapeInstance>* LinkShape::get_tri_instances() const {
  external();
  return &_tris;
}

const std::vector<ShapeInstance>* LinkShape::get_quad_instances() const {
  external();
  return &_quads;
}

HitRegion LinkShape::hit_test(const glm::vec2& point) const {
  external();
  if(!is_visible()) {
    return HitRegion::kMissedRegion;
  }
  return _border.hit_test(point);
}

void LinkShape::select(bool selected) {
  external();
  SelectableShape::select(selected);

  std::cerr << "linkshape selected: " << selected << "\n";
  if (selected) {
    _bg_quad.state |= (selected_transform_bitmask|selected_color_bitmask);
    _fg_quad.state |= selected_transform_bitmask;
    _bg_tri.state |= (selected_transform_bitmask|selected_color_bitmask);
    _fg_tri.state |= selected_transform_bitmask;
  } else {
    _bg_quad.state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _fg_quad.state &= ~selected_transform_bitmask;
    _bg_tri.state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _fg_tri.state &= ~selected_transform_bitmask;
  }
}

void LinkShape::set_head_pos(const glm::vec2& pos) {
  external();
  _head_pos = pos;
}

const glm::vec2& LinkShape::get_head_pos() const {
  external();
  return _head_pos;
}

void LinkShape::set_tail_pos(const glm::vec2& pos) {
  external();
  _tail_pos = pos;
}

const glm::vec2& LinkShape::get_tail_position() const {
  external();
  return _tail_pos;
}

float LinkShape::get_angle() const {
  external();
  return _angle;
}

float LinkShape::get_full_length() const {
  external();
  return _full_length;
}

float LinkShape::get_body_length() const {
  external();
  return _body_length;
}

const glm::vec2& LinkShape::get_dir() const {
  external();
  return _dir;
}
const glm::vec2& LinkShape::get_perp() const {
  external();
  return _perp;
}

}
