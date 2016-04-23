#include <base/device/transforms/transform.h>
#include <base/device/transforms/viewparams.h>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

namespace ngs {

const glm::mat4 Transform::kIdentity = glm::mat4();

// Rotation.
void Transform::get_x_rotation(float radians, glm::mat4& m) {
  m = kIdentity;
  m[1][1] = std::cos(radians);
  m[1][2] = std::sin(radians);
  m[2][1] = -std::sin(radians);
  m[2][2] = std::cos(radians);
}

void Transform::get_y_rotation(float radians, glm::mat4& m) {
  m = kIdentity;
  m[0][0] = std::cos(radians);
  m[0][2] = -std::sin(radians);
  m[2][0] = std::sin(radians);
  m[2][2] = std::cos(radians);
}

void Transform::get_z_rotation(float radians, glm::mat4& m) {
  m = kIdentity;
  m[0][0] = std::cos(radians);
  m[0][1] = std::sin(radians);
  m[1][0] = -std::sin(radians);
  m[1][1] = std::cos(radians);
}

void Transform::get_viewport(float x, float y, float width, float height, glm::mat4& m) {
  m = kIdentity;
  m[0][0] = width / 2.0;
  m[1][0] = 0.0;
  m[2][0] = 0.0;
  m[3][0] = x + width / 2.0;

  m[0][1] = 0.0;
  m[1][1] = height / 2.0;
  m[2][1] = 0.0;
  m[3][1] = y + height / 2.0;

  m[0][2] = 0.0;
  m[1][2] = 0.0;
  m[2][2] = 1.0;
  m[3][2] = 0.0;

  m[0][3] = 0.0;
  m[1][3] = 0.0;
  m[2][3] = 0.0;
  m[3][3] = 1.0;
}

void Transform::get_viewport_parameters(const glm::mat4& m, ViewportParams& v) {
  v.width = boost::math::round(m[0][0] * 2.0);
  v.x = boost::math::round(m[3][0] - (v.width / 2.0));
  v.height = boost::math::round(m[1][1] * 2.0);
  v.y = boost::math::round(m[3][1] - (v.height / 2.0));
}

void Transform::get_perspective(float fov_y, float aspect, float near_z, float far_z, glm::mat4& m) {
  m = kIdentity;
  float f = 1.0 / tan(fov_y * 0.5);

  m[0][0] = f / aspect;
  m[0][1] = 0.0;
  m[0][2] = 0.0;
  m[0][3] = 0.0;

  m[1][0] = 0.0;
  m[1][1] = f;
  m[1][2] = 0.0;
  m[1][3] = 0.0;

  m[2][0] = 0.0;
  m[2][1] = 0.0;
  m[2][2] = (far_z + near_z) / (near_z - far_z);
  m[2][3] = -1.0;

  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 2.0 * ((far_z * near_z) / (near_z - far_z));
  m[3][3] = 0.0;
}

void Transform::get_perspective_parameters(const glm::mat4& m, PerspectiveParams& p) {
  float f = m[1][1];
  p.fov_y = 2.0 * atan2(1.0f, f);
  p.aspect = f / m[0][0];
  float X = m[2][2];
  float Y = m[3][2];
  p.near_z = Y / (X + 1.0);
  p.far_z = Y / (X - 1.0);
}

bool Transform::is_perspective(const glm::mat4& m) {
  if ((std::abs(m[3][3]) < 0.000001) && (std::abs(m[2][3] + 1.0) < 0.000001)) {
    return true;
  }
  return false;
}

void Transform::get_orthographic(float left, float right, float bottom, float top, float near_z, float far_z, glm::mat4& m) {
  m = kIdentity;
  m[0][0] = 2.0 / (right - left);
  m[0][1] = 0.0;
  m[0][2] = 0.0;
  m[0][3] = 0.0;

  m[1][0] = 0.0;
  m[1][1] = 2.0 / (top - bottom);
  m[1][2] = 0.0;
  m[1][3] = 0.0;

  m[2][0] = 0.0;
  m[2][1] = 0.0;
  m[2][2] = -2.0 / (far_z - near_z);
  m[2][3] = 0.0;

  m[3][0] = -((right + left) / (right - left));
  m[3][1] = -((top + bottom) / (top - bottom));
  m[3][2] = -((far_z + near_z) / (far_z - near_z));
  m[3][3] = 1.0;
}

void Transform::get_orthographic_parameters(const glm::mat4& m, OrthographicParams& p) {
  float AA = m[0][0];
  float BB = m[1][1];
  float CC = m[2][2];
  float DD = m[3][0];
  float EE = m[3][1];
  float FF = m[3][2];

  p.left = -(DD / AA + 1.0 / AA);
  p.right = -(DD / AA - 1.0 / AA);
  p.bottom = -(EE / BB + 1.0 / BB);
  p.top = -(EE / BB - 1.0 / BB);
  p.near_z = (FF / CC + 1.0 / CC);
  p.far_z = (FF / CC - 1.0 / CC);
}

void Transform::get_rotation(const glm::vec3& rotation, glm::mat4& m) {
  glm::mat4 xm;
  get_x_rotation(rotation.x, xm);
  glm::mat4 ym;
  get_y_rotation(rotation.y, ym);
  glm::mat4 zm;
  get_z_rotation(rotation.z, zm);
  m = zm * ym * xm;
}

void Transform::get_translation(const glm::vec3& translation, glm::mat4& m) {
  m = kIdentity;
  m[3][0] = translation.x;
  m[3][1] = translation.y;
  m[3][2] = translation.z;
}

void Transform::get_scale(const glm::vec3& zoom, glm::mat4& m) {
  m = kIdentity;
  m[0][0] = zoom.x;
  m[1][1] = zoom.y;
  m[2][2] = zoom.z;
}

void Transform::get_pivot_rotate(const glm::vec3& rotation, const glm::vec4& camera_pivot, glm::mat4& m) {
  // Get translation to camera pivot.
  glm::vec3 trans(camera_pivot.x, camera_pivot.y, camera_pivot.z);

  // Camera to pivot space.
  glm::mat4 pivot_space;
  get_translation(-trans, pivot_space);

  // Rotation in pivot space.
  glm::mat4 rotation_matrix;
  get_rotation(rotation, rotation_matrix);

  // Pivot to camera space.
  glm::mat4 unpivot_space;
  get_translation(trans, unpivot_space);

  // Modify our preview orientation with these new_ matrices.
  m = unpivot_space * rotation_matrix * pivot_space;

}

void Transform::get_pivot_zoom(const glm::vec3& zoom, const glm::vec4& camera_pivot, glm::mat4& m) {
  // Get translation to camera pivot.
  glm::vec3 trans(camera_pivot.x, camera_pivot.y, camera_pivot.z);

  // Camera to pivot space.
  glm::mat4 pivot_space;
  get_translation(-trans, pivot_space);

  // Scale in pivot space.
  glm::mat4 scale_matrix;
  get_scale(zoom, scale_matrix);

  // Pivot to camera space.
  glm::mat4 unpivot_space;
  get_translation(trans, unpivot_space);

  // Concatenate the matrices.
  m = unpivot_space * scale_matrix * pivot_space;
}

}

