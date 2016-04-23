#include <base/device/geometry/controlpoint.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

namespace ngs {

ControlPoint::ControlPoint() {
}

ControlPoint::ControlPoint(float x, float y) {
  glm::vec2 point(x,y);
  _center = point;
  _in_tangent = point;
  _out_tangent = point;
}

ControlPoint::ControlPoint(const glm::vec2& left, const glm::vec2& center, const glm::vec2& right) {
  _in_tangent=left;
  _center = center;
  _out_tangent=right;
}

ControlPoint::ControlPoint(const glm::vec2& point) {
  _center = point;
  _in_tangent = point;
  _out_tangent = point;
}

void ControlPoint::save(SimpleSaver& saver) {
  saver.save(_center.x);
  saver.save(_center.y);
  saver.save(_in_tangent.x);
  saver.save(_in_tangent.y);
  saver.save(_out_tangent.x);
  saver.save(_out_tangent.y);
}

void ControlPoint::load(SimpleLoader& loader) {
  loader.load(_center.x);
  loader.load(_center.y);
  loader.load(_in_tangent.x);
  loader.load(_in_tangent.y);
  loader.load(_out_tangent.x);
  loader.load(_out_tangent.y);
}

ControlPoint& ControlPoint::operator=(const ControlPoint& other) {
  _center = other._center;
  _in_tangent = other._in_tangent;
  _out_tangent = other._out_tangent;
  return *this;
}

bool ControlPoint::operator==(const ControlPoint& right) const {
    if(_center!=right._center)
    {
        return false;
    }
    if(_in_tangent!=right._in_tangent)
    {
        return false;
    }
    if(_out_tangent!=right._out_tangent)
    {
        return false;
    }
    return true;
}

bool ControlPoint::operator!=(const ControlPoint& other)const {
  return !(*this==other);
}

}
