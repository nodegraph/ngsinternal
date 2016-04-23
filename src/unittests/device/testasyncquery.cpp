
#if GLES_MAJOR_VERSION >= 3

#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/memoryallocator/taggednew.h>
#include "testasyncquery.h"

#include <base/device/geometry/quad.h>

#include <base/device/program/program.h>
#include <base/device/program/pipeline.h>
#include <base/device/program/fragdatainfo.h>
#include <base/device/program/fragdatainfos.h>
#include <base/device/program/blockuniforminfos.h>
#include <base/device/program/attributeinfos.h>

#include <base/device/packedbuffers/packeduniformbuffer.h>

#include <base/device/unpackedbuffers/renderbuffer.h>
#include <base/device/unpackedbuffers/framebuffer.h>
#include <base/device/unpackedbuffers/texture.h>
#include <base/device/unpackedbuffers/uniformbuffer.h>


#include <base/device/asyncquery.h>

#include <glm/gtc/matrix_transform.hpp>

#include <components/resources/resources.h>


#include <iostream>

namespace ngs {

TestAsyncQuery::TestAsyncQuery() {
  _quad = new_ff Quad(512, 512, -100, glm::vec2(0, 0));
  //_query = new_ff AsyncQuery(GL_SAMPLES_PASSED);
  _query = new_ff AsyncQuery(GL_ANY_SAMPLES_PASSED);

  setup_pipeline();
  setup_fbo();
  run_pipeline();
}

TestAsyncQuery::~TestAsyncQuery() {
  delete_ff(_quad);
  delete_ff(_query);
  delete_ff(_pipeline);
  delete_ff(_rbo);
  delete_ff(_fbo);
  delete_ff(_output_texture);

  delete_ff(_setup_pubo);
  delete_ff(_setup_ubo);
}

void TestAsyncQuery::setup_pipeline() {
  std::string vertex_shader = es3_model_view_project_vert;
  std::string fragment_shader = es3_floating_texture_value_frag;

  // Pipeline. Any fragment shader will because we're just testing occlusion queries.
  _pipeline = new_ff Pipeline(vertex_shader,fragment_shader);

  // Setup the uniform matrices.
  glm::mat4 model_view = glm::mat4(1.0);
  glm::mat4 projection = glm::ortho(0.0f, 512.0f, 0.0f, 512.0f, -1000.0f,
                                    1000.0f);
  glm::mat4 model_view_projection = projection * model_view;

  // Setup viewport.
  glViewport(0, 0, 512, 512);

  glm::vec4 tint_color(1.0f, 1.0f, 1.0f, 1.0f);

  // Setup float pipeline.
  _pipeline->use();
//  _pipeline->set_uniform("model_view_projection", &model_view_projection[0][0]);
//  _pipeline->set_uniform("tint_color", &tint_color[0]);
//  _pipeline->update_gl();

  // Setup vertex attributes.
  GLuint position_index = _pipeline->get_attribute_infos()->get_attribute_location("position");
  _quad->feed_positions_to_attr(position_index);

  GLuint tex_coord_index = _pipeline->get_attribute_infos()->get_attribute_location("tex_coord");
  _quad->feed_tex_coords_to_attr(tex_coord_index);

  // Setup our block uniforms.
  BlockUniformInfos* bui = _pipeline->get_block_uniform_infos().at(0);
  _model_view_projection_info = bui->get_uniform_info("model_view_projection");
  _tint_color_info = bui->get_uniform_info("tint_color");

  _setup_pubo = new_ff PackedUniformBuffer(bui->get_byte_size());
  _setup_ubo = new_ff UniformBuffer();
  _setup_pubo->associate_uniform_buffers(_setup_ubo);

  _setup_pubo->set_uniform(_model_view_projection_info,&model_view_projection[0][0]);
  _setup_pubo->set_uniform(_tint_color_info, &tint_color[0]);

  // Unpack the uniforms.
  _setup_ubo->load_changed(_setup_pubo);

  // Setup the block bindings between the program and unpacked uniforms.
  GLint binding = 0; //block_index;
  _pipeline->get_program()->set_uniform_block_binding(0, binding);
  _setup_ubo->bind_to_uniform_block_binding(binding);

}

void TestAsyncQuery::setup_fbo() {
  // RenderBuffer.
  _rbo = new_ff RenderBuffer(GL_DEPTH32F_STENCIL8, 512, 512);

  // FrameBuffer.
  _fbo = new_ff FrameBuffer;
  _fbo->bind();
  _fbo->attach_render_buffer(GL_DEPTH_ATTACHMENT, *_rbo);
  _fbo->attach_render_buffer(GL_STENCIL_ATTACHMENT, *_rbo);

  // Get the frag data location.
  const FragDataInfo* frag_data_info = _pipeline->get_frag_data_infos()->get_frag_data_info("output_color");
  assert(frag_data_info);

  // Create the output texture.
  _output_texture = new_ff Texture(0, FloatElement, 512, 512, 4, true);
  _output_texture->unbind(0);

  // Attach the output texture at frag data location.
  GLenum attachment_point = FrameBuffer::get_color_attachment( frag_data_info->location);
  _fbo->attach_texture(attachment_point, *_output_texture, 0);
}

void TestAsyncQuery::run_pipeline() {
  // Bind the fbo.
  _fbo->bind();
  assert(_fbo->is_complete());

  // Make sure depth testing is enabled.
  glEnable(GL_DEPTH_TEST);

  // Clear the buffers.
  gpu(glClearColor(0.1f, 0.2f, 0.3f, 0.4f));
  gpu(glClearDepthf(1.0f));
  gpu(glClear (GL_COLOR_BUFFER_BIT));
  gpu(glClear (GL_DEPTH_BUFFER_BIT));
  gpu(glDisable(GL_CULL_FACE));

  AsyncQuery::enable_color_and_depth_writes();

  // First pass initializes the depth buffer.
  {
    _query->bind();
    _quad->draw();
    _query->unbind();

    // Check that 512x512=262144 fragments pass the depth test.
    while (!_query->resultIsReady()) {
    }
    int num_passed_samples = _query->get_result();
    assert(num_passed_samples == 262144);
  }

  AsyncQuery::disable_color_and_depth_writes();

  // Second pass. We draw the quad again.
  {
    _query->bind();
    _quad->draw();
    _query->unbind();

    // Check that 0 fragments pass the depth test.
    while (!_query->resultIsReady()) {
    }
    int num_passed_samples = _query->get_result();
    assert(num_passed_samples == 0);
  }

  AsyncQuery::enable_color_and_depth_writes();

  // Clear the depth buffer.
  gpu(glClearDepthf(1.0f));
  gpu(glClear (GL_DEPTH_BUFFER_BIT));

  // First pass. Initialize depth buffer with a quad shifted one pixel to the right.
  {
    // Setup the uniform matrices.
    glm::mat4 model_view = glm::mat4(1.0);
    model_view[3][0] = 1;  // Shift one pixel to the right.
    glm::mat4 projection = glm::ortho(0.0f, 512.0f, 0.0f, 512.0f, -1000.0f,
                                      1000.0f);
    glm::mat4 model_view_projection = projection * model_view;

    // Setup float pipeline.
    _pipeline->use();
    _setup_pubo->set_uniform(_model_view_projection_info, &model_view_projection[0][0]);
    _setup_ubo->load_changed(_setup_pubo);

    _query->bind();
    _quad->draw();
    _query->unbind();

    // Check that 511x512=261632 fragments pass the depth test.
    while (!_query->resultIsReady()) {
    }
    int num_passed_samples = _query->get_result();
    assert(num_passed_samples == 261632);
  }

  AsyncQuery::disable_color_and_depth_writes();

  // Second. Draw the quad in the original position.
  {
    // Setup the uniform matrices.
    glm::mat4 model_view = glm::mat4(1.0);
    glm::mat4 projection = glm::ortho(0.0f, 512.0f, 0.0f, 512.0f, -1000.0f,
                                      1000.0f);
    glm::mat4 model_view_projection = projection * model_view;

    // Setup float pipeline.
    _pipeline->use();
    _setup_pubo->set_uniform(_model_view_projection_info, &model_view_projection[0][0]);
    _setup_ubo->load_changed(_setup_pubo);

    _query->bind();
    _quad->draw();
    _query->unbind();

    // Check that 512 fragments pass the depth test.
    while (!_query->resultIsReady()) {
    }
    int num_passed_samples = _query->get_result();
    assert(num_passed_samples == 512);
  }

  AsyncQuery::enable_color_and_depth_writes();

  // Unbind the fbo.
  _fbo->unbind();
}

}

#endif
