#pragma once
#include <components/interactions/interactions_export.h>

#include <base/device/transforms/lens.h>
#include <base/device/transforms/trackball.h>
#include <base/device/transforms/viewparams.h>

namespace ngs {

class MouseInfo;

class INTERACTIONS_EXPORT ViewControls {
 public:
  ViewControls();
  virtual ~ViewControls();

  // These are some helper method to make dealing with
  // the view related structures easier.

  void resize(int width, int height);
  const glm::mat4& get_model_view() const;
  const glm::mat4& get_last_model_view() const;
  const glm::mat4& get_projection() const;
  glm::mat4 get_model_view_project() const;
  glm::mat4 get_last_model_view_project() const;

  void start_tracking(const MouseInfo& info);
  void frame(const glm::vec2& min, const glm::vec2& max);

  void update_coord_spaces(MouseInfo& info) const;
  void update_coord_spaces_with_last_model_view(MouseInfo& info) const;

  glm::vec2 get_center_in_object_space() const;

  static const float _background_screen_z;

  TrackBall track_ball; // controls the model view.
  OrthographicLens lens; // controls the projection.
  ViewportParams viewport; // controls the viewport.
};

}
