#if GLES_MAJOR_VERSION >= 3

#include "testmultisampling.h"
#include <base/memoryallocator/taggednew.h>
#include <base/device/geometry/quad.h>

#include <base/device/deviceheadersgl.h>

// Std headers.
#include <iostream>
#include <vector>

// Packed buffers.
#include <base/device/packedbuffers/packedtexture.h>

// Unpacked buffers.
#include <base/device/unpackedbuffers/texture.h>
#include <base/device/unpackedbuffers/renderbuffer.h>
#include <base/device/unpackedbuffers/framebuffer.h>
#include <base/device/unpackedbuffers/uniformbuffer.h>

#include <base/device/program/program.h>
#include <base/device/program/fragdatainfo.h>
#include <base/device/program/fragdatainfos.h>
#include <base/device/program/pipeline.h>
#include <base/device/program/defaultuniforminfos.h>
#include <base/device/program/blockuniforminfos.h>
#include <base/device/program/attributeinfos.h>

#include <base/device/packedbuffers/packeduniformbuffer.h>

#include <base/device/devicedebug.h>

#include <base/device/transforms/glmhelper.h>
#include <glm/gtc/matrix_transform.hpp>

#include <components/resources/resources.h>


/*

 The multi-sampling pattern with 4 samples on quadro fx.
 a=(0.375,0.125)
 b=(0.875,0.375)
 c=(0.125,0.625)
 d=(0.625,0.875)



 Note that the fragment shader is evaluated only once at o=(0.5,0.5) despite the multiple samples.
 So sometimes o is actually outside the polygon and texture coordinate will be out of bounds and will wrap.

 The way that multi-sampling works in opengl is that the fragment color is evaluated once and gets copied
 to the sample locations which overlapped the polygon. (Note implementation wise the hardware may not
 actually be copying the values.)

 Note also that in OpengGL 4.0 and glsl 4.0 it is possible to evaluate the fragment shader at all the multi-samples.

 test4 ------------------------------------
 d
 test3 ------------------------------------
 c
 test2 -------------o----------------------
 b
 test1 ------------------------------------
 a
 test0 ------------------------------------

 The input bar texture is (0,0,0,0) in the bottom half and (1,1,1,1) in the top half.
 The input bar texture using nearest neighbor filtering.

 We shift the textured quad slightly vertically so that it partially covers the multi-sample pixels.
 Alsoe we use a bacground clear color of (0.1,0.1,0.1,0.1).

 Test0: shift(0,0)
 Quad covers: a,b,c,d
 Background covers:
 color: a=b=c=d=(0,0,0,0),
 so color=(a+b+c+d)/4=(0,0,0,0)

 Test1: shift(0,0.25)
 Quad covers: b,c,d
 Background covers: a
 color: a=(0.1,0.1,0.1,0.1)
 b=c=d=(0,0,0,0)
 so color=(a+b+c+d)/4=(0.025,0.025,0.025,0.025)

 Test2: shift(0,0.5)
 Quad covers: c,d
 Background cover: a,b
 color: a=b=(0.1,0.1,0.1,0.1)
 c=d=(0,0,0,0)
 so color=(a+b+c+d)/4=(0.05,0.05,0.05,0.05)

 Test2b: shift(0,0.6) -> when evaluating at o, it gets a negative-y texture coordinate
 Quad covers: c,d
 Background covers: a,b
 color: a=b=(0.1,0.1,0.1,0.1)
 c=d=(1,1,1,1) -> because of the negative-y texture coordinate, wrapping, and nearest pixel settings.
 so color=(a+b+c+d)/4=(0.55,0.55,0.55,0.55)

 Test3: shift(0,0.75) -> when evaluating at o, it gets a negative-y texture coordinate
 Quad covers: d
 Background covers: a,b,c
 color: a=b=c=(0.1,0.1,0.1,0.1)
 d=(1,1,1,1) -> because of the negative-y texture coordinate, wrapping, and nearest pixel settings.
 so color=(a+b+c+d)/4=(0.325,0.325,0.325,0.325)

 Test4: shift(0,1)
 Quad covers: none
 Background covers: a,b,c
 color: a=b=c=d=(0.1,0.1,0.1,0.1)
 so color=(a+b+c+d)/4=(0.1,0.1,0.1,0.1)
 */

namespace ngs {

TestMultiSampling::TestMultiSampling()
    : _quad(NULL),
      _pipeline(NULL),
      _centroid_pipeline(NULL),
      _input_texture(NULL),
      _fbo(NULL),
      _depth_stencil_rbo(NULL),
      _color_rbo(NULL),
      _blit_fbo(NULL),
      _blit_texture(NULL) {
  setup_pipeline();
  setup_multisample_fbo();
  check_sample_positions();
  setup_blit_fbo();

  // Test0
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0));
  setup_quad();
  run_pipeline();
  check_blit_texture(0, 0);
  delete_ff(_quad);

  // Test1
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.25));
  setup_quad();
  run_pipeline();
  check_blit_texture(0.025f, 0);
  delete_ff(_quad);

  // Test2
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.5));
  setup_quad();
  run_pipeline();
  check_blit_texture(0.05f, 0);
  delete_ff(_quad);

  // Test2b
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.6));
  setup_quad();
  run_pipeline();
  check_blit_texture(0.55f, 0);
  delete_ff(_quad);

  // Test3
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.75));
  setup_quad();
  run_pipeline();
  check_blit_texture(0.325f, 0);
  delete_ff(_quad);

  // Test4
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 1.0));
  setup_quad();
  run_pipeline();
  check_blit_texture(0.1f, 0);
  delete_ff(_quad);

  // Test with centroid sampling.
  // Centroid sampling will make sure to use a sample that's covered by the polygon.
  // This way there will be no negative texture coordinates in the fragment shader.
  // So no texture wrapping, to pickup the nearest texel which was white.

  setup_centroid_pipeline();

  // Test0
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0));
  setup_centroid_quad();
  run_centroid_pipeline();
  check_blit_texture(0, 0);
  delete_ff(_quad);

  // Test1
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.25));
  setup_centroid_quad();
  run_centroid_pipeline();
  check_blit_texture(0.025f, 0);
  delete_ff(_quad);

  // Test2
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.5));
  setup_centroid_quad();
  run_centroid_pipeline();
  check_blit_texture(0.05f, 0);
  delete_ff(_quad);

  // Test2b
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.6));
  setup_centroid_quad();
  run_centroid_pipeline();
  check_blit_texture(0.05f, 0);
  delete_ff(_quad);

  // Test3
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0.75));
  setup_centroid_quad();
  run_centroid_pipeline();
  check_blit_texture(0.075f, 0);
  delete_ff(_quad);

  // Test4
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 1.0));
  setup_centroid_quad();
  run_centroid_pipeline();
  check_blit_texture(0.1f, 0);
  delete_ff(_quad);
}

TestMultiSampling::~TestMultiSampling() {
  delete_ff(_pipeline);
  delete_ff(_centroid_pipeline);
  delete_ff(_input_texture);
  delete_ff(_fbo);
  delete_ff(_depth_stencil_rbo);
  delete_ff(_color_rbo);
  delete_ff(_blit_fbo);
  delete_ff(_blit_texture);

  delete_ff(_setup_pubo);
  delete_ff(_setup_ubo);
}

void TestMultiSampling::setup_multisample_fbo() {
  // RenderBuffers.
  _depth_stencil_rbo = new_ff RenderBuffer(GL_DEPTH32F_STENCIL8, 512, 512, 4);
  _color_rbo = new_ff RenderBuffer(GL_RGBA32F, 512, 512, 4);

  // FrameBuffer.
  _fbo = new_ff FrameBuffer;
  _fbo->bind();

  // Attach the depth and stencil rbo.
  _fbo->attach_render_buffer(GL_DEPTH_ATTACHMENT, *_depth_stencil_rbo);
  _fbo->attach_render_buffer(GL_STENCIL_ATTACHMENT, *_depth_stencil_rbo);

  // Get the frag data location.
  const FragDataInfo* frag_data_info = _pipeline->get_frag_data_infos()->get_frag_data_info("output_color");
  assert(frag_data_info);

  // Attach the color_rbo at frag data location.
  GLenum attachment_point = FrameBuffer::get_color_attachment(frag_data_info->location);
  _fbo->attach_render_buffer(attachment_point, *_color_rbo);
}

void TestMultiSampling::setup_blit_fbo() {
  // Blit fbo.
  _blit_fbo = new_ff FrameBuffer;
  _blit_fbo->bind();

  // Get the frag data location.
  const FragDataInfo* frag_data_info = _pipeline->get_frag_data_infos()->get_frag_data_info("output_color");
  assert(frag_data_info);

  // Create the blit texture.
  _blit_texture = new_ff Texture(0, FloatElement, 512, 512, 4, false);

  // Attach the blit texture at the frag data location also.
  GLenum attachment_point = FrameBuffer::get_color_attachment(
      frag_data_info->location);
  _blit_fbo->attach_texture(attachment_point, *_blit_texture, 0);
}

void TestMultiSampling::setup_quad() {
  GLuint position_index = _pipeline->get_attribute_infos()->get_attribute_location("position");
  _quad->feed_positions_to_attr(position_index);

  GLuint tex_coord_index = _pipeline->get_attribute_infos()->get_attribute_location("tex_coord");
  _quad->feed_tex_coords_to_attr(tex_coord_index);
}

void TestMultiSampling::setup_pipeline() {
  // Input texture.
  _input_texture = create_bar_texture(FloatElement, 512, 512, 1, 4, false);
  check_bar_texture(_input_texture, 1, 0);

  std::string vertex_shader = es3_model_view_project_vert;
  std::string fragment_shader = es3_multisampling_frag;

  // Pipeline.
  _pipeline = new_ff Pipeline(vertex_shader,fragment_shader);

  // Setup uniform matrices.
  glm::mat4 projection;
  glm::mat4 model_view = glm::mat4(1.0);
  glm::mat4 model_view_projection = glm::mat4(1.0);

  projection = glm::ortho(0.0f, 512.0f, 0.0f, 512.0f, -1000.0f, 1000.0f);
  model_view_projection = projection * model_view;

  // Setup viewport.
  glViewport(0, 0, 512, 512);

  // Setup float pipeline.
  _pipeline->use();
//  _pipeline->set_uniform("model_view_projection", &model_view_projection[0][0]);
//  int tex_unit(1);
//  _input_texture->bind(1);
//  _pipeline->set_uniform("image_texture", &tex_unit);
//  _pipeline->update_gl();

  // Setup our default uniforms.
  int tex_unit = 1;
  _input_texture->bind(1);
  _image_texture_info = _pipeline->get_default_uniform_infos()->get_uniform_info("image_texture");
  _pipeline->get_default_uniform_infos()->set_uniform(_image_texture_info, &tex_unit);

  // Setup our block uniforms.
  const BlockUniformInfos* bui = _pipeline->get_block_uniform_infos().at(0);
  _model_view_projection_info = bui->get_uniform_info("model_view_projection");
  _tint_color_info = bui->get_uniform_info("tint_color");

  _setup_pubo = new_ff PackedUniformBuffer(bui->get_byte_size());
  _setup_ubo = new_ff UniformBuffer();
  _setup_pubo->associate_uniform_buffers(_setup_ubo);

  _setup_pubo->set_uniform(_model_view_projection_info,&model_view_projection[0][0]);
  glm::vec4 tint_color(1);
  _setup_pubo->set_uniform(_tint_color_info, &tint_color[0]);

  // Unpack the uniforms.
  _setup_ubo->load_changed(_setup_pubo);

  // Setup the block bindings between the program and unpacked uniforms.
  GLint binding = 0; //block_index;
  _pipeline->get_program()->set_uniform_block_binding(0, binding);
  _setup_ubo->bind_to_uniform_block_binding(binding);

}

void TestMultiSampling::check_sample_positions() {
  GLint num_samples;
  glGetIntegerv(GL_SAMPLES, &num_samples);

  assert(num_samples == 4);

  glm::vec2 a, b, c, d;
//  glGetMultisamplefv(GL_SAMPLE_POSITION, 0, &a[0]);
//  glGetMultisamplefv(GL_SAMPLE_POSITION, 1, &b[0]);
//  glGetMultisamplefv(GL_SAMPLE_POSITION, 2, &c[0]);
//  glGetMultisamplefv(GL_SAMPLE_POSITION, 3, &d[0]);
//
//  assert(a == glm::vec2(0.375, 0.125));
//  assert(b == glm::vec2(0.875, 0.375));
//  assert(c == glm::vec2(0.125, 0.625));
//  assert(d == glm::vec2(0.625, 0.875));
}

void TestMultiSampling::run_pipeline() {
  // Bind fbo.
  _fbo->bind();
  assert(_fbo->is_complete());

  // Clear buffers.
  gpu(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
  gpu(glClearDepthf(1.0f));
  gpu(glClear (GL_COLOR_BUFFER_BIT));
  gpu(glClear (GL_DEPTH_BUFFER_BIT));
  gpu(glDisable(GL_CULL_FACE));

  // Draw.
  _quad->draw();

  // Unbind fbo.
  _fbo->unbind();

  // Blit to the blit fbo.
  glm::ivec2 min(0, 0);
  glm::ivec2 dim(512, 512);
  FrameBuffer::blit(_fbo, min, dim, _blit_fbo, min, dim);
}

void TestMultiSampling::check_blit_texture(float first_row_value,
                                           float second_row_value) {
  int width = _blit_texture->get_chunk_geometry().get_width();
  int height = _blit_texture->get_chunk_geometry().get_height();
  int num_channels = _blit_texture->get_chunk_geometry().get_num_channels();
  size_t element_size = _blit_texture->get_chunk_geometry().get_element_size_bytes();

  // Download the texture to a packed texture.
  _blit_texture->bind(0);
  PackedTexture* packed_texture = _blit_texture->create_packed_texture();
  _blit_texture->unbind(0);

  // Check the first row of the blit texture.
  {
    char* raw = packed_texture->get_pixel(0, 0, 0);
    float* element = reinterpret_cast<float*>(raw);
    for (int j = 0; j < width * num_channels; j++) {
      if (*element != first_row_value) {
        std::cerr << "Error: unexpected contents in input texture.\n";
        assert(false);
      }
      ++element;
    }
  }

  // Check the second row of the blit texture.
  {
    char* raw = packed_texture->get_pixel(0, 1, 0);
    float* element = reinterpret_cast<float*>(raw);
    for (int j = 0; j < width * num_channels; j++) {
      if (*element != second_row_value) {
        std::cerr << "Error: unexpected contents in input texture.\n";
        assert(false);
      }
      ++element;
    }
  }

  // Cleanup packed texture.
  delete_ff(packed_texture)
  ;
}

void TestMultiSampling::setup_centroid_quad() {
  // Setup quad vertex attributes.
  GLuint position_index = _centroid_pipeline->get_attribute_infos()->get_attribute_location("position");
  _quad->feed_positions_to_attr(position_index);

  GLuint tex_coord_index = _centroid_pipeline->get_attribute_infos()->get_attribute_location("tex_coord");
  _quad->feed_tex_coords_to_attr(tex_coord_index);
}

void TestMultiSampling::setup_centroid_pipeline() {
  std::string vertex_shader = es3_centroid_model_view_project_vert;
  std::string fragment_shader = es3_centroid_multisampling_frag;

  // Pipeline.
  _centroid_pipeline = new_ff Pipeline(vertex_shader,fragment_shader);

  // Calculate the uniform matrices.
  glm::mat4 projection;
  glm::mat4 model_view = glm::mat4(1.0);
  glm::mat4 model_view_projection = glm::mat4(1.0);

  projection = glm::ortho(0.0f, 512.0f, 0.0f, 512.0f, -1000.0f, 1000.0f);
  model_view_projection = projection * model_view;

  // Setup viewport.
  glViewport(0, 0, 512, 512);

  // Setup float pipeline.
  _centroid_pipeline->use();
  _setup_pubo->set_uniform(_model_view_projection_info, &model_view_projection[0][0]);
  int tex_unit(1);
  _input_texture->bind(1);
  _centroid_pipeline->get_default_uniform_infos()->set_uniform(_image_texture_info, &tex_unit);

  _setup_ubo->load_changed(_setup_pubo);

  // Update the fbo and blit_fbo settings.

  // Get the frag data location.
  const FragDataInfo* frag_data_info = _centroid_pipeline->get_frag_data_infos()->get_frag_data_info("output_color");
  assert(frag_data_info);

  // Attach the color_rbo at frag data location.
  GLenum attachment_point = FrameBuffer::get_color_attachment( frag_data_info->location);
  _fbo->bind();
  _fbo->attach_render_buffer(attachment_point, *_color_rbo);

  // Attach the blit texture at the frag data location also.
  _blit_fbo->bind();
  _blit_fbo->attach_texture(attachment_point, *_blit_texture, 0);
}

void TestMultiSampling::run_centroid_pipeline() {
  // Bind fbo.
  _fbo->bind();
  assert(_fbo->is_complete());

  // Clear the buffers.
  gpu(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
  gpu(glClearDepthf(1.0f));
  gpu(glClear (GL_COLOR_BUFFER_BIT));
  gpu(glClear (GL_DEPTH_BUFFER_BIT));
  gpu(glDisable(GL_CULL_FACE));

  // Draw.
  _quad->draw();

  // Unbind fbo.
  _fbo->unbind();

  // Blit to the blit fbo.
  glm::ivec2 min(0, 0);
  glm::ivec2 dim(512, 512);
  FrameBuffer::blit(_fbo, min, dim, _blit_fbo, min, dim);
}

}

#endif
