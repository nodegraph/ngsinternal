#include <base/memoryallocator/taggednew.h>

#include <base/device/devicedebug.h>
#include <base/glewhelper/glewhelper.h>
#include <base/objectmodel/entity.h>
#include <components/compshapes/nodeselection.h>
#include <components/interactions/shapecanvas.h>
#include <gui/quick/fboworker.h>
#include <gui/quick/fborenderer.h>
#include <gui/quick/nodegraphquickitem.h>
#include <gui/quick/nodegraphquickitemglobals.h>
#include <gui/quick/texturedisplaynode.h>

// QtQuick.
#include <QtQuick/QQuickView>
#include <QtQuick/QSGTexture>

// System.
#include <utility>
#include <iostream>

namespace ngs {

FBOWorker::FBOWorker(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _renderer(this),
      _display_texture_wrapper(NULL),
      _render_texture_wrapper(NULL) {
  qRegisterMetaType<QSGTexture*>();
  get_dep_loader()->register_fixed_dep(_renderer, ".");
}

FBOWorker::~FBOWorker() {
}

void FBOWorker::update_state() {
  //render_next_texture();
}

// This gets called from the main scene graph render thread.
void FBOWorker::initialize_gl() {
  // Create our Qt texture wrappers.
  // The size doesn't need to be accurate.
  QSize size(1500, 1500);

  _display_texture_wrapper = g_quick_view->createTextureFromId(_renderer->get_display_texture_name(), size);
  _render_texture_wrapper = g_quick_view->createTextureFromId(_renderer->get_render_texture_name(), size);

  _display_texture_wrapper->setHorizontalWrapMode(QSGTexture::Repeat);
  _display_texture_wrapper->setVerticalWrapMode(QSGTexture::Repeat);

  _render_texture_wrapper->setHorizontalWrapMode(QSGTexture::Repeat);
  _render_texture_wrapper->setVerticalWrapMode(QSGTexture::Repeat);
}

void FBOWorker::uninitialize_gl() {
  // Make sure the context is not current before deleting.
  delete_ff(_display_texture_wrapper);
  delete_ff(_render_texture_wrapper);
}

bool FBOWorker::is_initialized_gl() {
  if (_display_texture_wrapper) {
    return true;
  }
  return false;
}

QSGTexture* FBOWorker::get_display_texture() {
  return _display_texture_wrapper;
}

void FBOWorker::render_next_texture() {
  _renderer->render();
}

QSGTexture* FBOWorker::swap_buffers() {
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
  return _renderer;
}

}
