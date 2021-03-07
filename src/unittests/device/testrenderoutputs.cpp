#include "testrenderoutputs.h"
#include <base/memoryallocator/taggednew.h>

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
#include <base/device/program/pipeline.h>
#include <base/device/program/fragdatainfos.h>
#include <base/device/program/attributeinfos.h>
#include <base/device/devicedebug.h>

#include <base/device/transforms/glmhelper.h>
#include <base/device/packedbuffers/packeduniformbuffer.h>
#include <base/device/program/blockuniforminfos.h>
#include <base/device/program/defaultuniforminfos.h>
#include <base/device/program/fragdatainfo.h>
#include <base/utils/fileutil.h>
#include <glm/gtc/matrix_transform.hpp>
#include <components/resources/resources.h>


namespace ngs {

#if GLES_MAJOR_VERSION >= 3

TestRenderOutputs::TestRenderOutputs(ElementID element_id,
		bool normalized_access) :
		_element_id(element_id), _normalized_access(normalized_access), _pipeline(
				NULL), _rbo(NULL), _fbo(NULL), _input_texture(NULL), _output_texture(
				NULL), _quad(512, 512, -100, glm::vec2(0, 0)) {
	setup_pipeline();
	setup_fbo();
	run_pipeline();
	check_bar_texture(_output_texture, 2, 1);
}

TestRenderOutputs::~TestRenderOutputs() {
  delete_ff(_pipeline);
  delete_ff(_rbo);
  delete_ff(_fbo);
  delete_ff(_input_texture);
  delete_ff(_output_texture);

  delete_ff(_setup_pubo);
  delete_ff(_setup_ubo);
}

// -------------------------------------------------------------------------------
// Vertex shader.
// -------------------------------------------------------------------------------

std::string TestRenderOutputs::get_vertex_shader() {
  return es3_model_view_project_vert;
}

// -------------------------------------------------------------------------------
// Fragment shader.
// -------------------------------------------------------------------------------

std::string TestRenderOutputs::get_fragment_shader() {
  switch (_element_id) {
    case FloatElement:
      // Floats are always accessed as floating point.
      return es3_floating_texture_value_frag;
      break;
    case HalfElement:
      // Floats are always accessed as floating point.
      return es3_floating_texture_value_frag;
      break;
    case IntElement:
      // Ints are always accessed as ints.
      return es3_int_texture_value_frag;
      break;
    case UIntElement:
      // Unsigned ints are always accessed as ints.
      return es3_uint_texture_value_frag;
      break;
    case ShortElement:
      if (_normalized_access) {
        return es3_floating_texture_value_frag;
      } else {
        return es3_int_texture_value_frag;
      }
      break;
    case UShortElement:
      if (_normalized_access) {
        return es3_floating_texture_value_frag;
      } else {
        return es3_uint_texture_value_frag;
      }
      break;
    case CharElement:
      if (_normalized_access) {
        return es3_floating_texture_value_frag;
      } else {
        return es3_int_texture_value_frag;
      }
      break;
    case UCharElement:
      if (_normalized_access) {
        return es3_floating_texture_value_frag;
      } else {
        return es3_uint_texture_value_frag;
      }
      break;
    default:
      assert(false && "unknown element type encountered\n");
      return "";
  }
  return "";
}


// -------------------------------------------------------------------------------
// Tint color.
// -------------------------------------------------------------------------------


glm::vec4 TestRenderOutputs::get_tint_color() {
	switch(_element_id) {
	case ShortElement:{
		  glm::vec4 tint_color(1.0f, 1.0f, 1.0f, 1.0f);
		  if (_normalized_access) {
		    float one = 2.0f / powf(2.0f, 2 * 8);  // Multiply by 2.0 because I can't seem to render to gl_rgba8_snorm textures.
		    tint_color[0] = one;
		    tint_color[1] = one;
		    tint_color[2] = one;
		    tint_color[3] = one;
		  }
		  return tint_color;
	}
	case UShortElement: {
		  glm::vec4 tint_color(1.0f, 1.0f, 1.0f, 1.0f);
		  if (_normalized_access) {
		    float one = 1.0f / powf(2.0f, 2 * 8);
		    tint_color[0] = one;
		    tint_color[1] = one;
		    tint_color[2] = one;
		    tint_color[3] = one;
		  }
		  return tint_color;
	}
	case CharElement: {
		  glm::vec4 tint_color(1.0f, 1.0f, 1.0f, 1.0f);
		  if (_normalized_access) {
		    float one = 2.0f / powf(2.0f, 1 * 8);  // Multiply by 2.0 because I can't seem to render to gl_rgba8_snorm textures.
		    tint_color[0] = one;
		    tint_color[1] = one;
		    tint_color[2] = one;
		    tint_color[3] = one;
		  }
		  return tint_color;
	}
	case UCharElement: {
		  glm::vec4 tint_color(1.0f, 1.0f, 1.0f, 1.0f);
		  if (_normalized_access) {
		    float one = 1.0f / powf(2.0f, 1 * 8);
		    tint_color[0] = one;
		    tint_color[1] = one;
		    tint_color[2] = one;
		    tint_color[3] = one;
		  }
		  return tint_color;
	}
	default: {
		// Covers float, int, and unsigned int cases.
		return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	}
}

// -------------------------------------------------------------------------------
// Core logic.
// -------------------------------------------------------------------------------

void TestRenderOutputs::setup_pipeline() {
  // Create the input texture.
  _input_texture = create_bar_texture(_element_id, 512, 512, 1, 4, _normalized_access);
  check_bar_texture(_input_texture, 1, 0);

  std::string vertex_shader;
  std::string fragment_shader;
  vertex_shader = get_vertex_shader();
  fragment_shader = get_fragment_shader();

  // Pipeline.
  _pipeline = new_ff Pipeline(vertex_shader,fragment_shader);

  // Setup the uniform matrices.
  glm::mat4 projection;
  glm::mat4 model_view = glm::mat4(1.0);
  glm::mat4 model_view_projection = glm::mat4(1.0);

  projection = glm::ortho(0.0f, 512.0f, 0.0f, 512.0f, -1000.0f, 1000.0f);
  model_view_projection = projection * model_view;

  // Setup viewport.
  glViewport(0, 0, 512, 512);

  // Setup additional uniforms.
  glm::vec4 tint_color = get_tint_color();

  // Setup float pipeline.
  _pipeline->use();
//  _pipeline->set_uniform("model_view_projection", &model_view_projection[0][0]);
//  _pipeline->set_uniform("tint_color", &tint_color[0]);
//  int tex_unit(1);
//  _input_texture->bind(1);
//  _pipeline->set_uniform("image_texture", &tex_unit);
//  _pipeline->update_gl();

  // Setup quad vertex attributes.
  GLuint position_index = _pipeline->get_attribute_infos()->get_attribute_location("position");
  _quad.feed_positions_to_attr(position_index);

  GLuint tex_coord_index = _pipeline->get_attribute_infos()->get_attribute_location("tex_coord");
  _quad.feed_tex_coords_to_attr(tex_coord_index);

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
  _setup_pubo->set_uniform(_tint_color_info, &tint_color[0]);

  // Unpack the uniforms.
  _setup_ubo->load_changed(_setup_pubo);

  // Setup the block bindings between the program and unpacked uniforms.
  GLint binding = 0; //block_index;
  _pipeline->get_program()->set_uniform_block_binding(0, binding);
  _setup_ubo->bind_to_uniform_block_binding(binding);

}

void TestRenderOutputs::setup_fbo() {
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
  _output_texture = new_ff Texture(0, _element_id, 512, 512, 4, _normalized_access);
  _output_texture->unbind(0);

  // Attach the output texture at frag data location.
  GLenum attachment_point = FrameBuffer::get_color_attachment(frag_data_info->location);
  _fbo->attach_texture(attachment_point, *_output_texture, 0);
}

void TestRenderOutputs::run_pipeline() {
  // Bind the fbo.
  _fbo->bind();
  assert(_fbo->is_complete());

  // Clear the buffers.
  gpu(glClearColor(0.1f, 0.2f, 0.3f, 0.4f));
  gpu(glClearDepthf(1.0f));
  gpu(glClear (GL_COLOR_BUFFER_BIT));
  gpu(glClear (GL_DEPTH_BUFFER_BIT));
  gpu(glDisable(GL_CULL_FACE));

  // Draw the quad.
  _quad.draw();

  // Unbind the fbo.
  _fbo->unbind();
}

#endif
}
