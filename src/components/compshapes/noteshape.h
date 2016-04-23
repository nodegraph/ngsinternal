#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>


//// Shapes.
//#include <base/shapes/atomic/text.h>
//#include <base/shapes/atomic/rectangle.h>
//#include <base/shapes/atomic/arrow.h>

namespace ngs {

class Node;
class OutputPlug;
class InputPlug;
class Link;
class Selectable;
class DeviceMemory;

class NodeProperties;

class COMPSHAPES_EXPORT NoteShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, NoteShape);

  // Pill.
  static const glm::vec2 _pill_border_size;

  NoteShape(Entity* entity);
  virtual ~NoteShape();

//  // Visibility.
//  virtual void set_fill_visibility(bool fill_vis);
//  virtual void set_outline_visibility(bool outline_vis);
//
//  // Configure tessellation params.
//  virtual void select(bool selected);
//  virtual bool is_selected();
//
//  // Tessellation.
//  virtual void tessellate();
//  virtual void tessellate_all();
//
//  // Shapes.
//  virtual Text& get_name_text() {return _name_text;}
//
//  // Positioning. (All comp shapes support this.)
//  virtual void set_anchor_position(const glm::vec2& anchor);
//  virtual const glm::vec2& get_anchor_position();
//
//  virtual void set_aux_position(const glm::vec2& aux);
//  virtual const glm::vec2& get_aux_position();
//
//  // Hit testing. (Shape must be tessellated to register a hit.)
//  virtual const Polygon& get_bounds() const;
//  virtual HitType hit_test(const glm::vec2& point);
//
//  // Get selected shape indices. This is used for panning selected shapes.
//  virtual void append_selection_index_sets();
//
//  // Index sets.
//  virtual void add_to_index(IndexSetName name);
//  virtual void remove_from_index(IndexSetName name);

 private:
  // Id.
//  void set_id(unsigned int id);

//  void configure_text();
//  void configure_tablet_shapes();

  Node* _node; // borrowed reference.
  NodeProperties* _props; // borrowed reference.

//  // Shapes.
//  Text _name_text;
//  Text _notes_text;
//  Rectangle _bg;
//  Rectangle _fg;
};


}
