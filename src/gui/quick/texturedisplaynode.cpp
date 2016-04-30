//#include <base/device/deviceheadersgl.h>
#include <gui/quick/texturedisplaynode.h>
#include <gui/quick/fboworker.h>
#include <gui/quick/nodegraphquickitem.h>
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

