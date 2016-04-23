#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>


namespace ngs {

class ViewportParams;

class DEVICE_EXPORT MouseInfo {
 public:

  void update_transforms(const ViewportParams& viewport, const glm::mat4& model_view, const glm::mat4 &projection, float background_z);

  // Mouse position can be in a number of coordinates.
  glm::vec2 screen_pos; // window (qt) screen coordinates. (x increases left to right, y increases top to bottom)
  glm::vec2 screen_pos_gl; // gl screen coordinates. (x increases left to right, y increases bottom to top)
  glm::vec4 camera_space_pos;
  glm::vec4 object_space_pos;

  // Which button.
  bool left_button;
  bool middle_button;
  bool right_button;

  // Modifiers.
  bool control_modifier;
  bool alt_modifier;
  bool shift_modifier;
};

// OpenGL Transformation Pipeline
// 1) object coordinates = point
// 2) eye coordinates = point*model_view
// 3) clip coordinates = eye*projection
// 4) normalized device coordinates --> eye[0]/=eye[3], eye[1]/=eye[3], eye[2]/=eye[3]
// 5) screen/window coordinates = ndc * view_transform --> 0<=x<=width, 0<=y<=height, 0<=z<=1


class ViewportParams;

// Converts a screen space click to object space.
glm::vec4 DEVICE_EXPORT get_object_space_click(const glm::mat4 &model_view,
                                 const glm::mat4 &projection,
                                 const ViewportParams& viewport,
                                 const glm::vec2 &screen_click_gl,
                                 float background_screen_z);

// Converts a object space click to camera space.
glm::vec4 DEVICE_EXPORT get_camera_space_click(const glm::vec4 &object_space_click, const glm::mat4 &model_view);

// Converts a screen space click to opengl screen space, where y increases from bottom to top.
glm::vec2 DEVICE_EXPORT get_screen_click_gl(const ViewportParams &viewport, const glm::vec2 &screen_click);


MouseInfo DEVICE_EXPORT get_vec2_info(const glm::vec2& screen_pos);

}
