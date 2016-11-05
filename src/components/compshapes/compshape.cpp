#include <components/compshapes/compshape.h>
#include <base/utils/polygon.h>

namespace ngs {

CompShape::CompShape(Entity* entity, ComponentDID did)
    : Component(entity, kIID(), did),
      _pannable(false){
}

CompShape::~CompShape(){
}

void CompShape::set_pos(const glm::vec2& anchor) {
  external();
}

const glm::vec2& CompShape::get_pos() const {
  external();
  static const glm::vec2 dummy;
  return dummy;
}

void CompShape::set_pannable(bool pannable) {
  _pannable = pannable;
}

bool CompShape::is_pannable() const {
  return _pannable;
}

const CompPolyBounds& CompShape::get_bounds() const {
  external();
  static const CompPolyBounds dummy;
  return dummy;
}

HitRegion CompShape::hit_test(const glm::vec2& point) const {
  external();
  if (simple_hit_test(point)) {
    return HitRegion::kShape;
  }
  return HitRegion::kMissedRegion;
}

bool CompShape::simple_hit_test(const glm::vec2& point) const {
  external();
  return get_bounds().contains(point);
}

const std::vector<ShapeInstance>* CompShape::get_tri_instances() const {
  return NULL;
}

const std::vector<ShapeInstance>* CompShape::get_circle_instances() const {
  return NULL;
}

const std::vector<ShapeInstance>* CompShape::get_quad_instances() const {
  return NULL;
}

const std::vector<CharInstance> * CompShape::get_char_instances() const {
  return NULL;
}

}
