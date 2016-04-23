#include <components/interactions/viewcontrols.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/device/transforms/coordutil.h>


namespace ngs {

const float ViewControls::_background_screen_z = 0.09980995208024979;


ViewControls::ViewControls()
    : track_ball(-1000) {
  track_ball.reset();
}

ViewControls::~ViewControls() {
}

void ViewControls::resize(int width, int height) {
  viewport.width = width;
  viewport.height = height;
  lens.resize(width, height);
}

const glm::mat4& ViewControls::get_model_view() const {
  return track_ball.get_model_view();
}

const glm::mat4& ViewControls::get_last_model_view() const {
  return track_ball.get_last_model_view();
}

const glm::mat4& ViewControls::get_projection() const {
  return lens.get_projection();
}

glm::mat4 ViewControls::get_model_view_project() const {
  return lens.get_projection() * get_model_view();
}

glm::mat4 ViewControls::get_last_model_view_project() const {
  return lens.get_projection() * get_last_model_view();
}

void ViewControls::start_tracking(const MouseInfo& info) {
  track_ball.start_tracking(info.screen_pos_gl, info.camera_space_pos, info.object_space_pos, viewport, lens.get_projection());
}

void ViewControls::frame(const glm::vec2& min, const glm::vec2& max) {
  track_ball.frame(viewport.width, viewport.height, min, max);
}

void ViewControls::update_coord_spaces(MouseInfo& info) const {
  info.update_transforms(viewport, track_ball.get_model_view(), lens.get_projection(), _background_screen_z);
}

void ViewControls::update_coord_spaces_with_last_model_view(MouseInfo& info) const {
  info.update_transforms(viewport, track_ball.get_last_model_view(), lens.get_projection(), _background_screen_z);
}

glm::vec2 ViewControls::get_center_in_object_space() const {
  glm::vec4 min_clip(-1,-1,1,1);
  glm::vec4 max_clip(1,1,1,1);
  glm::vec4 min_obj, max_obj;
  clip_to_object_coord(track_ball.get_model_view(), lens.get_projection(), min_clip, min_obj);
  clip_to_object_coord(track_ball.get_model_view(), lens.get_projection(), max_clip, max_obj);
  glm::vec4 result =  0.5f * (min_obj + max_obj);
  return result.xy();
}

}
