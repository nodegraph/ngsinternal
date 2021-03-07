#include <base/device/transforms/rayutil.h>

#define TEST_CULL

#define EPSILON 0.000001

#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

namespace ngs {


bool intersects_triangle(float orig[3], float dir[3], float vert0[3],
                       float vert1[3], float vert2[3], float *t, float *u,
                       float *v) {
  float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
  float det, inv_det;

  /* find vectors for two edges sharing vert0 */
  SUB(edge1, vert1, vert0);
  SUB(edge2, vert2, vert0);

  /* begin calculating determinant - also used to calculate U parameter */
  CROSS(pvec, dir, edge2);

  /* if determinant is near zero, ray lies in plane of triangle */
  det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
  if (det < EPSILON)
    return 0;

  /* calculate distance from vert0 to ray origin */
  SUB(tvec, orig, vert0);

  /* calculate U parameter and test bounds */
  *u = DOT(tvec, pvec);
  if (*u < 0.0 || *u > det)
    return 0;

  /* prepare to test V parameter */
  CROSS(qvec, tvec, edge1);

  /* calculate V parameter and test bounds */
  *v = DOT(dir, qvec);
  if (*v < 0.0 || *u + *v > det)
    return 0;

  /* calculate t, scale parameters, ray intersects triangle */
  *t = DOT(edge2, qvec);
  inv_det = 1.0f / det;
  *t *= inv_det;
  *u *= inv_det;
  *v *= inv_det;
#else                    /* the non-culling branch */
  if (det > -EPSILON && det < EPSILON)
  return 0;
  inv_det = 1.0 / det;

  /* calculate distance from vert0 to ray origin */
  SUB(tvec, orig, vert0);

  /* calculate U parameter and test bounds */
  *u = DOT(tvec, pvec) * inv_det;
  if (*u < 0.0 || *u > 1.0)
  return 0;

  /* prepare to test V parameter */
  CROSS(qvec, tvec, edge1);

  /* calculate V parameter and test bounds */
  *v = DOT(dir, qvec) * inv_det;
  if (*v < 0.0 || *u + *v > 1.0)
  return 0;

  /* calculate t, ray intersects triangle */
  *t = DOT(edge2, qvec) * inv_det;
#endif
  return 1;
}


bool ray_intersects_line(const glm::vec2& ray_start, const glm::vec2& ray_end, const glm::vec2& line_seg_start, const glm::vec2& line_seg_end) {

    float d1, d2;
    float a1, a2, b1, b2, c1, c2;

    // Convert vector 1 to a line (line 1) of infinite length.
    // We want the line in linear equation standard form: A*x + B*y + C = 0
    // See: http://en.wikipedia.org/wiki/Linear_equation
    a1 = ray_end.y - ray_start.y;
    b1 = ray_start.x - ray_end.x;
    c1 = (ray_end.x * ray_start.y) - (ray_start.x * ray_end.y);

    // Every point (x,y), that solves the equation above, is on the line,
    // every point that does not solve it, is either above or below the line.
    // We insert (x1,y1) and (x2,y2) of vector 2 into the equation above.
    d1 = (a1 * line_seg_start.x) + (b1 * line_seg_start.y) + c1;
    d2 = (a1 * line_seg_end.x) + (b1 * line_seg_end.y) + c1;

    // If d1 and d2 both have the same sign, they are both on the same side of
    // our line 1 and in that case no intersection is possible. Careful, 0 is
    // a special case, that's why we don't test ">=" and "<=", but "<" and ">".
    if (d1 > 0 && d2 > 0) return false;
    if (d1 < 0 && d2 < 0) return false;

    // We repeat everything above for vector 2.
    // We start by calculating line 2 in linear equation standard form.
    a2 = line_seg_end.y - line_seg_start.y;
    b2 = line_seg_start.x - line_seg_end.x;
    c2 = (line_seg_end.x * ray_start.y) - (line_seg_start.x * line_seg_end.y);

    // Calulate d1 and d2 again, this time using points of vector 1
    d1 = (a2 * ray_start.x) + (b2 * ray_start.y) + c2;
    d2 = (a2 * ray_end.x) + (b2 * ray_end.y) + c2;

    // Again, if both have the same sign (and neither one is 0),
    // no intersection is possible.
    if (d1 > 0 && d2 > 0) return false;
    if (d1 < 0 && d2 < 0) return false;

    // If we get here, only three possibilities are left. Either the two
    // vectors intersect in exactly one point or they are collinear
    // (they both lie both on the same infinite line), in which case they
    // may intersect in an infinite number of points or not at all.
    if ((a1 * b2) - (a2 * b1) == 0.0f) return true;

    // If they are not collinear, they must intersect in exactly one point.
    return true;
}

bool intersects_polygon2D(const glm::vec2& point, const std::vector<glm::vec2>& sides) {
  // Test the ray against all sides
  int intersections = 0;
  glm::vec2 ray_start(point.x-100, point.y);
  for (size_t side = 0; side < sides.size(); ++side) {
      // Test if current side intersects with ray.
      // If yes, intersections++;
    size_t next_side(0);
    if (side == sides.size()-1) {
      next_side = 0;
    } else {
      next_side = side+1;
    }
    if (ray_intersects_line(ray_start,point,sides[side],sides[next_side])) {
      ++intersections;
    }
  }

  if ((intersections & 1) == 1) {
      return true; // Inside polygon.
  }
  return false;// Outside polygon.
}

}
