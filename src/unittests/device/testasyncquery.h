#pragma once

#if GLES_MAJOR_VERSION >= 3

#include <base/device/transforms/glmhelper.h>
#include <components/resources/dataheaders/es3_shaders.h>


namespace ngs {

struct PosTexVertex;
struct ShapeInstance;
class Quad;


class Pipeline;
class RenderBuffer;
class FrameBuffer;
class Texture;
class AsyncQuery;
class BlockUniformInfos;
class PackedUniformBuffer;
class UniformBuffer;
class BlockUniformInfo;

class TestAsyncQuery {
 public:
  TestAsyncQuery();
  virtual ~TestAsyncQuery();

 private:

  void setup_pipeline();
  void setup_fbo();
  void run_pipeline();

  Quad* _quad;

  // Main pipeline and buffers.
  Pipeline* _pipeline;
  RenderBuffer* _rbo;
  FrameBuffer* _fbo;

  // Textures.
  Texture* _output_texture;

  // Uniform Buffers.
  PackedUniformBuffer* _setup_pubo;
  UniformBuffer* _setup_ubo;

  // Info about our block uniforms.
  const BlockUniformInfo* _model_view_projection_info;
  const BlockUniformInfo* _tint_color_info;

  AsyncQuery* _query;
};

}

#endif
