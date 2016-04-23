#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>
#include <ostream>

// ------------------------------------------------------------
// Notes:
// ------------------------------------------------------------
// We use GLM matrices which are stored in column major order.
// These matrices are also multiplied by vectors on the right.
// OpenGL uses the same mental paradigm.
// For example: M[i][k]
// -- this means we're grabbing the k'th row from i'th column.
// -- the first index is the column.
//
// In contract Imath stores elements in row major order and
// the vectors are multiplied on the left.


namespace ngs {

class ViewportParams;
class PerspectiveParams;
class OrthographicParams;


class DEVICE_EXPORT Transform {
 public:

  static const glm::mat4 kIdentity;

  // Rotation.
  static void get_x_rotation(float radians, glm::mat4& m);
  static void get_y_rotation(float radians, glm::mat4& m);
  static void get_z_rotation(float radians, glm::mat4& m);

  // Viewport.
  static void get_viewport(float x, float y, float width, float height, glm::mat4& m);
  static void get_viewport_parameters(const glm::mat4& m, ViewportParams& v);

  // Perspective Projection.
  static void get_perspective(float fov_y, float aspect, float near_z, float far_z, glm::mat4& m);
  static void get_perspective_parameters(const glm::mat4& m, PerspectiveParams& p);
  static bool is_perspective(const glm::mat4& m);

  // Orthographic Projection.
  static void get_orthographic(float left, float right, float bottom, float top, float near_z, float far_z, glm::mat4& m);
  static void get_orthographic_parameters(const glm::mat4& m, OrthographicParams& p);

  // Rotation.
  static void get_rotation(const glm::vec3& rotation, glm::mat4& m); //rotation.xyz holds the rotation angles per axis.

  // Translation.
  static void get_translation(const glm::vec3& translation, glm::mat4& m); //translation.xyz holds the translation amounts per axis.

  // Scale.
  static void get_scale(const glm::vec3& zoom, glm::mat4& m); //zoom.xyz holds the zoom amounts per axis.

  // Perform a rotation around a pivot specified in camera space. rotation holds the rotation angles per axis.
  static void get_pivot_rotate(const glm::vec3& rotation, const glm::vec4& camera_pivot, glm::mat4& m);

  // Zoom around a pivot specified in camera space. zooms holds the zoom amounts per axis.
  static void get_pivot_zoom(const glm::vec3& zoom, const glm::vec4& camera_pivot, glm::mat4& m);
};

}
