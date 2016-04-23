#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <string>

// Type can be one of the following.

// Vector types.
// FLOAT, FLOAT_VEC2, FLOAT_VEC3, FLOAT_VEC4, 
// INT, INT_VEC2, INT_VEC3, INT_VEC4, 
// UNSIGNED_INT, UNSIGNED_INT_VEC2, UNSIGNED_INT_VEC3, or UNSIGNED_INT_VEC4.

// Matrix types.
// FLOAT_MAT2, FLOAT_MAT3, FLOAT_MAT4, FLOAT_MAT2x3, FLOAT_MAT2x4, FLOAT_MAT3x2, FLOAT_MAT3x4, FLOAT_MAT4x2, FLOAT_MAT4x3, 

namespace ngs {

/*
 This class wraps information about a Vertex Attribute.
 Vertex attributes are vertices and related attributes like texture coordinates.
 */

class DEVICE_EXPORT AttributeInfo {
 public:
  AttributeInfo();
  AttributeInfo(const AttributeInfo& other);

  std::string name;  // name in the glsl shader
  GLint size;  // number of elements
  GLenum type;  // data type
  GLuint location;  // this corresponds to the attribute index.
};

}



