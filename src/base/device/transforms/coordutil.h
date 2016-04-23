#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>

/*
 clip_coord=object_coord*model_view*projection

 device_coord.x=clip_coord.x/clip_coord.w
 device_coord.y=clip_coord.y/clip_coord.w
 device_coord.z=clip_coord.z/clip_coord.w

 window_coord.x=viewport.width/2.0*device_coord.x + viewport.x + viewport.width/2.0
 window_coord.y=viewport.height/2.0*device_coord.y + viewport.y + viewport.height/2.0
 window_coord.z=(far_z-near_z)/2.0*device_coord.z + (near_z+far_z)/2.0
 */

// Device is officially supposed to be 3-tuple, while clip is supposed to be 4-tuple.
// But for us the device and clip are equal here as ilm's Imath assumes an implicit 1
// in the homogenous coordinate for 3-tuples when multiplying.

namespace ngs {

 class ViewportParams;

 void DEVICE_EXPORT object_to_clip_coord(const glm::mat4& model_view, const glm::mat4& projection,
 const glm::vec4& object_coord, glm::vec4& clip_coord);

 void DEVICE_EXPORT device_to_window_coord(const ViewportParams& viewport, float near_z, float far_z,
 const glm::vec4& device_coord, glm::vec4& window_coord);

 void DEVICE_EXPORT window_to_device_coord(const ViewportParams& viewport, float near_z, float far_z,
 const glm::vec4& window_coord, glm::vec4& device_coord);

 void DEVICE_EXPORT clip_to_object_coord(const glm::mat4& model_view, const glm::mat4& projection,
 const glm::vec4& clip_coord, glm::vec4& object_coord);

}
