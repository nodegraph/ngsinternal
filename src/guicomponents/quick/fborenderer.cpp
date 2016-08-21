#include <base/device/deviceheadersgl.h>
#include <base/memoryallocator/taggednew.h>

#include <base/objectmodel/deploader.h>


#include <base/device/devicedebug.h>
#include <base/glewhelper/glewhelper.h>
#include <base/device/pipelinesetups/quadpipelinesetup.h>
#include <base/device/program/fragdatainfo.h>
#include <base/device/program/fragdatainfos.h>

#include <base/device/program/pipeline.h>

#include <base/device/unpackedbuffers/renderbuffer.h>
#include <base/device/unpackedbuffers/framebuffer.h>
#include <base/device/unpackedbuffers/texture.h>
#include <base/objectmodel/entity.h>
#include <components/compshapes/nodeselection.h>
#include <utility>
#include <cassert>
#include <components/resources/resources.h>

#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/nodegraphquickitemglobals.h>
#include <QtCore/QDebug>

namespace ngs {

FBORenderer::FBORenderer(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _render_rbo(NULL),
      _render_fbo(NULL),
      _render_texture(NULL),
      _display_rbo(NULL),
      _display_fbo(NULL),
      _display_texture(NULL),
      _ng_canvas(this) {
  get_dep_loader()->register_fixed_dep(_ng_canvas, ".");
}

FBORenderer::~FBORenderer() {
}

void FBORenderer::update_state() {
  //render();
}

GLuint FBORenderer::get_display_texture_name() {
  return _display_texture->get_name();
}

GLuint FBORenderer::get_render_texture_name() {
  return _render_texture->get_name();
}

GLsizei FBORenderer::get_texture_width() {
  return _display_texture->get_chunk_geometry().get_width();
}

GLsizei FBORenderer::get_texture_height() {
  return _display_texture->get_chunk_geometry().get_height();
}

void FBORenderer::initialize_gl() {
  // Create and setup two fbos.
  setup_fbo();
  swap_buffers();
  setup_fbo();

  _render_fbo->unbind();
  _display_fbo->unbind();
}

void FBORenderer::uninitialize_gl() {
  delete_ff(_render_rbo);
  delete_ff(_render_fbo);
  delete_ff(_render_texture);
  delete_ff(_display_rbo);
  delete_ff(_display_fbo);
  delete_ff(_display_texture);
}

bool FBORenderer::is_initialized_gl() {
  if (_render_rbo) {
    return true;
  }
  return false;
}

void FBORenderer::setup_fbo() {
  const ViewportParams &viewport = _ng_canvas->get_current_interaction()->get_viewport_params();

  // RenderBuffer.
  //render_rbo_ = new_ff RenderBuffer(GL_DEPTH32F_STENCIL8, viewport.width, viewport.height);
  _render_rbo = new_ff RenderBuffer(GL_DEPTH_COMPONENT16, viewport.width, viewport.height);

  // FrameBuffer.
  _render_fbo = new_ff FrameBuffer;
  _render_fbo->bind();
  //_render_fbo->attach_render_buffer(GL_DEPTH_ATTACHMENT_EXT, *_render_fbo);
  _render_fbo->attach_render_buffer(GL_DEPTH_ATTACHMENT, *_render_rbo);
  //_render_fbo->attach_render_buffer(GL_STENCIL_ATTACHMENT_EXT, *_render_fbo);
  assert(_render_fbo->is_complete());

  // Create the output texture.
  _render_texture = new_ff Texture(0, UCharElement, viewport.width, viewport.height, 4, true);
  _render_texture->unbind(0);

#if GLES_MAJOR_VERSION <= 2
//  frag_data_info = _ng_canvas->get_pipeline()->get_frag_data_info("gl_FragColor");
//  assert(frag_data_info);
  _render_fbo->attach_texture(GL_COLOR_ATTACHMENT0, *_render_texture, 0);
#else
  // Get the frag data location.
  const FragDataInfo* frag_data_info = NULL;
  frag_data_info = _ng_canvas->get_pipeline()->get_frag_data_info("output_color");
//  assert(frag_data_info);
//  // Attach the output texture at frag data location.
//  GLenum attachment_point = FrameBuffer::get_color_attachment(frag_data_info->location);
//  _render_fbo->attach_texture(attachment_point, *_render_texture, 0);

  _render_fbo->attach_texture(GL_COLOR_ATTACHMENT0, *_render_texture, 0);
#endif

}


void FBORenderer::resize_gl(GLsizei width, GLsizei height) {

  _ng_canvas->get_current_interaction()->resize_gl(width,height);

  _render_rbo->bind();
  _render_rbo->resize(width, height);
  _render_texture->bind(0);
  _render_texture->reallocate(width, height, 1, 4);

  _display_rbo->bind();
  _display_rbo->resize(width, height);
  _display_texture->bind(0);
  _display_texture->reallocate(width, height, 1, 4);

}


void FBORenderer::draw() {
  _ng_canvas->clean();
  _ng_canvas->draw_gl();
  // Make sure that all the gl commands have flushed threw.
  glFlush();
}

void FBORenderer::render() {
  const ViewportParams& p = _ng_canvas->get_current_interaction()->get_viewport_params();
  if (p.is_empty()) {
    return;
  }

  glViewport(p.x, p.y, p.width, p.height);

	if (p.width != get_texture_width() || p.height != get_texture_height()) {
	  resize_gl(p.width, p.height);
	}

  // Bind the fbo.
  _render_fbo->bind();
  assert(_render_fbo->is_complete());
  // Draw the geometry.
  draw();
  // Unbind the fbo.
  _render_fbo->unbind();
}

void FBORenderer::swap_buffers() {
  // Swap the rendering buffers.
  std::swap(_render_rbo, _display_rbo);
  std::swap(_render_fbo, _display_fbo);
  std::swap(_render_texture, _display_texture);
}




}
