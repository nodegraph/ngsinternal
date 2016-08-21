#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/device/devicebasictypesgl.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>

#include <entities/componentids.h>

// Qt.
#include <QtCore/QObject>

class QOpenGLContext;
class QOffscreenSurface;
class QQuickWindow;
class QSGTexture;

namespace ngs {

class FBORenderer;
class TextureDisplayNode;
class Resources;
class Entity;
class ShapeCanvas;
class Interaction;
class NodeSelection;

// The FBOWorker depebds on the FBORenderer.
// Together they run on a separate thread to render images to a texture.
class QUICK_EXPORT FBOWorker : public QObject, public Component {
Q_OBJECT
 public:

  COMPONENT_ID(FBOWorker, FBOWorker)

  FBOWorker(Entity* entity);
  virtual ~FBOWorker();

  // Our state.
  virtual void update_state();

  // These method must be called from Qt's Scene Graph rendering thread.
  // Generally this would be the "QQuickItem::updatePaintNode" method.
  virtual void initialize_gl();
  virtual void uninitialize_gl();
  virtual bool is_initialized_gl();

  // Get the finished display texture.
  QSGTexture* get_display_texture();

  const Dep<FBORenderer>& get_renderer() const;

 public Q_SLOTS:
  void render_next_texture();
  QSGTexture* swap_buffers();

Q_SIGNALS:

 private:
  // Our fixed deps.
  Dep<FBORenderer> _renderer;

  // Our qt texture wrappers. We own these.
  QSGTexture* _render_texture_wrapper;
  QSGTexture* _display_texture_wrapper;
};


}
