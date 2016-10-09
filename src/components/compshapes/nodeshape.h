#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/selectableshape.h>
#include <base/utils/polygon.h>

namespace ngs {

class Resources;
class NodeSelection;
class Compute;

class COMPSHAPES_EXPORT NodeShape: public SelectableShape {
 public:
  COMPONENT_ID(CompShape, InvalidComponent);

  static const float marker_bg_depth;
  static const float marker_fg_depth;

  static const glm::vec2 marker_border_size;

  static const std::array<unsigned char,4> edit_bg_color;
  static const std::array<unsigned char,4> edit_fg_color;
  static const std::array<unsigned char,4> view_bg_color;
  static const std::array<unsigned char,4> view_fg_color;
  static const std::array<unsigned char,4> processing_bg_color;
  static const std::array<unsigned char,4> processing_fg_color;

  NodeShape(Entity* entity, ComponentDID did);
  virtual ~NodeShape();

  // Our sub interfaces.
  virtual bool is_linkable() const {return true;}

  HitRegion hit_test(const glm::vec2& point) const;

  // Our shape instancing.
  virtual const std::vector<ShapeInstance>* get_quad_instances() const {return &_quads_cache;}
  virtual const std::vector<CharInstance> * get_char_instances() const {return &_chars_cache;}
  virtual const std::vector<ShapeInstance>* get_tri_instances() const {return &_tris_cache;}
  virtual const std::vector<ShapeInstance> * get_circle_instances() const {return &_circles_cache;}

  // Selection.
  virtual void select(bool selected);

  // Edit State.
  virtual void show_edit_marker(bool on);
  virtual bool edit_marker_is_showing() const;

  // View State.
  virtual void show_view_marker(bool on);
  virtual bool view_marker_is_showing() const;

  // Processing State.
  // Attention "is_being_processed" is similarly named to currently_processing.
  // Do not confuse this or mistakenly override the other.
  // This process term here should be renamed. It's purely for visualization.
  virtual void show_compute_marker(bool on);
  virtual bool compute_marker_is_showing() const;

 protected:
  virtual void update_quads(const glm::vec2& pen);
  virtual void update_edit_view_text();

  virtual void update_quads_cache();
  virtual void update_chars_cache();
  virtual void update_tris_cache();
  virtual void update_circles_cache();

  // Our fixed deps.
  Dep<Resources> _resources;

  // Our accumulation caches.
  std::vector<ShapeInstance> _tris_cache;
  std::vector<ShapeInstance> _quads_cache;
  std::vector<CharInstance> _chars_cache;
  std::vector<ShapeInstance> _circles_cache;

 private:

  // Our visual marking state.
  bool _show_edit_marker;
  bool _show_view_marker;
  bool _show_compute_marker;

  // Shared state.
  unsigned char _shared_state;

  // Edit and view chars.
  std::vector<CharInstance> _edit_chars; // This just holds an 'E'.
  std::vector<CharInstance> _view_chars; // This just holds an 'V'.
  std::vector<CharInstance> _processing_chars; // This just holds an 'P'.

  // Edit and view shapes.
  ShapeInstance _edit_quad_bg;
  ShapeInstance _edit_quad_fg;
  ShapeInstance _view_quad_bg;
  ShapeInstance _view_quad_fg;
  ShapeInstance _processing_quad_bg;
  ShapeInstance _processing_quad_fg;
};

}
