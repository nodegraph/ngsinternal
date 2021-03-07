#include <base/memoryallocator/taggednew.h>

#include <base/device/devicedebug.h>
#include <base/glewhelper/glewhelper.h>
#include <base/objectmodel/entity.h>
#include <components/interactions/shapecanvas.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/texturedisplaynode.h>
#include <guicomponents/quick/nodegraphview.h>

#include <QtQuick/QQuickView>
#include <QtQuick/QSGTexture>

// System.
#include <utility>
#include <iostream>

namespace ngs {

FBOWorker::FBOWorker(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _renderer(this),
      _ng_view(this),
      _display_texture_wrapper(NULL),
      _render_texture_wrapper(NULL) {
  qRegisterMetaType<QSGTexture*>();
  get_dep_loader()->register_fixed_dep(_renderer, Path());
  get_dep_loader()->register_fixed_dep(_ng_view, Path());
}

FBOWorker::~FBOWorker() {
}

bool FBOWorker::update_state() {
  internal();
  //render_next_texture();
  return true;
}

// This gets called from the main scene graph render thread.
void FBOWorker::initialize_gl() {
  internal();
  // Create our Qt texture wrappers.
  // The size doesn't need to be accurate.
  QSize size(1500, 1500);

  _display_texture_wrapper = _ng_view->create_texture_from_id(_renderer->get_display_texture_name(), size);
  _render_texture_wrapper = _ng_view->create_texture_from_id(_renderer->get_render_texture_name(), size);

  _display_texture_wrapper->setHorizontalWrapMode(QSGTexture::Repeat);
  _display_texture_wrapper->setVerticalWrapMode(QSGTexture::Repeat);

  _render_texture_wrapper->setHorizontalWrapMode(QSGTexture::Repeat);
  _render_texture_wrapper->setVerticalWrapMode(QSGTexture::Repeat);
}

void FBOWorker::uninitialize_gl() {
  internal();
  // Make sure the context is not current before deleting.
  delete_ff(_display_texture_wrapper);
  delete_ff(_render_texture_wrapper);
}

bool FBOWorker::is_initialized_gl_imp() const {
  internal();
  if (_display_texture_wrapper) {
    return true;
  }
  return false;
}

QSGTexture* FBOWorker::get_display_texture() {
  external();
  return _display_texture_wrapper;
}

void FBOWorker::render_next_texture() {
  external();
  _renderer->render();
}

QSGTexture* FBOWorker::swap_buffers() {
  external();
  _renderer->swap_buffers();

  // Swap our texture wrappers.
  std::swap(_render_texture_wrapper, _display_texture_wrapper);

  // Check that our texture wrappers match the renderers actual textures.
  assert(_display_texture_wrapper->textureId() == _renderer->get_display_texture_name());
  assert(_render_texture_wrapper->textureId() == _renderer->get_render_texture_name());

  // Return the current display wrapper.
  return _display_texture_wrapper;
}

const Dep<FBORenderer>& FBOWorker::get_renderer() const {
  external();
  return _renderer;
}

}
