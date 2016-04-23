#include <base/utils/polygon.h>
#include <base/device/geometry/instancevertexattribute.h>
#include <boost/math/constants/constants.hpp>
#include <base/device/geometry/instancevertexattribute.h>

namespace ngs {

void Polygon::offset(const glm::vec2& offset) {
  for (size_t i=0; i< vertices.size(); ++i) {
    vertices[i] += offset;
  }
}

void Polygon::rotate(const glm::vec2& origin, float angle) {
  for (size_t i = 0; i<vertices.size(); ++i) {
    glm::vec2 delta = vertices[i]-origin;
    float rx = cos(angle)*delta.x - sin(angle)*delta.y;
    float ry = sin(angle)*delta.x + cos(angle)*delta.y;
    vertices[i].x = rx + origin.x;
    vertices[i].y = ry + origin.y;
  }
}

void Polygon::scale(const glm::vec2& origin, float scale) {
  for (size_t i = 0; i<vertices.size(); ++i) {
    glm::vec2 &pos = vertices[i];

    // Perform the scale.
    float dx = pos.x - origin.x;
    float dy = pos.y - origin.y;
    float rx = scale*dx;
    float ry = scale*dy;
    pos.x = rx + origin.x;
    pos.y = ry + origin.y;
  }
}

bool Polygon::contains(const glm::vec2& point) const
{
  bool inside = false;
  int next(0);
  int prev(vertices.size()-1);
  while(next<vertices.size()) {
    if ((vertices[next].y>point.y) != (vertices[prev].y>point.y)) {
      glm::vec2 diff = vertices[prev] - vertices[next];
      if (point.x < diff.x/diff.y*(point.y-vertices[next].y) + vertices[next].x) {
        inside = !inside;
      }
    }
    prev=next;
    ++next;
  }
  return inside;
}

glm::vec2 Polygon::get_center() const
{
  glm::vec2 sum;
  if (vertices.empty()) {
    return sum;
  }

  for(const glm::vec2& ele: vertices) {
    sum += ele;
  }
  return sum = sum/static_cast<float>(vertices.size());

}

void Polygon::get_aa_bounds(glm::vec2& min, glm::vec2& max)const{
  if (vertices.size()) {
    min = vertices[0];
    max = vertices[0];
  } else {
    min = glm::vec2();
    max = glm::vec2();
  }
  for(const glm::vec2& vert: vertices) {
    if (vert.x < min.x) {
      min.x=vert.x;
    }
    if (vert.x > max.x) {
      max.x=vert.x;
    }
    if (vert.y < min.y) {
      min.y=vert.y;
    }
    if (vert.y > max.y) {
      max.y = vert.y;
    }
  }
}

}
