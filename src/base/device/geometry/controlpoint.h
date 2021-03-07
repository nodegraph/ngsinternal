#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>

namespace ngs {

class SimpleSaver;
class SimpleLoader;

class DEVICE_EXPORT ControlPoint {
  public:
  ControlPoint();
  ControlPoint(float x, float y);
  ControlPoint(const glm::vec2& left, const glm::vec2& center, const glm::vec2& right);
  ControlPoint(const glm::vec2& point);

  // Serialization.
  void save(SimpleSaver& saver);
  void load(SimpleLoader& loader);

  // Operators.
  ControlPoint& operator=(const ControlPoint& other);
  bool operator==(const ControlPoint& other)const;
  bool operator!=(const ControlPoint& other)const;

  // Storage.
  glm::vec2 _center;
  glm::vec2 _in_tangent;
  glm::vec2 _out_tangent;
};

}
