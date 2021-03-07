#include <base/device/transforms/lens.h>
#include <base/device/transforms/transform.h>

namespace ngs {

Lens::Lens() {
}

Lens::~Lens() {
}

// Perspective.


PerspectiveLens::PerspectiveLens(float fov_y, float aspect, float near_z, float far_z):
  _fov_y(fov_y),
  _aspect(aspect),
  _near_z(near_z),
  _far_z(far_z){
  update();
}

PerspectiveLens::~PerspectiveLens() {
}

void PerspectiveLens::resize(float width, float height) {
  _aspect = width/height;
  update();
}

void PerspectiveLens::update() {
  Transform::get_perspective(_fov_y,_aspect,_near_z,_far_z,_projection);
}


// Orthographic.

OrthographicLens::OrthographicLens(float left, float right, float bottom, float top, float near_z, float far_z):
  _left(left),
  _right(right),
  _bottom(bottom),
  _top(top),
  _near_z(near_z),
  _far_z(far_z){
  update();
}

OrthographicLens::~OrthographicLens() {
}

void OrthographicLens::resize(float width, float height) {
  _right = width;
  _top = height;
  update();
}

void OrthographicLens::update() {
  Transform::get_orthographic(_left,_right,_bottom,_top,_near_z,_far_z,_projection);
}


}
