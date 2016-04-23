#include <components/compshapes/compshape.h>
#include <base/utils/polygon.h>

namespace ngs {

CompShape::CompShape(Entity* entity, size_t did)
    : Component(entity, kIID(), did) {
}

CompShape::~CompShape(){
}

void CompShape::set_pos(const glm::vec2& anchor) {
  start_method();
}

const glm::vec2& CompShape::get_pos() const {
  start_method();
  static const glm::vec2 dummy;
  return dummy;
}

const Polygon& CompShape::get_bounds() const {
  start_method();
  static const Polygon dummy;
  return dummy;
}

HitRegion CompShape::hit_test(const glm::vec2& point) const {
  start_method();
  if (simple_hit_test(point)) {
    return kShape;
  }
  return kMissed;
}

bool CompShape::simple_hit_test(const glm::vec2& point) const {
  start_method();
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
