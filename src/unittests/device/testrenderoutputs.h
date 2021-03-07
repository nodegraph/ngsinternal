#pragma once
#include <base/device/geometry/quad.h>
#include <base/device/packedbuffers/elementid.h>

#include <string>

#include <base/device/transforms/glmhelper.h>
#include <components/resources/dataheaders/unittestshaders.h>

namespace ngs {

/*
 This class tests the gpu pipeline class using a very simple setup.
 The vertex shader simply does model view project on the vertices.
 The fragment shader samples an input texture, tints it, and sends it to the output texture.
 These tests are templated on the texture element type.
 Both input and output textures are templated on the same type in these tests.
 Although in the real world they likely won't.

 Note we have three fragment shaders we test with.
 floating_texture_value.f
 int_texture_value.f
 uint_texture_value.f

 The normalized version uses a sampler which returns a normalized value.
 The int version uses a sampler which returns a integer value.
 The uint version uses a sampler which returns a unsigned interger value.
 Note the glsl shader is able to use these samplers because an appropriate unpacked_format
 was used when creating the texture.
 */

class RenderBuffer;
class FrameBuffer;
class Texture;
class Pipeline;

class BlockUniformInfos;
class PackedUniformBuffer;
class UniformBuffer;
class DefaultUniformInfo;
class BlockUniformInfo;

#if GLES_MAJOR_VERSION >= 3

class TestRenderOutputs {
 public:
  TestRenderOutputs(ElementID element_id, bool normalized_access);
  virtual ~TestRenderOutputs();

 private:
  std::string get_vertex_shader();
  // The fragment shader changes for element types less than 4 bytes, when normalized_access is changed
  std::string get_fragment_shader();
  // The tint color is adjust based on the element type so that the
  // output always comes out to 2 for each pixel component.
  glm::vec4 get_tint_color();

  void setup_pipeline();
  void setup_fbo();
  void run_pipeline();

  // Determine whether we'll be testing with textures which yield normalized values or not.
  bool _normalized_access;
  ElementID _element_id;

  // Main pipeline and buffers.
  Pipeline* _pipeline;
  RenderBuffer* _rbo;
  FrameBuffer* _fbo;

  // Textures.
  Texture* _input_texture;
  Texture* _output_texture;

  // Uniform Buffers.
  PackedUniformBuffer* _setup_pubo;
  UniformBuffer* _setup_ubo;

  // Info about our default uniforms.
  const DefaultUniformInfo* _image_texture_info;

  // Info about our block uniforms.
  const BlockUniformInfo* _model_view_projection_info;
  const BlockUniformInfo* _tint_color_info;

  Quad _quad;
};

#endif
}
