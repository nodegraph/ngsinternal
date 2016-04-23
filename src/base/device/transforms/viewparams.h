#pragma once
#include <base/device/device_export.h>

namespace ngs {

class DEVICE_EXPORT ViewportParams {
 public:
  ViewportParams():
    x(0),
    y(0),
    width(128),
    height(128)
 {}
  ViewportParams(int pwidth,int pheight):
    x(0),
    y(0),
    width(pwidth),
    height(pheight)
  {}
  int x;
  int y;
  int width;
  int height;
};

class DEVICE_EXPORT PerspectiveParams {
 public:
  float fov_y;
  float aspect;
  float near_z;
  float far_z;
};

class DEVICE_EXPORT OrthographicParams {
 public:
  float left;
  float right;
  float bottom;
  float top;
  float near_z;
  float far_z;
};

}
