#include <components/interactions/shapecanvas.h>
#include <components/interactions/graphbuilder.h>

#include <base/objectmodel/deploader.h>
#include <guicomponents/quick/nodegraphquickitemglobals.h>
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



NodeGraphRenderer::NodeGraphRenderer()
    : Component(g_app_root, kIID(), kDID()),
      QQuickFramebufferObject(),
      _canvas(this),
      _graph_builder(this){
  get_dep_loader()->register_fixed_dep(_canvas, "");
  get_dep_loader()->register_fixed_dep(_graph_builder, "");

  initialize_fixed_deps();

  std::cerr << "enter something to continue\n";
  int dummy;
  std::cin >> dummy;
}

NodeGraphRenderer::~NodeGraphRenderer() {
}

void NodeGraphRenderer::initialize_fixed_deps() {
  std::cerr << "NodeGraphRenderer is initializing fixed deps\n";
  Component::initialize_fixed_deps();
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
//  int test;
//  std::cin >> test;
  std::cerr << "NodeGraphRenderer::render called\n";
  if (!g_quick_view) {
    std::cerr << "but g quick window was not set\n";
    return;
  }

  if (!_canvas) {
    std::cerr << "but canvas was not set\n";
    return;
  }
  if (!_canvas->is_initialized_gl()) {
    std::cerr << "initializing the app in the gl context \n";

    g_app_root->initialize_deps();
    g_app_root->update_deps_and_hierarchy();
    g_app_root->initialize_gl();

    std::cerr << "now building the test graph \n";
    _graph_builder->build_test_graph();
  }

  std::cerr << "NodeGraphRenderer::render is really drawing\n";
  _canvas->draw_gl();

}

}
