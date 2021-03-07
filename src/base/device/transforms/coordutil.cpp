#include <base/device/transforms/coordutil.h>
#include <base/device/transforms/viewparams.h>

namespace ngs {

void object_to_clip_coord(const glm::mat4& model_view,
                          const glm::mat4& projection,
                          const glm::vec4& object_coord,
                          glm::vec4& clip_coord) {
   clip_coord =  projection * model_view * object_coord;
}

void device_to_window_coord(const ViewportParams& viewport, float near_z, float far_z,
                            const glm::vec4& device_coord,
                            glm::vec4& window_coord) {

  window_coord.x = viewport.width / 2.0f * device_coord.x + viewport.x + viewport.width / 2.0f;
  window_coord.y = viewport.height / 2.0f * device_coord.y + viewport.y + viewport.height / 2.0f;
  window_coord.z = (far_z - near_z) / 2.0f * device_coord.z + (near_z + far_z) / 2.0f;

}

void window_to_device_coord(const ViewportParams& viewport, float near_z, float far_z,
                            const glm::vec4& window_coord,
                            glm::vec4& device_coord) {

  device_coord.x = 2.0f / viewport.width * (window_coord.x - viewport.x) - 1.0f;
  device_coord.y = 2.0f / viewport.height * (window_coord.y - viewport.y) - 1.0f;
  device_coord.z = 2.0f / (far_z - near_z) * (window_coord.z - near_z - far_z);

}

void clip_to_object_coord(const glm::mat4& model_view,
                          const glm::mat4& projection,
                          const glm::vec4& clip_coord,
                          glm::vec4& object_coord) {

  glm::mat4 unproject = projection * model_view;
  unproject = glm::inverse(unproject);
  object_coord = unproject * clip_coord;

}

}
