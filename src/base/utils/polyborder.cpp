#include <base/device/geometry/instancevertexattribute.h>
#include <boost/math/constants/constants.hpp>
#include <base/device/geometry/instancevertexattribute.h>
#include <base/utils/polyborder.h>

namespace ngs {

void PolyBorder::offset(const glm::vec2& offset) {
  for (size_t i=0; i< vertices.size(); ++i) {
    vertices[i] += offset;
  }
}

void PolyBorder::rotate(const glm::vec2& origin, float angle) {
  for (size_t i = 0; i<vertices.size(); ++i) {
    glm::vec2 delta = vertices[i]-origin;
    float rx = cos(angle)*delta.x - sin(angle)*delta.y;
    float ry = sin(angle)*delta.x + cos(angle)*delta.y;
    vertices[i].x = rx + origin.x;
    vertices[i].y = ry + origin.y;
  }
}

void PolyBorder::scale(const glm::vec2& origin, float scale) {
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

bool PolyBorder::contains(const glm::vec2& point) const
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

glm::vec2 PolyBorder::get_center() const
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

void PolyBorder::get_aa_bounds(glm::vec2& min, glm::vec2& max)const{
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

// ------------------------------------------------------------------------
// CompPolyBorder.
// ------------------------------------------------------------------------

bool CompPolyBorder::contains(const glm::vec2& point) const {
  for (auto &iter: poly_bound_map) {
    if (iter.second.contains(point)) {
      return true;
    }
  }
  return false;
}

void CompPolyBorder::get_aa_bounds(glm::vec2& min, glm::vec2& max) const {
  // Initialize return value.
  min=glm::vec2(0,0);
  max=glm::vec2(0,0);

  // Loop over the poly bounds.
  bool first = true;
  for (auto &iter : poly_bound_map) {
    glm::vec2 low;
    glm::vec2 high;
    iter.second.get_aa_bounds(low, high);
    update_extremes(low, high, first, min, max);
  }
}

void CompPolyBorder::update_extremes(const glm::vec2& low, const glm::vec2& high, bool& first, glm::vec2& min, glm::vec2& max) {
  // Skip empty poly bounds.
  if (low == high) {
    return;
  }

  // If this is the first one, we initialize min and max with it.
  if (first) {
    min = low;
    max = high;
    first = false;
  } else {
    // Otherwise, we update the extremes of the aa bound.
    if (low.x < min.x) {
      min.x = low.x;
    }
    if (low.y < min.y) {
      min.y = low.y;
    }
    if (high.x > max.x) {
      max.x = high.x;
    }
    if (high.y > max.y) {
      max.y = high.y;
    }
  }
}

void CompPolyBorder::coalesce_bounds(const std::vector<CompPolyBorder>& bounds, glm::vec2& min, glm::vec2& max) {
  // Static Method.
  min=glm::vec2(0,0);
  max=glm::vec2(0,0);

  bool first = true;
  for (size_t i=0; i<bounds.size(); ++i) {
    // Handle the nodes.
    glm::vec2 low;
    glm::vec2 high;
    bounds[i].get_aa_bounds(low,high);
    update_extremes(low, high, first, min, max);
  }
}

HitRegion CompPolyBorder::hit_test(const glm::vec2& point) const {
  for (auto & iter: poly_bound_map) {
    if (iter.second.contains(point)) {
      return iter.first;
    }
  }
  return HitRegion::kMissedRegion;
}

}
