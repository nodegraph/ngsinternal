#include <base/device/transforms/trackball.h>
#include <base/device/transforms/transform.h>
#include <cmath>
#include <iostream>
#include <ostream>



namespace ngs {

TrackBall::TrackBall(float default_pull_back) {
  // The default zoom out from the origin.
  _default_pull_back = default_pull_back;

  // Manipulation Speeds.
  _rotation_speed = 0.01;
  _translation_speed = 1.0;
  _zoom_speed = 1.0;
  _dolly_speed = 1.0;

  // Zoom Limits.
  _zoom_min = -50.0;
  _zoom_max = 100.0;

  // Whether we're rolling the trackball.
  _tracking = false;

  // Object pivot.
  _object_space_pivot = glm::vec4(0, 0, 0, 1);

  // Reset Matrices.
  reset();
}

TrackBall::~TrackBall() {

}

void TrackBall::reset() {
  _model_view = glm::mat4();
  _model_view[3][2] = _default_pull_back;
  _last_model_view = _model_view;

  // Update threaded model view.
  update_model_view_thread_safe();
  update_last_model_view_thread_safe();
}

void TrackBall::rotate(const glm::vec3 &rotation) {
  glm::vec3 scaled = _rotation_speed * rotation;
  glm::vec4 camera_pivot =  _model_view * _object_space_pivot;
  glm::mat4 t;
  Transform::get_pivot_rotate(scaled, _object_space_pivot, t);
  _model_view = t * _model_view;

  // Update threaded model view.
  update_model_view_thread_safe();
}

void TrackBall::translate(const glm::vec3& translation) {
  glm::vec3 scaled = _translation_speed * translation;
  // Apply the translation.
  glm::mat4 t;
  Transform::get_translation(scaled, t);
  _model_view = t * _model_view;

  // Update threaded model view.
  update_model_view_thread_safe();
}

void TrackBall::zoom(float zoom, const glm::vec4 &camera_pivot) {
  float scaled = std::pow(1.01f, _zoom_speed * zoom);
  glm::vec3 scaled3(scaled, scaled, 1.0);
  glm::mat4 t;
  Transform::get_pivot_zoom(scaled3, camera_pivot, t);
  _model_view = t * _model_view;

  // Update threaded model view.
  update_model_view_thread_safe();
}

void TrackBall::dolly(float z_trans) {
  glm::vec3 trans(0, 0, z_trans * _dolly_speed);
  glm::mat4 t;
  Transform::get_translation(trans, t);
  _model_view = t * _model_view;

  // Update threaded model view.
  update_model_view_thread_safe();
}

void TrackBall::frame(float view_width, float view_height, const glm::vec2& min_point, const glm::vec2& max_point) {
  if (min_point == max_point) {
    return;
  }
  // Currently this only works for orthogonal projection.
  // The projection is setup to map (0,0) to the bottom left, and (view_width,view_height) to the top right.

  // Determine the max scale.
  glm::vec2 diff = max_point - min_point;
  glm::vec2 scale = diff/glm::vec2(view_width,view_height);
  float max_scale = 1;
  bool max_scale_is_horizontal = false;
  if (scale.x>scale.y) {
    max_scale_is_horizontal = true;
    max_scale = scale.x;
  } else {
    max_scale_is_horizontal = false;
    max_scale = scale.y;
  }

  // Determine the translation.
  glm::vec2 translation;
  if (max_scale_is_horizontal) {
    translation.x = min_point.x;
    translation.y = min_point.y - 0.5*(view_height*max_scale -diff.y);
  } else {
    translation.y = min_point.y;
    translation.x = min_point.x - 0.5*(view_width*max_scale -diff.x);
  }

  // Set the model view.
  _model_view = glm::mat4();
  _model_view[0][0] = 1.0f/max_scale;
  _model_view[1][1] = 1.0f/max_scale;
  _model_view[2][2] = 1.0f/max_scale;

  _model_view[3][0] = -translation.x/max_scale;
  _model_view[3][1] = -translation.y/max_scale;
  _model_view[3][2] = _default_pull_back;

  // Update threaded model view.
  update_model_view_thread_safe();
}

void TrackBall::start_tracking(const glm::vec2 &screen_pos_gl,
                               const glm::vec4 &camera_space_click,
                               const glm::vec4 &object_space_click,
                               const ViewportParams &viewport,
                               const glm::mat4 &projection) {
  _tracking = true;


  // our tracking state
  _last_model_view = _model_view;
  _last_screen_click_gl = screen_pos_gl;
  _last_camera_space_click = camera_space_click;  // used for zooms
  _last_object_space_click = object_space_click;  // used for rotations
  _last_viewport = viewport;
  _last_projection = projection;

  // Update threaded last model view.
  update_last_model_view_thread_safe();
}

void TrackBall::stop_tracking() {
  if (_tracking) {
    _tracking = false;
  }
}

bool TrackBall::track(const glm::vec2 &screen_pos_gl, const TrackBall::Mode& action) {
  // If the mouse is not down return.
  if (!_tracking) {
    return false;
  }

  // Find out how much the mouse has moved in screen space.
  glm::vec2 diff = screen_pos_gl - _last_screen_click_gl;
  float dx = diff.x;
  float dy = diff.y;

  // Reset the model to where it was on the last mouse down.
  _model_view = _last_model_view;

  // Update threaded model view.
  update_model_view_thread_safe();

  // The  key modifier allows the mouse to control rotation.
  // Alt+left controls rotation on x and y axis.
  if (action == TrackBall::ROTATE) {  // rotation on x and y axis.
    rotate(glm::vec3(dy, dx, 0));
  } else if (action == TrackBall::DOLLY) {  // dollies the camera not zoom.
    float z_trans = dx + dy;
    dolly(z_trans);
  } else if (action == TrackBall::ZOOM) {  // zooms the camera.
    float zoom_amount = dx + dy;
    zoom(zoom_amount, _last_camera_space_click);
  } else if (action == TrackBall::TRANSLATE) {  // translates the camera.
    float z_factor = _last_camera_space_click[2];
    z_factor *= _last_projection[2][3];
    if (z_factor == 0) {  // With orthographic projections proj[2][3] is zero.
      z_factor = 1;
    }
    float width = _last_viewport.width;
    float height = _last_viewport.height;
    float s = dx * 2.0 / width * z_factor / _last_projection[0][0];
    float t = dy * 2.0 / height * z_factor / _last_projection[1][1];

    glm::vec3 diff(s, t, 0);
    translate(diff);
  }

  return true;
}

void TrackBall::wheel_zoom(const glm::vec4& center_in_camera_space, float zoom_amount) {
  zoom(zoom_amount, center_in_camera_space);
}

void TrackBall::pinch_zoom(const glm::vec4& center_in_camera_space, float factor) {
  glm::vec3 scaled3(factor, factor, 1.0);
  glm::mat4 t;
  Transform::get_pivot_zoom(scaled3, center_in_camera_space, t);
  _model_view = t * _last_model_view;

  // Update threaded model view.
  update_model_view_thread_safe();
}

void TrackBall::finalize_pinch_zoom() {
  _last_model_view = _model_view;
}

void TrackBall::set_pivot(const glm::vec4& object_space_click) {
  _object_space_pivot = object_space_click;
}

void TrackBall::set_model_view(const glm::mat4& model_view) {
	_model_view=model_view;
	_last_model_view = _model_view;

	// Update threaded copies.
	update_model_view_thread_safe();
	update_last_model_view_thread_safe();
}

void TrackBall::revert_to_last_model_view() {
  set_model_view(get_last_model_view());
}

const glm::mat4& TrackBall::get_model_view_thread_safe(Permit<TrackBall>& holder) {
	check_permit(holder);
	return _model_view_thread_safe;
}

const glm::mat4& TrackBall::get_model_view_thread_safe() {
	Permit<TrackBall> holder(this);
	return get_model_view_thread_safe(holder);
}

const glm::mat4& TrackBall::get_last_model_view_thread_safe(Permit<TrackBall>& holder) {
	check_permit(holder);
	return _last_model_view_thread_safe;
}

const glm::mat4& TrackBall::get_last_model_view_thread_safe() {
	Permit<TrackBall> holder(this);
	return get_last_model_view_thread_safe(holder);
}


void TrackBall::update_model_view_thread_safe() {
	Permit<TrackBall> holder(this);
	set_model_view_thread_safe(holder,_model_view);
}

void TrackBall::set_model_view_thread_safe(Permit<TrackBall>& holder, const glm::mat4& model_view) {
	check_permit(holder);
	_model_view_thread_safe = model_view;
}

void TrackBall::update_last_model_view_thread_safe() {
	Permit<TrackBall> holder(this);
	set_last_model_view_thread_safe(holder,_last_model_view);
}

void TrackBall::set_last_model_view_thread_safe(Permit<TrackBall>& holder, const glm::mat4& last_model_view) {
	check_permit(holder);
	_last_model_view_thread_safe = last_model_view;
}

void TrackBall::check_permit(Permit<TrackBall>& holder) {
  // Make sure the holder has locked us.
  if (!holder.is_for(this)) {
    assert(false);
  }
}

}
