#pragma once
#include <base/device/transforms/glmhelper.h>
#include <base/device/transforms/viewparams.h>
#include <base/utils/permit.h>


#include <mutex>

namespace ngs {

class DEVICE_EXPORT TrackBall {
 public:
  TrackBall(float default_pull_back);
  ~TrackBall();

  // Lockable.
  void lock() {
    _mutex.lock();
  }
  void unlock() {
    _mutex.unlock();
  }

  enum Mode {
    ROTATE = 0,
    TRANSLATE = 1,
    DOLLY = 2, // only useful with perspective. dolly doesn't affect orthograph projections
    ZOOM = 3, // useful for both perspective and orthographic
  };


  void reset(); // resets matrices.
  void rotate(const glm::vec3 &rotation);
  void translate(const glm::vec3& translation);
  void zoom(float zoom, const glm::vec4 &camera_pivot);
  void dolly(float z_trans);
  void frame(float view_width, float view_height, const glm::vec2& min, const glm::vec2& max);

  // Tracking. (With shift and control, you can drag zoom and drag rotate.)
  void start_tracking(const glm::vec2 &screen_pos_gl,
                       const glm::vec4 &camera_space_click,
                       const glm::vec4 &object_space_click,
                       const ViewportParams &viewport,
                       const glm::mat4 &projection);
  void stop_tracking();
  bool track(const glm::vec2 &screen_pos_gl, const TrackBall::Mode& action);

  // Wheel Zoom. (This doesn't use mouse tracking.)
  void wheel_zoom(const glm::vec4& center_in_camera_space, float zoom_amount);

  // Pinch Zoom.
  void pinch_zoom(const glm::vec4& center_in_camera_space, float factor);
  void finalize_pinch_zoom();

  // Pivot.
  void set_pivot(const glm::vec4& object_space_click);

  // Getters.
  const glm::mat4& get_model_view() const {return _model_view;}
  const glm::mat4& get_last_model_view() const {return _last_model_view;} // last model view before starting to track

  // Setters.
  void set_model_view(const glm::mat4& model_view);
  void revert_to_last_model_view();

  // Multithread reads from other treads.
  // The manipulation of the track ball should be done from the main gui thread.
  // Other threads like rendering thread can only read the matrix.
  const glm::mat4& get_model_view_thread_safe(Permit<TrackBall>& holder);
  const glm::mat4& get_model_view_thread_safe();
  const glm::mat4& get_last_model_view_thread_safe(Permit<TrackBall>& holder);
  const glm::mat4& get_last_model_view_thread_safe();


 private:
  void check_permit(Permit<TrackBall>& holder);

  void update_model_view_thread_safe();
  void set_model_view_thread_safe(Permit<TrackBall>& holder, const glm::mat4& model_view);

  void update_last_model_view_thread_safe();
  void set_last_model_view_thread_safe(Permit<TrackBall>& holder, const glm::mat4& last_model_view);

  // The default zoom out from the origin.
  float _default_pull_back;

  // Manipulation Speeds.
  float _rotation_speed;
  float _translation_speed;
  float _zoom_speed;
  float _dolly_speed;

  // Zoom Limits.
  float _zoom_min;
  float _zoom_max;

  // Whether we're rolling the trackball.
  bool _tracking;

  // Object pivot.
  glm::vec4 _object_space_pivot;

  // Matrices.
  glm::mat4 _model_view;

  // Info from the last mouse down event.
  glm::mat4 _last_model_view;
  glm::vec2 _last_screen_click_gl;
  glm::vec4 _last_camera_space_click; // used for zooms
  glm::vec4 _last_object_space_click; // used for rotations

  ViewportParams _last_viewport;
  glm::mat4 _last_projection;

  // Locking.
  std::recursive_mutex _mutex;
  glm::mat4 _model_view_thread_safe;
  glm::mat4 _last_model_view_thread_safe;

};

}
