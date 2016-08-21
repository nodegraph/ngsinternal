#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <base/device/devicebasictypesgl.h>
#include <base/device/transforms/lens.h>
#include <base/device/transforms/trackball.h>

class QSGTexture;

namespace ngs {

// Device.
class RenderBuffer;
class FrameBuffer;
class Texture;

// ObjectModel.
class Entity;
class ShapeCanvas;

class QUICK_EXPORT FBORenderer: public Component {
 public:

  COMPONENT_ID(FBORenderer, FBORenderer);

  FBORenderer(Entity* entity);
  virtual ~FBORenderer();

  // Our state.
  virtual void update_state();
  virtual void initialize_gl();
  virtual void uninitialize_gl();
  virtual bool is_initialized_gl();

  void resize_gl(int width, int height);
  void render();

  GLuint get_display_texture_name();
  GLuint get_render_texture_name();

  void swap_buffers();

 private:
  void setup_fbo();
  void draw();

  GLsizei get_texture_width();
  GLsizei get_texture_height();

  void load_demo();
  void add_test_nodes();

  // The texture to render to.
  RenderBuffer* _render_rbo;
  FrameBuffer* _render_fbo;
  Texture* _render_texture;

  // The texture to display from.
  RenderBuffer* _display_rbo;
  FrameBuffer* _display_fbo;
  Texture* _display_texture;

  // Our fixed deps.
  Dep<ShapeCanvas> _ng_canvas;
};

}
