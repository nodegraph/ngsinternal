#pragma once
#include <base/device/device_export.h>
#include <base/device/geometry/elementsource.h>

namespace ngs {

struct DEVICE_EXPORT PosTexVertex {
  float position[3];
  float tex_coord[2];

  static const ElementSource position_source;
  static const ElementSource tex_coord_source;
};

}
