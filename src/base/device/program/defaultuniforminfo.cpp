#include <base/device/program/defaultuniforminfo.h>
#include <base/device/program/uniforminfo.h>

namespace ngs {

DefaultUniformInfo::DefaultUniformInfo()
    : UniformInfo(),
      _location(0) {
}

DefaultUniformInfo::DefaultUniformInfo(const DefaultUniformInfo& other)
    : UniformInfo(other),
      _location(other.get_location()) {
}

DefaultUniformInfo::~DefaultUniformInfo() {
}

void DefaultUniformInfo::set_location(GLint location) {
  _location = location;
}

GLint DefaultUniformInfo::get_location() const {
  return _location;
}

}

