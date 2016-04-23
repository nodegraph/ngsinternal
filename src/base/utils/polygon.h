#pragma once
#include <base/device/transforms/glmhelper.h>
#include <base/utils/utils_export.h>

#include <vector>

namespace ngs {

class UTILS_EXPORT Polygon {
public:
  // The vertices should contain the points on the border of the polygon.
  // They should be in counter-clockwise order and should only contain one loop.
  // The last/first point on the loop should not be repeated.
  // It is understood that the first and last points on the loop form the last edge of the polygon.
  std::vector<glm::vec2> vertices;

  void offset(const glm::vec2& offset);
  void rotate(const glm::vec2& origin, float theta);
  void scale(const glm::vec2& origin, float scale);

  // Hit test only with the x,y coordinates of point.
  bool contains(const glm::vec2& point) const;

  // Getters.
  glm::vec2 get_center() const;
  void get_aa_bounds(glm::vec2& min, glm::vec2& max) const; // gets the min and max of an axis aligned box around this polygon.
};

}
