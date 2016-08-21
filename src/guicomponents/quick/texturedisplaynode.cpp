//#include <base/device/deviceheadersgl.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/texturedisplaynode.h>
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

