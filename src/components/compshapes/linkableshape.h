#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/selectableshape.h>
#include <base/utils/polygon.h>

namespace ngs {

class Resources;
class NodeGraphSelection;
class Compute;

class COMPSHAPES_EXPORT LinkableShape: public SelectableShape {
 public:
  COMPONENT_ID(CompShape, InvalidComponent);

  static const float marker_bg_depth;
  static const float marker_fg_depth;

  static const glm::vec2 marker_border_size;

  static const std::array<unsigned char,4> edit_bg_color;
  static const std::array<unsigned char,4> edit_fg_color;
  static const std::array<unsigned char,4> view_bg_color;
  static const std::array<unsigned char,4> view_fg_color;

  LinkableShape(Entity* entity, size_t did);
  virtual ~LinkableShape();

  // Our sub interfaces.
  virtual bool is_linkable() {return true;}

  // Our state.
  virtual void update_state();

  // Input and Output Ordering.
  virtual size_t get_input_order(const std::string& input_name) const = 0;
  virtual size_t get_output_order(const std::string& output_name) const = 0;

  // Num inputs and outputs.
  virtual size_t get_num_linkable_inputs() const = 0; // number of inputs with input plugs
  virtual size_t get_num_linkable_outputs() const = 0; // number of outputs with output plugs
  virtual size_t get_num_input_params() const = 0; // number of inputs without input plugs
  virtual size_t get_num_output_params() const = 0; // number of outputs without output plugs
  virtual size_t get_num_all_inputs() const = 0; // number of all inputs
  virtual size_t get_num_all_outputs() const = 0; // number of all outputs

  // Our shape instancing.
  virtual const std::vector<ShapeInstance>* get_quad_instances() const {return &_quads_cache;}
  virtual const std::vector<CharInstance> * get_char_instances() const {return &_chars_cache;}
  virtual const std::vector<ShapeInstance>* get_tri_instances() const {return &_tris_cache;}
  virtual const std::vector<ShapeInstance> * get_circle_instances() const {return &_circles_cache;}

  // Selection.
  virtual void select(bool selected);

  // Edit State.
  virtual void edit(bool on);
  virtual bool is_being_edited() const;

  // View State.
  virtual void view(bool on);
  virtual bool is_being_viewed() const;

 protected:
  virtual void update_edit_view_quads(const glm::vec2& pen);
  virtual void update_edit_view_text();

  virtual void update_quads_cache();
  virtual void update_chars_cache();
  virtual void update_tris_cache();
  virtual void update_circles_cache();

  // Our fixed deps.
  Dep<Resources> _resources;

  // Our accumulation caches.
  std::vector<ShapeInstance> _tris_cache; // This is a cache of our quads.
  std::vector<ShapeInstance> _quads_cache; // This is a cache of our quads.
  std::vector<CharInstance> _chars_cache; // This is a cache of our quads.
  std::vector<ShapeInstance> _circles_cache; // This is a cache of our circles.

 private:

  // Ard edit and view state.
  bool _being_edited;
  bool _being_viewed;

  // Shared state.
  unsigned char _shared_state;

  // Edit and view chars.
  std::vector<CharInstance> _edit_chars; // This just holds an 'E'.
  std::vector<CharInstance> _view_chars; // This just holds an 'V'.

  // Edit and view shapes.
  ShapeInstance _edit_quad_bg;
  ShapeInstance _edit_quad_fg;
  ShapeInstance _view_quad_bg;
  ShapeInstance _view_quad_fg;

};

}
