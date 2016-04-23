#include <base/objectmodel/entity.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/noteshape.h>
#include <components/compshapes/outputshape.h>

namespace ngs {

const glm::vec2 NoteShape::_pill_border_size(3,3);

NoteShape::NoteShape(Entity* entity):
    CompShape(entity, kDID()),
//    _name_text(system_buffers),
//    _notes_text(system_buffers),
//    _bg(system_buffers),
//    _fg(system_buffers),
    _node(NULL),
    _props(NULL){

  // Set our id.
//  set_id(_id);

//  // Setup the text shape for the name.
//  _name_text.text = _node->get_name();
//  _name_text.text_size = MEDIUM_TEXT;
//  _name_text.position = _props->get_anchor_pos();
//  _name_text.fill_depth = -8;
//
//  // Setup the text shape for the notes.
//  _notes_text.text = _node->get_name();
//  _notes_text.text_size = SMALL_TEXT;
//  _notes_text.position = _props->get_anchor_pos();
//  _notes_text.fill_depth = -8;
//
//  // Setup the pill's bg shape.
//  _bg.fill_depth = -10;
//  _bg.fill_color = {26,26,26,255};
//
//  // Setup the pill's fg shape.
//  _fg.fill_depth = -9;
//  _fg.fill_color = {77,77,77,255};

}

NoteShape::~NoteShape() {
//  _props = NULL;
}


//void NoteShape::set_id(unsigned int id) {
//  _name_text.id = id;
//  _notes_text.id = id;
//  _bg.id = id;
//  _fg.id = id;
//}

//void NoteShape::set_fill_visibility(bool fill_vis) {
//  _name_text.draw_fill = fill_vis;
//  _notes_text.draw_fill = fill_vis;
//  _bg.draw_fill = fill_vis;
//  _fg.draw_fill = fill_vis;
//}
//
//void NoteShape::set_outline_visibility(bool outline_vis) {
//  _name_text.draw_fill = outline_vis;
//  _notes_text.draw_fill = outline_vis;
//  _bg.draw_outline = outline_vis;
//  _fg.draw_outline = outline_vis;
//}
//
//void NoteShape::select(bool selected) {
//  if (selected) {
//    _bg.draw_outline = true;
//  } else {
//    _bg.draw_outline = false;
//  }
//}
//
//bool NoteShape::is_selected() {
//  if (_bg.draw_outline) {
//    return true;
//  }
//  return false;
//}
//
//
//void NoteShape::configure_text() {
//
//  const glm::vec2& aux_pos = _props->get_aux_pos();
//  const glm::vec2& anchor_pos = _props->get_anchor_pos();
//
//  // Position the node name.
//  _name_text.text = _node->get_name();
//  _name_text.position = anchor_pos + glm::vec2(15,-45);
//
//  // Position the notes starting point.
//  _notes_text.text = _props->get_notes();
//  _notes_text.position = _name_text.position-glm::vec2(0,35);
//
//  // Determine the wrap length.
//  _notes_text.wrap_length = aux_pos.x -(2*15)- 2*_pill_border_size.x;
//}
//
//
//void NoteShape::configure_tablet_shapes() {
//
//  const glm::vec2 &aux_pos = _props->get_aux_pos(); // this is interpreted as width and height
//  const glm::vec2 &anchor_pos = _props->get_anchor_pos();
//
//  glm::vec2 min(anchor_pos.x,anchor_pos.y-aux_pos.y);
//  glm::vec2 max(anchor_pos.x+aux_pos.x,anchor_pos.y);
//
//  _bg.set_axis_aligned(min,max);
//  _fg.set_axis_aligned(min+_pill_border_size, max-_pill_border_size);
//}
//
//void NoteShape::set_anchor_position(const glm::vec2& anchor) {
//  _props->set_anchor_pos(anchor);
//}
//
//const glm::vec2& NoteShape::get_anchor_position() {
//  return _props->get_anchor_pos();
//}
//
//void NoteShape::set_aux_position(const glm::vec2& aux) {
//  _props->set_aux_pos(aux);
//}
//
//const glm::vec2& NoteShape::get_aux_position() {
//  return _props->get_aux_pos();
//}
//
//void NoteShape::tessellate() {
//  configure_text();
//  _name_text.tessellate();
//  _notes_text.tessellate();
//
//  // Pack the pill shapes using the bounds of the tessellated text.
//  configure_tablet_shapes();
//  _bg.tessellate();
//  _fg.tessellate();
//}
//
//void NoteShape::tessellate_all() {
//  // Tessellate ourself.
//  tessellate();
//}
//
//const Polygon& NoteShape::get_bounds() {
//  return _bg.get_tessellated()->get_bounds();
//}
//
//HitType NoteShape::hit_test(const glm::vec2& point) {
//  if (!simple_hit_test(point)) {
//    return MISSED;
//  }
//  bool left = false;
//  bool right = false;
//  bool bottom = false;
//  bool top = false;
//
//  const float delta = 15;
//
//  const glm::vec2 &aux_pos = _props->get_aux_pos(); // this is interpreted as width and height
//  const glm::vec2 &anchor_pos = _props->get_anchor_pos();
//
//  glm::vec2 min(anchor_pos.x,anchor_pos.y-aux_pos.y);
//  glm::vec2 max(anchor_pos.x+aux_pos.x,anchor_pos.y);
//
//  if (point.x - min.x < delta) {
//    left = true;
//  } else if (max.x-point.x < delta) {
//    right = true;
//  }
//
//  if (point.y - min.y < delta) {
//    bottom = true;
//  } else if (max.y-point.y < delta) {
//    top = true;
//  }
//
//  if (bottom) {
//    if (left) {
//      return BOTTOM_LEFT_CORNER;
//    } else if (right) {
//      return BOTTOM_RIGHT_CORNER;
//    }
//    return BOTTOM_EDGE;
//  }
//
//  if (top) {
//    if (left) {
//      return TOP_LEFT_CORNER;
//    } else if (right) {
//      return TOP_RIGHT_CORNER;
//    }
//    return TOP_EDGE;
//  }
//
//  if (left) {
//    return LEFT_EDGE;
//  } else if (right) {
//    return RIGHT_EDGE;
//  }
//
//  return INSIDE_SHAPE_REGION;
//}
//
//// Get selected shape indices. This is used for panning selected shapes.
//void NoteShape::append_selection_index_sets() {
//  PanningMode mode = kNotPanning;
//  if (is_selected()) {
//    mode = kSelectedPanning;
//  } else {
//    mode = kUnselectedPanning;
//  }
//
//  _name_text.get_tessellated()->set_selection_index(mode);
//  _notes_text.get_tessellated()->set_selection_index(mode);
//  _bg.get_tessellated()->set_selection_index(mode);
//  _fg.get_tessellated()->set_selection_index(mode);
//}
//
//void NoteShape::add_to_index(IndexSetName name) {
//  _name_text.add_to_index(name);
//  _notes_text.add_to_index(name);
//  _bg.add_to_index(name);
//  _fg.add_to_index(name);
//}
//
//void NoteShape::remove_from_index(IndexSetName name) {
//  _name_text.remove_from_index(name);
//  _notes_text.remove_from_index(name);
//  _bg.remove_from_index(name);
//  _fg.remove_from_index(name);
//}

}
