#include <entities/guientities.h>
#include <entities/nonguientities.h>
#include <entities/factory.h>

#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/entity.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>
#include <components/interactions/graphbuilder.h>
#include <components/interactions/viewcontrols.h>

#include <components/resources/resources.h>

#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/dotnodeshape.h>
#include <components/compshapes/inputlabelshape.h>
#include <components/compshapes/inputnodeshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/outputlabelshape.h>
#include <components/compshapes/outputnodeshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/groupnodeshape.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/rectnodeshape.h>

#include <components/computes/compute.h>
#include <components/computes/dotnodecompute.h>
#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/outputnodecompute.h>

#include <gui/widget/nodegrapheditor.h>

#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/appworker.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphview.h>

#include <QtGui/QGuiApplication>
#include <QtQml/QtQml>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtGui/QScreen>

/*
 * Entity Structure
 *
 * Syntax:  Entity : Components in the Entity
 * Indentation: Indented lines are child entities of the preceding line, otherwise they are siblings.
 * Dir has no components and acts a namespace
 *
 * Group:
 *   node_a : RectNodeShape, NodeCompute
 *     - inputs : dir
 *       - in_a : InputCompute, InputShape
 *         - label : InputLabelShape
 *     - outputs : dir
 *       - out_b : OutputCompute, OutputShape
 *         - label : OutputLabelShape
 *     - links : dir
 *       - link1 : LinkShape
 *   inputs : dir
 *   outputs : dir
 *   links : dir
 */

namespace ngs {

void QMLAppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeSelection(this);
  new_ff ShapeCanvas(this);
  // Node Graph Rendering through QtQuick/QML.
  new_ff FBORenderer(this);
  new_ff FBOWorker(this);
  new_ff NodeGraphQuickItem(this);
  new_ff NodeGraphView(this);
  // Qt Releated Components.
  new_ff FileModel(this);
  new_ff AppComm(this);
  new_ff AppWorker(this);
  new_ff LicenseChecker(this);
}

void QMLAppEntity::init_view(QSurfaceFormat& format) {
  NodeGraphView* view = get_node_graph_view();

  // Set the view size.
#if ARCH == ARCH_WINDOWS
  view->setWidth(800);
  view->setHeight(1024);
  view->setMaximumWidth(800);
  view->setMaximumHeight(1024);
  view->setMinimumWidth(800);
  view->setMinimumHeight(1024);
#else
  setWidth(QGuiApplication::primaryScreen()->size().width() );
  setHeight(QGuiApplication::primaryScreen()->size().height() );
#endif

  view->setResizeMode(QQuickView::SizeRootObjectToView);
  view->setFormat(format);
  view->setColor(QColor(3,169,244,255));
  view->update();

  view->engine()->addImportPath("qrc:/qml");
  view->engine()->addImportPath("qrc:/");
}

void QMLAppEntity::expose_to_qml() {
  // Grab some components without using Deps.
  FileModel* file_model = get_file_model();
  NodeGraphQuickItem* node_graph = get_node_graph_quick_item();
  AppComm* app_comm = get_app_comm();
  AppWorker* app_worker = get_app_worker();
  NodeGraphView* view = get_node_graph_view();
  LicenseChecker* license_checker = get_license_checker();

  // Register gl types.
  qRegisterMetaType<GLsizei>("GLsizei");

  // Register std types.
  qRegisterMetaType<size_t>("size_t"); // Used by some of our c++ quick item classes. (eg. NodeGraphQuickItem::get_num_nodes())

  // Register other types.
  qmlRegisterUncreatableType<InputCompute>("InputCompute", 1, 0, "InputCompute", "You cannot create this type from QML.");

  // Inject them into the qml context.
  QQmlContext* context = view->engine()->rootContext();
  context->setContextProperty(QStringLiteral("file_model"), file_model);
  context->setContextProperty(QStringLiteral("node_graph_item"), node_graph);
  context->setContextProperty(QStringLiteral("app_comm"), app_comm);
  context->setContextProperty(QStringLiteral("app_worker"), app_worker);
  context->setContextProperty(QStringLiteral("license_checker"), license_checker);

  // Expose other useful objects to qml.
  context->setContextProperty("pdpi", QGuiApplication::primaryScreen()->physicalDotsPerInch());
  context->setContextProperty("dpr", QGuiApplication::primaryScreen()->devicePixelRatio());
  context->setContextProperty(QStringLiteral("quick_view"), view);

#if (ARCH == ARCH_ANDROID)
    // Create our java bridge.
    JavaBridge *java_bridge = new_ff JavaBridge(&app);
    context->setContextProperty(QLatin1String("java_bridge"), java_bridge);
#endif
}

void QMLAppEntity::embed_node_graph() {
  NodeGraphView* view = get_node_graph_view();
  NodeGraphQuickItem* node_graph = get_node_graph_quick_item();

  // Embed the node graph quick item into the node graph page.
  QQuickItem* node_graph_page = view->rootObject()->findChild<QQuickItem*>("node_graph_page", Qt::FindChildrenRecursively);
  if (!node_graph_page) {
    // For our unit tests the root object is the node graph page.
    node_graph_page = view->rootObject();
    // Also the width() and height() keep coming back as zero.
    // So we hard code this for unit tests for now.
    node_graph->setWidth(1024);
    node_graph->setHeight(1024);
  } else {
    node_graph->setWidth(node_graph_page->width());
    node_graph->setHeight(node_graph_page->height());
  }
  node_graph->setParentItem(node_graph_page);

}

FileModel* QMLAppEntity::get_file_model() {
  return get<FileModel>();
}

AppComm* QMLAppEntity::get_app_comm() {
  return get<AppComm>();
}

AppWorker* QMLAppEntity::get_app_worker() {
  return get<AppWorker>();
}

LicenseChecker* QMLAppEntity::get_license_checker() {
  return get<LicenseChecker>();
}

NodeGraphQuickItem* QMLAppEntity::get_node_graph_quick_item() {
  return get<NodeGraphQuickItem>();
}

GraphBuilder* QMLAppEntity::get_graph_builder() {
  return get<GraphBuilder>();
}

NodeGraphView* QMLAppEntity::get_node_graph_view() {
  return get<NodeGraphView>();
}

void QtAppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeSelection(this);
  new_ff ShapeCanvas(this);
}

void AppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeSelection(this);
  new_ff ShapeCanvas(this);
}

void NodeHelperEntity::create_internals() {
  _inputs = add_namespace(this, "inputs");
  _outputs = add_namespace(this, "outputs");
}

Entity* NodeHelperEntity::add_namespace(Entity* parent, const std::string& name) {
  Entity* links = new_ff BaseNamespaceEntity(parent, name);
  links->create_internals();
  return links;
}

Entity* NodeHelperEntity::add_input(Entity* parent, const std::string& name, ParamType type, bool expose) {
  InputEntity* input = new_ff InputEntity(parent, name);
  input->create_internals();
  input->set_param_type(type);
  input->set_exposed(expose);
  return input;
}

Entity* NodeHelperEntity::add_output(Entity* parent, const std::string& name, bool expose) {
  OutputEntity* output = new_ff OutputEntity(parent, name);
  output->create_internals();
  output->set_exposed(expose);
  return output;
}

void GroupNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff GroupNodeCompute(this);
  // Gui related.
  new_ff GroupInteraction(this);
  new_ff CompShapeCollective(this);
  new_ff GroupNodeShape(this);
  // Create more namespace entities.
  add_namespace(this, "links");
}

void GroupNodeEntity::create_namespaces() {
  add_namespace(this, "links");
  add_namespace(this, "inputs");
  add_namespace(this, "outputs");
}

void GroupNodeEntity::copy(SimpleSaver& saver, const std::unordered_set<Entity*>& children) const {
    if (children.empty()) {
      return;
    }

    // Find the relevant links to save.
    std::unordered_set<Entity*> links_to_save;
    Entity* links_folder = get_child("links");
    if (links_folder) {
      const NameToChildMap &links = links_folder->get_children();
      NameToChildMap::const_iterator iter;
      for (iter = links.begin(); iter != links.end(); ++iter) {
        LinkShape* link_shape = static_cast<LinkShape*>(iter->second->get(LinkShape::kIID()));
        assert(link_shape);

        const Dep<InputShape>& input_shape = link_shape->get_input_shape();
        const Dep<NodeShape>& input_node_shape = input_shape->get_node_shape();
        Entity* input_node_entity = input_node_shape->our_entity();
        assert(input_node_entity);

        const Dep<OutputShape>& output_shape = link_shape->get_output_shape();
        const Dep<NodeShape>& output_node_shape = output_shape->get_node_shape();
        Entity* output_node_entity = output_node_shape->our_entity();
        assert(output_node_entity);

        if (children.count(input_node_entity) && children.count(output_node_entity)) {
          links_to_save.insert(iter->second);
        }
      }
    }

    // Save the number of entities.
    size_t num_nodes = children.size();
    if (links_to_save.size()) {
      // +1 for the links folder.
      num_nodes += 1;
    }
    saver.save(num_nodes);

    // Save the entities.
    for (Entity* e : children) {
      e->save(saver);
    }

    // Save the links folder.
    if (links_to_save.size()) {
      // Save out info about the folder.
      links_folder->pre_save(saver);
      links_folder->save_components(saver);
      // Save out the contents of the folder.
      size_t num_links = links_to_save.size();
      saver.save(num_links);
      for (Entity* e: links_to_save) {
        e->save(saver);
      }
    }
}

void LinkEntity::create_internals() {
  // Our components.
  // Gui components.
  new_ff LinkShape(this);
}

void DotNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff DotNodeCompute(this);
  // Gui components.
  DotNodeShape* shape = new_ff DotNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "in");
  add_output(_outputs,"out");
}

void InputNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff InputNodeCompute(this);
  // Gui components.
  new_ff InputNodeShape(this);
  // Our sub entities.
  add_output(_outputs, "out");
}

void OutputNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff OutputNodeCompute(this);
  // Gui components.
  new_ff OutputNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "in");
}

void MockNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff MockNodeCompute(this);
  // Gui components.
  new_ff RectNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "a");
  add_input(_inputs, "b");
  add_output(_outputs, "c");
  add_output(_outputs, "d");
}

void OpenBrowserNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff OpenBrowserCompute(this);
  // Gui components.
  new_ff RectNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "in");
  add_output(_outputs, "out");
}

void CloseBrowserNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff CloseBrowserCompute(this);
  // Gui components.
  new_ff RectNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "in");
  add_output(_outputs, "out");
}

void CreateSetFromValuesNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff CreateSetFromValuesCompute(this);
  // Gui components.
  new_ff RectNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "in");
  add_input(_inputs, "type", ParamType::kWrapType);
  add_input(_inputs, "values", ParamType::kQStringList);
  add_output(_outputs, "out");
}

void CreateSetFromTypeNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff CreateSetFromTypeCompute(this);
  // Gui components..
  new_ff RectNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "in");
  add_input(_inputs, "type", ParamType::kWrapType, false);
  add_output(_outputs, "out");
}

void MouseActionNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff MouseActionCompute(this);
  // Gui components.
  new_ff RectNodeShape(this);
  // Our input sub entities.
  add_input(_inputs, "in");
  add_input(_inputs, "set_index", ParamType::kInt, false);
  add_input(_inputs, "overlay_index", ParamType::kInt, false);
  add_input(_inputs, "action_type", ParamType::kActionType, false);
  add_input(_inputs, "x", ParamType::kFloat, false);
  add_input(_inputs, "y", ParamType::kFloat, false);
  add_output(_outputs, "out");
}


void ScriptNodeEntity::create_internals() {
  NodeHelperEntity::create_internals();
  // Our components.
  new_ff ScriptNodeCompute(this);
  // Gui components.
  new_ff RectNodeShape(this);
  // Our sub entities.
  add_input(_inputs, "in");
  add_output(_outputs, "out");
}

void InputEntity::create_internals() {
  // Our components.
  new_ff InputCompute(this);
  // Gui components.
  new_ff InputShape(this);
  // Our sub entities.
  {
    InputLabelEntity* label = new_ff InputLabelEntity(this, "label");
    label->create_internals();
  }
}

void InputEntity::set_param_type(ParamType param_type) {
  get<InputCompute>()->set_param_type(param_type);
}

void InputEntity::set_exposed(bool expose) {
  get<InputCompute>()->set_exposed(expose);
}

void InputLabelEntity::create_internals() {
  // Our components.
  // Gui components.
  new_ff InputLabelShape(this);
}

void OutputEntity::create_internals() {
  // Our components.
  new_ff OutputCompute(this);
  // Gui components.
  new_ff OutputShape(this);
  // Our sub entities.
  {
    Entity* label = new_ff OutputLabelEntity(this, "label");
    label->create_internals();
  }
}
void OutputEntity::set_exposed(bool expose) {
  get<OutputCompute>()->set_exposed(expose);
}

void OutputLabelEntity::create_internals() {
  // Our components.
  // Gui components.
  new_ff OutputLabelShape(this);
}

}
