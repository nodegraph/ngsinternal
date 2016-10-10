#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/selectableshape.h>
#include <base/utils/polygon.h>

namespace ngs {

class Resources;
class NodeSelection;
class Compute;

class NodeMarker {
 public:
  static const float _bg_depth;
  static const float _fg_depth;
  static const glm::vec2 _border_size;

  NodeMarker();
  ~NodeMarker();

  void set_bg_color(const std::array<unsigned char,4>& bg_color);
  void set_fg_color(const std::array<unsigned char,4>& fg_color);
  void set_letter(const std::string& letter);

  void show(bool show) {_show_marker = show;}
  bool is_showing() const {return _show_marker;}

  void set_state(unsigned char state);
  void update_quads(glm::vec2& start);
  void update_quads_cache(std::vector<ShapeInstance>& quads_cache);
  void update_chars(Resources* resources, unsigned char state);
  void update_chars_cache(std::vector<CharInstance>& chars_cache);

 private:
  bool _show_marker;

  std::array<unsigned char,4> _bg_color;
  std::array<unsigned char,4> _fg_color;
  std::string _letter;

  ShapeInstance _bg_quad;
  ShapeInstance _fg_quad;
  std::vector<CharInstance> _chars;
};

class COMPSHAPES_EXPORT NodeShape: public SelectableShape {
 public:
  COMPONENT_ID(CompShape, InvalidComponent);

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
  virtual void show_compute_marker(bool on);
  virtual bool compute_marker_is_showing() const;

  // Clean State.
  virtual void show_clean_marker(bool on);
  virtual bool clean_marker_is_showing() const;

  // Error State.
  virtual void show_error_marker(bool on);
  virtual bool error_marker_is_showing() const;

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

  NodeMarker _edit_marker;
  NodeMarker _view_marker;
  NodeMarker _compute_marker;
  NodeMarker _clean_marker;
  NodeMarker _error_marker;

  // Shared state.
  unsigned char _shared_state;
};

}
