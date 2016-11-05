#include <base/utils/polyborder.h>
#include <components/compshapes/compshape.h>

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

const CompPolyBorder& CompShape::get_border() const {
  external();
  static const CompPolyBorder dummy;
  return dummy;
}

HitRegion CompShape::hit_test(const glm::vec2& point) const {
  external();
  return HitRegion::kMissedRegion;
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
