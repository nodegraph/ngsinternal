//#include <base/device/deviceheadersgl.h>
#include <gui/qt/qml/texturedisplaynode.h>
#include <gui/qt/qml/fboworker.h>
#include <gui/qt/qml/nodegraphquickitem.h>
#include <QtQuick/QQuickWindow>

#include <iostream>

namespace ngs {

TextureDisplayNode::TextureDisplayNode(){
  setFiltering(QSGTexture::None);
  setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
  setFlag(QSGNode::OwnedByParent);
}

TextureDisplayNode::~TextureDisplayNode() {
}

}

