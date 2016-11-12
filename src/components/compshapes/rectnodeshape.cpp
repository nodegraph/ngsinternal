#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/device/geometry/textlimits.h>

#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/resources/resources.h>

#include <components/computes/compute.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/rectnodeshape.h>

// Externals.
#include <freetype-gl/texture-font.h>

// STL.
#include <iostream>

namespace ngs {

const float RectNodeShape::node_bg_depth = 10.0f;
const float RectNodeShape::node_fg_depth = 11.0f;
const float RectNodeShape::button_bg_depth = 12.0f;
const float RectNodeShape::button_fg_depth = 13.0f;

const std::array<unsigned char, 4> RectNodeShape::node_bg_color = { 64, 64, 64, 255 };
const std::array<unsigned char, 4> RectNodeShape::node_fg_color = { 100, 100, 100, 255 };

const glm::vec2 RectNodeShape::node_border_size = glm::vec2(10.0f, 10.0f);

RectNodeShape::RectNodeShape(Entity* entity)
    : NodeShape(entity, kDID()),
      _color(node_bg_color) {
  _node_quad_bg.state = 0;
  _node_quad_fg.state = 0;
}

RectNodeShape::RectNodeShape(Entity* entity, ComponentDID did)
    : NodeShape(entity, did),
      _color(node_bg_color) {
  _node_quad_bg.state = 0;
  _node_quad_fg.state = 0;
}

RectNodeShape::~RectNodeShape() {
}

void RectNodeShape::select(bool selected) {
  external();
  NodeShape::select(selected);

  if (selected) {
    _node_quad_bg.state |= (selected_transform_bitmask|selected_color_bitmask);
    _node_quad_fg.state |= selected_transform_bitmask;
  } else {
    _node_quad_bg.state &= ~(selected_transform_bitmask|selected_color_bitmask);
    _node_quad_fg.state &= ~selected_transform_bitmask;
  }

  // Update the current chars to the selected state.
  // We use the bg quad's state as the canonical state.
  for (CharInstance& ci: _node_name_chars) {
    ci.set_state(_node_quad_fg.state);
  }
}

void RectNodeShape::set_pos(const glm::vec2& pos) {
  external();
  _pos = pos;
}

const glm::vec2& RectNodeShape::get_pos() const {
  external();
  return _pos;
}

bool RectNodeShape::update_state() {
  internal();
  // Update our chars.
  update_text();

  // Update our quads.
  update_node_quads();

  glm::vec2 text_dim = _text_max - _text_min;
  glm::vec2 start = _pos + glm::vec2(text_dim.x + 80, -40);
  update_quads(start);

  // The edit and view text position relies on the edit and view quads.
  update_edit_view_text();

  // Update our accumulation caches.
  update_chars_cache();
  update_quads_cache();

  // We have no triangles.
  _tris_cache.clear();
  return true;
}

void RectNodeShape::update_node_quads() {
  internal();

  // Bg Quad.
  glm::vec2 text_dim = _text_max - _text_min;
  glm::vec2 bg_min = _pos-glm::vec2(70,40);
  glm::vec2 bg_max = _pos+glm::vec2(text_dim.x+70,110);
  glm::vec2 bg_dim = bg_max-bg_min;

  // Update our bounds.
  {
    std::vector<glm::vec2>& verts = _border.poly_bound_map[HitRegion::kNodeRegion].vertices;
    verts.resize(4);
    verts[0] = bg_min;
    verts[1] = verts[0] + glm::vec2(bg_dim.x,0);
    verts[2] = verts[1] + glm::vec2(0, bg_dim.y);
    verts[3] = verts[2] + glm::vec2(-bg_dim.x, 0);
  }

  // Node.
  _node_quad_bg.set_scale(bg_dim);
  _node_quad_bg.set_rotate(0);
  _node_quad_bg.set_translate(bg_min, node_bg_depth);
  _node_quad_bg.set_color(node_bg_color);

  _node_quad_fg.set_scale(bg_dim - 2.0f * node_border_size);
  _node_quad_fg.set_rotate(0);
  _node_quad_fg.set_translate(bg_min + node_border_size, node_fg_depth);
  _node_quad_fg.set_color(node_fg_color);
}

void RectNodeShape::update_quads_cache() {
  internal();
  if (!is_visible()) {
    return;
  }

  NodeShape::update_quads_cache();
  _quads_cache.insert(_quads_cache.end(), _node_quad_bg);
  _quads_cache.insert(_quads_cache.end(), _node_quad_fg);
}

void RectNodeShape::update_text() {
  internal();
  const std::string& name = our_entity()->get_name();
  const glm::vec2 &anchor = _pos;
  _resources->get_text_limits()->tessellate_to_instances(name, glm::vec2(0,0), 0, anchor, _node_quad_fg.state, _node_name_chars, _text_min, _text_max);
}

void RectNodeShape::update_chars_cache() {
  internal();
  if (!is_visible()) {
    return;
  }

  NodeShape::update_chars_cache();
  _chars_cache.insert(_chars_cache.end(), _node_name_chars.begin(), _node_name_chars.end());
}

void RectNodeShape::save(SimpleSaver& saver) const {
  external();
  CompShape::save(saver);
  // Position.
  saver.save(_pos.x);
  saver.save(_pos.y);
  // Color.
  saver.save(_color[0]);
  saver.save(_color[1]);
  saver.save(_color[2]);
  saver.save(_color[3]);
}
void RectNodeShape::load(SimpleLoader& loader) {
  external();
  CompShape::load(loader);
  // Position.
  loader.load(_pos.x);
  loader.load(_pos.y);
  // Color.
  loader.load(_color[0]);
  loader.load(_color[1]);
  loader.load(_color[2]);
  loader.load(_color[3]);
}

}
