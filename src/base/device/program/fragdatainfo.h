#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <string>

namespace ngs {

/*
 This class wraps information about an output coming out of the fragment shader.

 */

struct DEVICE_EXPORT FragDataInfo {
  FragDataInfo() {
    location = 0;
    index = 0;
    type = 0;
  }
  std::string name;  // Name of an output variable in the fragment shader.
  GLint location;  // The color attachement number to which the output was bound when compiled.
  GLint index;  // Index may be zero or one to specify that the color be used as either the first or second color input to the blend equation,
  GLenum type;  // This is the type of the fragment shader output variable. It is obtained from scanning the fragment shader.
};

}

