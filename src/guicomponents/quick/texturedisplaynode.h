#pragma once
#include <guicomponents/quick/quick_export.h>

// Qt.
#include <QtQuick/QSGSimpleTextureNode>


class QQuickWindow;

namespace ngs {

class FBOWorker;
class NodeGraphQuickItem;

// Note this class will run on Qt's Scene Graph render thread.
// The Qt's Scene Graph thread runs separate from the fbo rendering thread.
class QUICK_EXPORT TextureDisplayNode : public QObject, public QSGSimpleTextureNode
{
Q_OBJECT
   public:
  TextureDisplayNode();
  virtual ~TextureDisplayNode();
};


}
