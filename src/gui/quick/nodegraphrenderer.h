#pragma once
#include <gui/gui_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <components/entities/componentids.h>
#include <base/device/transforms/mouseinfo.h>

#include <QtQuick/QQuickFramebufferObject>


namespace ngs {

class ShapeCanvas;
class GraphBuilder;

class GUI_EXPORT NodeGraphRenderer : public QQuickFramebufferObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(NodeGraphRenderer, NodeGraphRenderer)

  NodeGraphRenderer();
  virtual ~NodeGraphRenderer();

  // Testing.
  Q_INVOKABLE void build_test_graph();

  virtual void initialize_fixed_deps();

  Renderer *createRenderer() const;
  void render();

private:
  Dep<ShapeCanvas> _canvas;
  Dep<GraphBuilder> _graph_builder;
};

}
