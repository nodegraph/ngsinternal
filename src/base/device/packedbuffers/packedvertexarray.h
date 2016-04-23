#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>
#include <vector>


namespace ngs {

#if GLES_MAJOR_VERSION >= 100

struct DEVICE_EXPORT VertexAttributes {
  float x, y, z;        //Vertex
  float nx, ny, nz;     //Normal
  float s0, t0;         //Texcoord0
  float s1, t1;         //Texcoord1
  float s2, t2;         //Texcoord2
};

#pragma warning(push)
#pragma warning(disable:4251)

class DEVICE_EXPORT PackedVertexArray {
 public:
  PackedVertexArray();
  void bind();
  void draw();

 private:
  GLenum _tex_unit[3];

  std::vector<VertexAttributes> _vertex;
  std::vector<unsigned short> _index;
};

#pragma warning(pop)

#endif

}


