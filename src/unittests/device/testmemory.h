#pragma once
#include <base/device/geometry/quad.h>

namespace ngs {

class RenderBuffer;

class TestMemory {
 public:
  TestMemory();
  virtual ~TestMemory();

  void draw_quad();
  RenderBuffer* create_depth_rbo(int width, int height);
#if GLES_MAJOR_VERSION >= 3
  void rundown_memory_with_depth_textures();
#endif
  void rundown_memory_with_depth_rbos();
  void rundown_memory_with_textures();
 private:
  static const int _width = 1024;//512;
  static const int _height = 1024;//512;
  static const int _min_gpu_mem = 300000;  // This is in kbytes.

  Quad _quad;
};

}
