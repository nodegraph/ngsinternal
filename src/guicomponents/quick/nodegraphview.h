#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtQuick/QQuickView>


class QSGTexture;

namespace ngs {

class NodeGraphQuickItem;

class QUICK_EXPORT NodeGraphView: public QQuickView, public Component {
  Q_OBJECT
 public:

  COMPONENT_ID(NodeGraphView, NodeGraphView);

  NodeGraphView(Entity* entity);
  virtual ~NodeGraphView();

  // Overrides.
  QSGTexture* create_texture_from_id(uint id, const QSize &size, CreateTextureOptions options = CreateTextureOption()) const;

  Q_INVOKABLE static bool app_update_is_available();
  Q_INVOKABLE static void close_splash_page();

  Q_INVOKABLE void start_app_update();
  Q_INVOKABLE bool update_is_starting();




 private:

  bool _update_is_starting;

};

}
