#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/unpackedbuffers/framebuffer.h>
#include <base/device/unpackedbuffers/texture.h>
#include <base/device/unpackedbuffers/renderbuffer.h>



#include <iostream>

namespace ngs {

FrameBuffer::FrameBuffer() {
  create_name();
  _target = GL_FRAMEBUFFER;
}

FrameBuffer::~FrameBuffer() {
  remove_name();
}

GLuint FrameBuffer::get_name() {
  return _name;
}

GLenum FrameBuffer::get_target() {
  return _target;
}

void FrameBuffer::bind() {
  gpu(glBindFramebuffer(_target, _name));
}

void FrameBuffer::unbind() {
  gpu(glBindFramebuffer(_target, 0));
}

#if GLES_MAJOR_VERSION >= 3
// Static method.
void FrameBuffer::blit(FrameBuffer* src, glm::ivec2 src_min, glm::ivec2 src_dim,
                       FrameBuffer* dest, glm::ivec2 dest_min,
                       glm::ivec2 dest_dim) {
  // Make sure no fbos are currently bound.
  gpu(glBindFramebuffer(GL_FRAMEBUFFER, 0););

  // Bind the src fbo to the read target.
  gpu(glBindFramebuffer(GL_READ_FRAMEBUFFER, src->get_name()););

  // Bind the dest fbo to the draw target.
  gpu(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->get_name()););

  // Perform blitting.
  // Note the copy mask is currently fixed to the color buffer.
  // Also note that the filter is set to nearest.
  gpu(glBlitFramebuffer( src_min[0], src_min[1], src_dim[0], src_dim[1], dest_min[0], dest_min[1], dest_dim[0], dest_dim[1], GL_COLOR_BUFFER_BIT, GL_NEAREST););
}
#endif

void FrameBuffer::create_name() {
  gpu(glGenFramebuffers(1, &_name));
  if (_name == 0) {
    std::cerr
        << "Error: insufficient memory is likely. unable to create a framebuffer object name.\n";
    assert(false);
  }
}

void FrameBuffer::remove_name() {
  if (_name != 0) {
    gpu(glDeleteFramebuffers(1, &_name));
  }
  _name = 0;
}

void FrameBuffer::attach_render_buffer(GLenum attachment_point,
                                       RenderBuffer& obj) {
  gpu(glFramebufferRenderbuffer(_target, attachment_point, obj.get_target(), obj.get_name()););
}

void FrameBuffer::attach_texture(GLenum attachment_point, Texture& obj,
                                 GLint level, GLint layer) {
  GLenum unpacked_target = obj.get_unpacked_format().get_unpacked_target();
  switch (unpacked_target) {
#if GLES_MAJOR_VERSION >= 100
    case GL_TEXTURE_1D: {
      gpu(glFramebufferTexture1D(_target, attachment_point, unpacked_target, obj.get_name(), level););
    }
      break;
    case GL_TEXTURE_RECTANGLE: {
      gpu(glFramebufferTexture2D(_target, attachment_point, unpacked_target, obj.get_name(), level););
    }
      break;
    case GL_TEXTURE_BUFFER: {
      gpu(glFramebufferTexture2D(_target, attachment_point, unpacked_target, obj.get_name(), level););
    }
      break;

    case GL_TEXTURE_1D_ARRAY: {
      gpu(glFramebufferTextureLayer(_target, attachment_point, obj.get_name(), level, layer););
    }
      break;
    case GL_TEXTURE_2D_MULTISAMPLE: {
      gpu(glFramebufferTexture2D(_target, attachment_point, unpacked_target, obj.get_name(), level););
    }
      break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: {
      gpu(glFramebufferTextureLayer(_target, attachment_point, obj.get_name(), level, layer););
    }
      break;
#endif
#if GLES_MAJOR_VERSION >= 3
    case GL_TEXTURE_3D: {
      gpu(glFramebufferTextureLayer(_target, attachment_point, unpacked_target, level, layer););
    }
      break;
    case GL_TEXTURE_2D_ARRAY: {
      gpu(glFramebufferTextureLayer(_target, attachment_point, obj.get_name(), level, layer););
    }
      break;
#endif
#if GLES_MAJOR_VERSION >= 1
    case GL_TEXTURE_2D: {
      gpu(glFramebufferTexture2D(_target, attachment_point, unpacked_target, obj.get_name(), level););
    }
      break;
    case GL_TEXTURE_CUBE_MAP: {
      GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
      gpu(glFramebufferTexture2D(_target, attachment_point, target, obj.get_name(), level););
    }
      break;
#endif



    default: {
      std::cerr
          << "Error: framebuffer object encountered an unhandled texture target type: "
          << obj.get_unpacked_format().get_unpacked_target() << "\n";
    }
      break;
  }
}

void FrameBuffer::attach_texture_layers(GLenum attachment_point,
                                        Texture& obj, GLint level) {
#if GLES_MAJOR_VERSION >= 100
  gpu(glFramebufferTexture(_target, attachment_point, obj.get_name(), level););
#else
  GLenum texture_target = obj.get_unpacked_format().get_unpacked_target();
  gpu(glFramebufferTexture2D(_target, attachment_point, texture_target,obj.get_name(), level););
#endif
}

bool FrameBuffer::is_complete() {
  gpu(GLenum state=glCheckFramebufferStatus(_target););

  if (state == GL_FRAMEBUFFER_COMPLETE) {
    return true;
  }

  switch (state) {

    case GL_FRAMEBUFFER_UNSUPPORTED: {
      std::cerr << "The combination of attached images is not supported.\n";
    }
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
      std::cerr
          << "One or more attachments are not complete. See Attachment completeness rules.\n";
    }
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
      std::cerr << "There must be at least one attachment.\n";
    }
      break;

#if GLES_MAJOR_VERSION >= 3
    case GL_FRAMEBUFFER_UNDEFINED: {
      std::cerr << "The default frame buffer doesn't exists.\n";
    }
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: {
      std::cerr
          << "All images must have the same number of multisample samples.\n";
    }
      break;
#endif

#if GLES_MAJOR_VERSION >= 100
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: {
      std::cerr
          << "All draw buffers must specify attachment points that have images attached.\n";
    }
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: {
      std::cerr
          << "If the read buffer is set, then it must specify an attachment point that has an image attached.\n";
    }
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: {
      std::cerr
          << "If a layered image is attached to one attachment, then all attachments must be layered attachments. The attached layers do not have to have the same number of layers, nor do the layers have to come from the same kind of texture.\n";
    }
      break;
#endif
    default: {
      std::cerr
          << "Framebuffer object is incomplete due to an unknown GLenum state code: " << state << "\n";
    }
      break;

  }
  return false;
}

#if GLES_MAJOR_VERSION >= 3
void FrameBuffer::set_draw_buffers(int num) {
  gpu(glDrawBuffers(num, &_attachment_points[0]));
}
#endif

void FrameBuffer::clear_attachments() {
  for (int i = 0; i < _attachment_points.size(); i++) {
    gpu(glFramebufferTexture2D(_target,_attachment_points[i], GL_TEXTURE_2D, 0, 0));
  }
}

GLenum FrameBuffer::get_color_attachment(int index) {
  return _attachment_points[index];
}

#if GLES_MAJOR_VERSION >= 3
    const GLenum FrameBuffer::_raw_attachment_points[16] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
    GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12,
    GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15 };
#elif GLES_MAJOR_VERSION <= 2
    const GLenum FrameBuffer::_raw_attachment_points[1] = {
    GL_COLOR_ATTACHMENT0 };
#endif


#if GLES_MAJOR_VERSION >= 3
    const std::vector<GLenum> FrameBuffer::_attachment_points(_raw_attachment_points, _raw_attachment_points+16);
#elif GLES_MAJOR_VERSION <= 2
    const std::vector<GLenum> FrameBuffer::_attachment_points(_raw_attachment_points, _raw_attachment_points+1);
#endif


}

/*
 Attachment Completeness

 Each attachment point itself must be complete according to these rules. Empty attachments (attachments with no image attached) are complete by default. If an image is attached, it must adhere to the following rules:

 * The source object for the image still exists and has the same type it was attached with.
 * The image has a non-zero width and height.
 * The layer for 3D or depth textures attachments is less than the depth of the texture.
 * The image's format must match the attachment point's requirements, as defined above. Color-renderable formats for color attachments, etc.


 */

