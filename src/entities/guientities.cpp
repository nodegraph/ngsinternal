#include <entities/guientities.h>
#include <entities/nonguientities.h>
#include <entities/factory.h>
#include <entities/componentinstancer.h>

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
#include <components/compshapes/topology.h>
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
#include <components/computes/datanodecompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/inputs.h>
#include <components/computes/outputs.h>

#include <gui/widget/nodegrapheditor.h>

#include <guicomponents/comms/nodejsprocess.h>
#include <guicomponents/comms/messagesender.h>
#include <guicomponents/computes/messagereceiver.h>
#include <guicomponents/comms/appconfig.h>
#include <guicomponents/computes/browserrecorder.h>
#include <guicomponents/computes/nodejsworker.h>
#include <guicomponents/computes/httpworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/cryptologic.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/computes/entergroupcompute.h>
#include <guicomponents/computes/enterbrowsergroupcompute.h>
#include <guicomponents/computes/enterfirebasegroupcompute.h>
#include <guicomponents/computes/entermqttgroupcompute.h>

#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/computes/scriptgroupnodecompute.h>

#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphview.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

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

void QMLAppEntity::create_internals(const std::vector<size_t>& ids) {
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
  new_ff NodeGraphController(this);
  new_ff NodeGraphView(this);
  new_ff NodeGraphManipulator(this);
  // Qt Releated Components.
  new_ff FileModel(this);
  new_ff NodeJSProcess(this);
  new_ff MessageSender(this);
  new_ff MessageReceiver(this);
  new_ff AppConfig(this);
  new_ff TaskScheduler(this);
  //new_ff AppComm(this);
  new_ff NodeJSWorker(this);
  new_ff BrowserRecorder(this);
  new_ff HTTPWorker(this);
  new_ff LicenseChecker(this);
  new_ff CryptoLogic(this);
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
  NodeGraphController* ng_controller = get_node_graph_controller();
  NodeJSWorker* web_worker = get_app_worker();
  BrowserRecorder* web_recorder = get_app_recorder();
  NodeGraphView* view = get_node_graph_view();
  LicenseChecker* license_checker = get_license_checker();

  // Clean and open the socket on web_worker.
  web_worker->clean_state();
  web_worker->open();

  // Register gl types.
  qRegisterMetaType<GLsizei>("GLsizei");

  // Register std types.
  qRegisterMetaType<size_t>("size_t"); // Used by some of our c++ quick item classes. (eg. NodeGraphQuickItem::get_num_nodes())

  // Register other types.
  //qmlRegisterUncreatableType<InputCompute>("InputCompute", 1, 0, "InputCompute", "You cannot create this type from QML.");

  // Inject them into the qml context.
  QQmlContext* context = view->engine()->rootContext();
  context->setContextProperty(QStringLiteral("file_model"), file_model);
  context->setContextProperty(QStringLiteral("node_graph_item"), node_graph);
  context->setContextProperty(QStringLiteral("web_worker"), web_worker);
  context->setContextProperty(QStringLiteral("web_recorder"), web_recorder);
  context->setContextProperty(QStringLiteral("license_checker"), license_checker);
  context->setContextProperty(QStringLiteral("ng_controller"), ng_controller);
  context->setContextProperty(QStringLiteral("js_type"), &ng_controller->js_type_wrap);
  context->setContextProperty(QStringLiteral("hint_key"), &ng_controller->hint_key_wrap);
  context->setContextProperty(QStringLiteral("enum_hint_value"), &ng_controller->enum_hint_value_wrap);

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

NodeJSWorker* QMLAppEntity::get_app_worker() {
  return get<NodeJSWorker>();
}

BrowserRecorder* QMLAppEntity::get_app_recorder() {
  return get<BrowserRecorder>();
}

LicenseChecker* QMLAppEntity::get_license_checker() {
  return get<LicenseChecker>();
}

NodeGraphQuickItem* QMLAppEntity::get_node_graph_quick_item() {
  return get<NodeGraphQuickItem>();
}

NodeGraphController* QMLAppEntity::get_node_graph_controller() {
  return get<NodeGraphController>();
}


GraphBuilder* QMLAppEntity::get_graph_builder() {
  return get<GraphBuilder>();
}

NodeGraphView* QMLAppEntity::get_node_graph_view() {
  return get<NodeGraphView>();
}

void QtAppEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeSelection(this);
  new_ff ShapeCanvas(this);
}

void AppEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeSelection(this);
  new_ff ShapeCanvas(this);
}

void GroupNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs();
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  new_ff GroupInteraction(this);
  new_ff CompShapeCollective(this);
  new_ff GroupNodeShape(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
  // Sub Components.
  ComputeNodeEntity* enter = new_ff ComputeNodeEntity(this, "group_context");
  enter->set_compute_did(ComponentDID::kEnterGroupCompute);
  enter->set_visible(true);
  enter->create_internals();
  ComputeNodeEntity* exit = new_ff ComputeNodeEntity(this, "exit_group_context");
  exit->set_compute_did(ComponentDID::kExitGroupCompute);
  exit->set_visible(false);
  exit->create_internals();
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

void ScriptGroupNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff ScriptGroupNodeCompute(this))->create_inputs_outputs();
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  new_ff GroupInteraction(this);
  new_ff CompShapeCollective(this);
  new_ff GroupNodeShape(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
  // Sub Components.
  ComputeNodeEntity* sub = new_ff ComputeNodeEntity(this, "group_context");
  sub->set_compute_did(ComponentDID::kEnterGroupCompute);
  sub->set_visible(true);
  sub->create_internals();
  ComputeNodeEntity* exit = new_ff ComputeNodeEntity(this, "exit_group_context");
  exit->set_compute_did(ComponentDID::kExitGroupCompute);
  exit->set_visible(false);
  exit->create_internals();
}

void BrowserGroupNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs();
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  new_ff GroupInteraction(this);
  new_ff CompShapeCollective(this);
  new_ff GroupNodeShape(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
  // Sub Components.
  ComputeNodeEntity* sub = new_ff ComputeNodeEntity(this, "group_context");
  sub->set_compute_did(ComponentDID::kEnterBrowserGroupCompute);
  sub->set_visible(true);
  sub->create_internals();
  ComputeNodeEntity* exit = new_ff ComputeNodeEntity(this, "exit_group_context");
  exit->set_compute_did(ComponentDID::kExitBrowserGroupCompute);
  exit->set_visible(false);
  exit->create_internals();
}

void FirebaseGroupNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs();
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  new_ff GroupInteraction(this);
  new_ff CompShapeCollective(this);
  new_ff GroupNodeShape(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
  // Sub Components.
  ComputeNodeEntity* sub = new_ff ComputeNodeEntity(this, "group_context");
  sub->set_compute_did(ComponentDID::kEnterFirebaseGroupCompute);
  sub->set_visible(true);
  sub->create_internals();
  ComputeNodeEntity* exit = new_ff ComputeNodeEntity(this, "exit_group_context");
  exit->set_compute_did(ComponentDID::kExitGroupCompute);
  exit->set_visible(false);
  exit->create_internals();
}

void MQTTGroupNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs();
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  new_ff GroupInteraction(this);
  new_ff CompShapeCollective(this);
  new_ff GroupNodeShape(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
  // Sub Components.
  ComputeNodeEntity* sub = new_ff ComputeNodeEntity(this, "group_context");
  sub->set_compute_did(ComponentDID::kEnterMQTTGroupCompute);
  sub->set_visible(true);
  sub->create_internals();
  ComputeNodeEntity* exit = new_ff ComputeNodeEntity(this, "exit_group_context");
  exit->set_compute_did(ComponentDID::kExitGroupCompute);
  exit->set_visible(false);
  exit->create_internals();
}

void LinkEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  // Gui components.
  new_ff LinkShape(this);
}

void DotNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff DotNodeCompute(this))->create_inputs_outputs();
  // Gui components.
  new_ff DotNodeShape(this);
  new_ff Inputs(this);
  new_ff Outputs(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
}

void DataNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff DataNodeCompute(this))->create_inputs_outputs();
  // Gui components.
  new_ff InputNodeShape(this);
  new_ff Inputs(this);
  new_ff Outputs(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
}

void InputNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff InputNodeCompute(this))->create_inputs_outputs();
  // Gui components.
  new_ff InputNodeShape(this);
  new_ff Inputs(this);
  new_ff Outputs(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
}

void OutputNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff OutputNodeCompute(this))->create_inputs_outputs();
  // Gui components.
  new_ff OutputNodeShape(this);
  new_ff Inputs(this);
  new_ff Outputs(this);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
}

void ComputeNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  // The compute component::did must be set with a call to set_compute_did before calling this method.
  Compute* c = static_cast<Compute*>(ComponentInstancer::instance_imp(this,_did));
  c->create_inputs_outputs();

  // Input and output gatherers.
  new_ff Inputs(this);
  new_ff Outputs(this);

  // Gui components.
  RectNodeShape* r = new_ff RectNodeShape(this);
  r->set_visible(_visible);
  new_ff InputTopology(this);
  new_ff OutputTopology(this);
}

void ComputeNodeEntity::set_compute_did(ComponentDID did) {
  _did = did;
}

Compute* ComputeNodeEntity::get_compute() {
  return get<Compute>();
}

void InputEntity::create_internals(const std::vector<size_t>& ids) {
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

void InputEntity::set_unconnected_value(const QJsonValue& value) {
  get<InputCompute>()->set_unconnected_value(value);
}

void InputEntity::set_exposed(bool expose) {
  get<InputCompute>()->set_exposed(expose);
}

void InputLabelEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  // Gui components.
  new_ff InputLabelShape(this);
}

void OutputEntity::create_internals(const std::vector<size_t>& ids) {
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

void OutputLabelEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  // Gui components.
  new_ff OutputLabelShape(this);
}

}
