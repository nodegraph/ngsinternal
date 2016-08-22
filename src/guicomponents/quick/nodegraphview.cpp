#include <guicomponents/quick/nodegraphview.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/comms/appcomm.h>

#include <QtGui/QGuiApplication>
#include <QtQml/QtQml>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtGui/QScreen>

namespace ngs {

// Using the QQmlApplicationEngine would of allowed us to declare a ApplicationWindow on the QML side.
// This would have been nicer but, it doesn't allow us to set the opengl context and version settings.
// Hence we use a QQuickView as the ApplicationWindow and then load QML components into that.

NodeGraphView::NodeGraphView(Entity* entity)
    : QQuickView(NULL),
      Component(entity, kIID(), kDID()) {

  // Set the app icon.
  QIcon icon(":images/octopus_blue.png");
  setIcon(icon);
  setTitle("Smash Browse");
}

NodeGraphView::~NodeGraphView() {
}

QSGTexture* NodeGraphView::create_texture_from_id(uint id, const QSize &size, CreateTextureOptions options) const {
  return createTextureFromId(id, size, options);
}

}
