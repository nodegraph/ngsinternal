#include <components/interactions/shapecanvas.h>
#include <components/interactions/graphbuilder.h>

#include <base/objectmodel/deploader.h>
#include <guicomponents/quick/nodegraphrenderer.h>
#include <guicomponents/quick/nodegraphrenderer.h>
#include <QtGui/QOpenGLFramebufferObject>

namespace ngs {

class RenderLogic : public QQuickFramebufferObject::Renderer
{
public:
  RenderLogic(NodeGraphRenderer* parent)
      : _parent(parent){
  }

  void render() {
    // FBO is already setup here.
    // Just render into it.
    std::cerr << "RenderLogic::render called\n";
    _parent->render();
    update();
  }

  QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(1);
    return new QOpenGLFramebufferObject(size, format);
  }

private:
  NodeGraphRenderer* _parent;
};



NodeGraphRenderer::NodeGraphRenderer(Entity* parent)
    : QQuickFramebufferObject(),
      Component(parent, kIID(), kDID()),
      _canvas(this),
      _graph_builder(this){
  get_dep_loader()->register_fixed_dep(_canvas, Path());
  get_dep_loader()->register_fixed_dep(_graph_builder, Path());

  std::cerr << "enter something to continue\n";
  int dummy;
  std::cin >> dummy;
}

NodeGraphRenderer::~NodeGraphRenderer() {
}

void NodeGraphRenderer::build_test_graph() {
  // Build a test graph.
  if (!_graph_builder) {
    std::cerr << "weird graph builder was not set\n";
    return;
  }
  std::cerr << "building the test graph\n";
  _graph_builder->build_test_graph();
}

NodeGraphRenderer::Renderer *NodeGraphRenderer::createRenderer() const
{
    std::cerr << "NodeGraphRenderer::createRenderer called\n";
    return new RenderLogic(const_cast<NodeGraphRenderer*>(this));
}

void NodeGraphRenderer::render() {
  if (!_canvas) {
    std::cerr << "but canvas was not set\n";
    return;
  }
  if (!_canvas->is_initialized_gl()) {
    std::cerr << "initializing the app in the gl context \n";
    get_app_root()->initialize_gl();
    std::cerr << "now building the test graph \n";
    _graph_builder->build_test_graph();
  }

  std::cerr << "NodeGraphRenderer::render is really drawing\n";
  _canvas->draw_gl();

}

}
