#include <base/device/deviceheadersgl.h>
#include <base/device/program/attributeinfo.h>

namespace ngs {

AttributeInfo::AttributeInfo() {
  name = "";  // name in the glsl shader
  size = 0;  // number of elements
  type = 0;  // data type
  location = 0;  // this corresponds to the attribute index.
}

AttributeInfo::AttributeInfo(const AttributeInfo& other) {
  name = other.name;
  size = other.size;
  type = other.type;
  location = other.location;
}

}

