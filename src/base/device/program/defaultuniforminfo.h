#pragma once
#include <base/device/device_export.h>

#include <base/device/program/uniforminfo.h>

namespace ngs {

class UniformInfo;

class DEVICE_EXPORT DefaultUniformInfo : public UniformInfo {
 public:
  DefaultUniformInfo();
  DefaultUniformInfo(const DefaultUniformInfo& other);
  virtual ~DefaultUniformInfo();

  // Custom methods.
  void set_location(GLint location);
  GLint get_location() const;

 private:
  GLint _location;

};

}
