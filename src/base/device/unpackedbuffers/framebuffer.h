#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>

#include <vector>

namespace ngs {

class Texture;
class RenderBuffer;

// Attachment Points:
// GL_COLOR_ATTACHMENTi
// GL_DEPTH_ATTACHMENT
// GL_STENCIL_ATTACHMENT
// GL_DEPTH_STENCIL_ATTACHMENT

class DEVICE_EXPORT FrameBuffer {
 public:
  FrameBuffer();
  virtual ~FrameBuffer();

  GLuint get_name();
  GLenum get_target();

  void bind();
  void unbind();

#if GLES_MAJOR_VERSION >= 3
  static void blit(FrameBuffer* src, glm::ivec2 src_min, glm::ivec2 src_dim,
                   FrameBuffer* dest, glm::ivec2 dest_min, glm::ivec2 dest_dim);
#endif

  // Attach render buffers.
  void attach_render_buffer(GLenum attachment_point, RenderBuffer& obj);

  // Attach one image from a texture. 
  void attach_texture(GLenum attachment_point, Texture& obj, GLint level,
                      GLint layer = 0);

  // Attach a whole layer of images.  Eg all faces of a cube map at mipmap level 3.
  // This can only be used with geometry shaders.
  void attach_texture_layers(GLenum attachment_point, Texture& obj,
                             GLint level);

  // Check for completeness.
  bool is_complete();

  // Draw and read buffers.
#if GLES_MAJOR_VERSION >= 3
  void set_draw_buffers(int num);
#endif

  // Clear current attchments.
  void clear_attachments();

  static GLenum get_color_attachment(int index);

 private:
  void create_name();
  void remove_name();

  GLuint _name;
  GLenum _target;

#if GLES_MAJOR_VERSION >= 3
  static const GLenum _raw_attachment_points[16];
#elif GLES_MAJOR_VERSION >= 2
  static const GLenum _raw_attachment_points[1];
#endif
  static const std::vector<GLenum> _attachment_points;
};

}
