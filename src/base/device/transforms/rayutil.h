#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>

#include <vector>

//  http://jgt.akpeters.com/papers/MollerTrumbore97/code.html

namespace ngs {

// 3d intersection.
bool DEVICE_EXPORT intersects_triangle(float orig[3], float dir[3], float vert0[3],
                              float vert1[3], float vert2[3], float *t,
                              float *u, float *v);

// 2d intersection with polygons.
bool DEVICE_EXPORT ray_intersects_line(const glm::vec2& ray_start, const glm::vec2& ray_end, const glm::vec2& line_seg_start, const glm::vec2& line_seg_end);
bool DEVICE_EXPORT intersect_polygon2D(const glm::vec2& point, const std::vector<glm::vec2>& sides);

}



