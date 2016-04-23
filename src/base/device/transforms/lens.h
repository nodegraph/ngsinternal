#pragma once

#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>

namespace ngs {

class DEVICE_EXPORT Lens {
 public:
  Lens();
  virtual ~Lens();

  virtual void resize(float width, float height) {}

  const glm::mat4& get_projection() const {return _projection;}

 protected:
  glm::mat4 _projection;
};

class DEVICE_EXPORT PerspectiveLens: public Lens {
 public:
  PerspectiveLens(float fov_y=1.75, float aspect=1.0, float near_z=1, float far_z=10000.0);
  virtual ~PerspectiveLens();

  virtual void resize(float width, float height);

 private:
  void update();

  float _fov_y;
  float _aspect;
  float _near_z;
  float _far_z;
};

class DEVICE_EXPORT OrthographicLens: public Lens {
 public:
  OrthographicLens(float left=0.0, float right=512.0, float bottom=0.0, float top=512.0, float near_z=1, float far_z=10000.0);
  virtual ~OrthographicLens();

  virtual void resize(float width, float height);

 private:
  void update();

  float _left;
  float _right;
  float _bottom;
  float _top;
  float _near_z;
  float _far_z;
};

}
