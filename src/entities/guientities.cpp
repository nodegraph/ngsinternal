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
#include <components/computes/ifgroupnodecompute.h>
#include <components/computes/whilegroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/inputs.h>
#include <components/computes/outputs.h>
#include <gui/widget/nodegrapheditor.h>

#include <guicomponents/computes/messagesender.h>
#include <guicomponents/computes/messagereceiver.h>
#include <base/objectmodel/appconfig.h>
#include <components/computes/scriptloopcontext.h>

#include <guicomponents/computes/browserrecorder.h>
#include <guicomponents/computes/httpworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/cryptologic.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/computes/acceptsaveprocess.h>
#include <guicomponents/computes/javaprocess.h>
#include <guicomponents/computes/downloadmanager.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/mqtthostcompute.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/computes/taskqueuer.h>
#include <guicomponents/computes/waitnodecompute.h>

#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphcontroller.h>
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

// -----------------------------------------------------------------------------------------------------------
// Helpers.
// -----------------------------------------------------------------------------------------------------------

// Create and connect an input node to each input for a node.
void surround_with_input_nodes(Entity* node) {
  QMLAppEntity* app_root = static_cast<QMLAppEntity*>(node->get_app_root());
  BaseNodeGraphManipulator* manipulator = app_root->get_manipulator();
  Entity* inputs = node->get_child(kInputsFolderName);
  const Entity::NameToChildMap& children = inputs->get_children();
  for (auto &iter: children) {
    // Create the input node.
    InputNodeEntity* input_node = new_ff InputNodeEntity(node->get_parent(), iter.first);
    EntityConfig config;
    config.visible = false;
    input_node->create_internals(config);

    // Connect the input to the output from the input node.
    OutputEntity* output = static_cast<OutputEntity*>(input_node->get_child(kOutputsFolderName)->get_child(kMainOutputNodeName));
    InputEntity* input = static_cast<InputEntity*>(iter.second);
    manipulator->connect_plugs(input, output);

    // Copy the hint description from the node being surrounded to the input plug.
    manipulator->copy_description_to_input_node(input, input_node);
  }
}

// -----------------------------------------------------------------------------------------------------------
// Implementations.
// -----------------------------------------------------------------------------------------------------------


void QMLAppEntity::create_internals(const EntityConfig& config) {
  // Our components.
  new_ff Factory(this);
  new_ff AppConfig(this);
  new_ff LicenseChecker(this);
  new_ff CryptoLogic(this);
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  // Gui related.
  if (config.visible) {
    new_ff NodeSelection(this);
    new_ff ShapeCanvas(this);
    // Node Graph Rendering through QtQuick/QML.
    new_ff FBORenderer(this);
    new_ff FBOWorker(this);
    new_ff NodeGraphQuickItem(this);
    new_ff NodeGraphController(this);
    new_ff NodeGraphView(this);
    new_ff NodeGraphManipulator(this);
  }
  // Our tasks processing and workers.
  new_ff FileModel(this);
  new_ff AcceptSaveProcess(this);
  new_ff JavaProcess(this);
  new_ff DownloadManager(this);
  new_ff MessageSender(this);
  new_ff MessageReceiver(this);
  new_ff TaskScheduler(this);
  new_ff TaskQueuer(this);
  new_ff BrowserRecorder(this);
  new_ff HTTPWorker(this);

}

void QMLAppEntity::init_view(QSurfaceFormat& format) {
  NodeGraphView* view = get_node_graph_view();

  qDebug() << "physical dots per inch: " << QGuiApplication::primaryScreen()->physicalDotsPerInch();
  qDebug() << "device pixel ratio: " << QGuiApplication::primaryScreen()->devicePixelRatio();
  qDebug() << "width: " << QGuiApplication::primaryScreen()->size().width();
  qDebug() << "height: " << QGuiApplication::primaryScreen()->size().height();


  // Set the view size.
//#if ARCH == ARCH_WINDOWS
  view->setWidth(800);
  view->setHeight(1024);
  view->setMaximumWidth(800);
  view->setMaximumHeight(1024);
  view->setMinimumWidth(800);
  view->setMinimumHeight(1024);
//#else
//  setWidth(QGuiApplication::primaryScreen()->size().width() );
//  setHeight(QGuiApplication::primaryScreen()->size().height() );
//#endif

  view->setResizeMode(QQuickView::SizeRootObjectToView);
  view->setFormat(format);
  view->setColor(QColor(3,169,244,255));
  view->update();

  view->engine()->addImportPath("qrc:/qml");
  view->engine()->addImportPath("qrc:/");
}

void QMLAppEntity::expose_to_qml() {
  // Grab some components without using Deps.
  CryptoLogic* crypto_logic = get_crypto_logic();
  LicenseChecker* license_checker = get_license_checker();
  FileModel* file_model = get_file_model();
  NodeGraphQuickItem* node_graph = get_node_graph_quick_item();
  NodeGraphController* ng_controller = get_node_graph_controller();
  TaskQueuer* task_queuer = get_task_queuer();
  BrowserRecorder* browser_recorder = get_app_recorder();
  DownloadManager* download_manager = get_download_manager();
  NodeGraphView* view = get_node_graph_view();
  BaseNodeGraphManipulator* base_manipulator = get_manipulator();
  NodeGraphManipulator* ng_manipulator = static_cast<NodeGraphManipulator*>(base_manipulator);
  NodeGraphManipulatorImp* manipulator = ng_manipulator->get_imp();

  // Clean and open the socket on task_queuer.
  task_queuer->clean_state();
  task_queuer->open();

  // Register gl types.
  qRegisterMetaType<GLsizei>("GLsizei");

  // Register std types.
  qRegisterMetaType<size_t>("size_t"); // Used by some of our c++ quick item classes. (eg. NodeGraphQuickItem::get_num_nodes())

  // Register other types.
  //qmlRegisterUncreatableType<InputCompute>("InputCompute", 1, 0, "InputCompute", "You cannot create this type from QML.");

  // Inject them into the qml context.
  QQmlContext* context = view->engine()->rootContext();
  context->setContextProperty(QStringLiteral("crypto_logic"), crypto_logic);
  context->setContextProperty(QStringLiteral("license_checker"), license_checker);
  context->setContextProperty(QStringLiteral("file_model"), file_model);
  context->setContextProperty(QStringLiteral("node_graph_item"), node_graph);
  context->setContextProperty(QStringLiteral("task_queuer"), task_queuer);
  context->setContextProperty(QStringLiteral("browser_recorder"), browser_recorder);
  context->setContextProperty(QStringLiteral("download_manager"), download_manager);
  context->setContextProperty(QStringLiteral("manipulator"), manipulator);
  context->setContextProperty(QStringLiteral("physical_dots_per_inch"), QGuiApplication::primaryScreen()->physicalDotsPerInch());

  qmlRegisterType<GUITypes>("GUITypes", 1, 0, "GUITypes");
  qRegisterMetaType<GUITypes::JSType>("JSType");
  qRegisterMetaType<GUITypes::HintKey>("HintKey");
  qRegisterMetaType<GUITypes::EnumHintValue>("EnumHintValue");
  qRegisterMetaType<GUITypes::PostType>("PostType");
  qRegisterMetaType<GUITypes::VideoDownloadState>("VideoDownloadState");
  //Q_DECLARE_METATYPE(GUITypes::JSType);
  //Q_DECLARE_METATYPE(GUITypes::HintKey);
  //Q_DECLARE_METATYPE(GUITypes::EnumHintValue);

  context->setContextProperty(QStringLiteral("ng_controller"), ng_controller);
  //context->setContextProperty(QStringLiteral("gui_types"), &ng_controller->js_type_wrap);
  //context->setContextProperty(QStringLiteral("hint_key"), &ng_controller->hint_key_wrap);
  //context->setContextProperty(QStringLiteral("enum_hint_value"), &ng_controller->enum_hint_value_wrap);

  // Expose other useful objects to qml.
  context->setContextProperty("pdpi", QGuiApplication::primaryScreen()->physicalDotsPerInch());
  context->setContextProperty("dpr", QGuiApplication::primaryScreen()->devicePixelRatio());
  context->setContextProperty(QStringLiteral("quick_view"), view);

#if (ARCH == ARCH_ANDROID)
    // Create our java bridge.
    //JavaBridge *java_bridge = new_ff JavaBridge(&app);
    //context->setContextProperty(QLatin1String("java_bridge"), java_bridge);
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

CryptoLogic* QMLAppEntity::get_crypto_logic() {
  return get<CryptoLogic>();
}

LicenseChecker* QMLAppEntity::get_license_checker() {
  return get<LicenseChecker>();
}

FileModel* QMLAppEntity::get_file_model() {
  return get<FileModel>();
}

TaskQueuer* QMLAppEntity::get_task_queuer() {
  return get<TaskQueuer>();
}

BrowserRecorder* QMLAppEntity::get_app_recorder() {
  return get<BrowserRecorder>();
}

DownloadManager* QMLAppEntity::get_download_manager() {
  return get<DownloadManager>();
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

BaseNodeGraphManipulator* QMLAppEntity::get_manipulator() {
  return get<BaseNodeGraphManipulator>();
}

void QtAppEntity::create_internals(const EntityConfig& config) {
  // Our components.
  new_ff Factory(this);
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  // Gui related.
  if (config.visible) {
    new_ff NodeSelection(this);
    new_ff ShapeCanvas(this);
  }
}

void AppEntity::create_internals(const EntityConfig& config) {
  // Our components.
  new_ff Factory(this);
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  // Gui related.
  if (config.visible) {
    new_ff NodeSelection(this);
    new_ff ShapeCanvas(this);
  }
}



void GroupNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  if (config.visible) {
    new_ff GroupInteraction(this);
    new_ff CompShapeCollective(this);
    new_ff GroupNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }

  // These sub components are not required for the functionality of the group.
  // They are just added in for convenience. The user may destroy or rename them.
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, kMainInputNodeName);
    EntityConfig config2;
    config2.visible = true;
    config2.unconnected_value = QJsonObject();
    in->create_internals(config2);
  }
  {
    OutputNodeEntity* out = new_ff OutputNodeEntity(this, kMainOutputNodeName);
    EntityConfig config2;
    config2.visible = true;
    out->create_internals(config2);
  }

}

void GroupNodeEntity::copy(SimpleSaver& saver, const std::unordered_set<Entity*>& children) const {
    if (children.empty()) {
      return;
    }

    // Find the relevant links to save.
    std::unordered_set<Entity*> links_to_save;
    Entity* links_folder = get_child(kLinksFolderName);
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

void IfGroupNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff IfGroupNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  if (config.visible) {
    new_ff GroupInteraction(this);
    new_ff CompShapeCollective(this);
    new_ff GroupNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
  // Sub Components.
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, kMainInputNodeName);
    EntityConfig config2;
    config2.visible = true;
    config2.unconnected_value = QJsonObject();
    in->create_internals(config2);
  }
  {
    OutputNodeEntity* out = new_ff OutputNodeEntity(this, kMainOutputNodeName);
    EntityConfig config2;
    config2.visible = true;
    out->create_internals(config2);
  }
}

void WhileGroupNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff WhileGroupNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  new_ff ScriptLoopContext(this);
  // Gui related.
  if (config.visible) {
    new_ff GroupInteraction(this);
    new_ff CompShapeCollective(this);
    new_ff GroupNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
  // Sub Components.
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, kMainInputNodeName);
    EntityConfig config2;
    config2.visible = true;
    config2.unconnected_value = QJsonObject();
    in->create_internals(config2);
  }
  {
    OutputNodeEntity* out = new_ff OutputNodeEntity(this, kMainOutputNodeName);
    EntityConfig config2;
    config2.visible = true;
    out->create_internals(config2);
  }
}

void BrowserGroupNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  if (config.visible) {
    new_ff GroupInteraction(this);
    new_ff CompShapeCollective(this);
    new_ff GroupNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
  // Sub Components.
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, "browser_width");
    EntityConfig config2;
    config2.visible = false;
    config2.unconnected_value = 1024;
    in->create_internals(config2);
  }
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, "browser_height");
    EntityConfig config2;
    config2.visible = false;
    config2.unconnected_value = 1150;
    in->create_internals(config2);
  }
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, "release_on_exit");
    EntityConfig config2;
    config2.visible = false;
    config2.unconnected_value = false;
    in->create_internals(config2);
  }

  // These sub components are not required for the functionality of the browser group.
  // They are just added in for convenience. The user may destroy or rename them.
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, kMainInputNodeName);
    EntityConfig config2;
    config2.visible = true;
    config2.unconnected_value = QJsonObject();
    in->create_internals(config2);
  }
  {
    OutputNodeEntity* out = new_ff OutputNodeEntity(this, kMainOutputNodeName);
    EntityConfig config2;
    config2.visible = true;
    out->create_internals(config2);
  }
}

void MQTTGroupNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  if (config.visible) {
    new_ff GroupInteraction(this);
    new_ff CompShapeCollective(this);
    new_ff GroupNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
  // Sub Components.
  ComputeNodeEntity* host = new_ff ComputeNodeEntity(this, "host");
  EntityConfig config2;
  config2.compute_did = ComponentDID::kMQTTHostCompute;
  config2.visible = false;
  host->create_internals(config2);
  surround_with_input_nodes(host);

  // These sub components are not required for the functionality of the mqtt group.
  // They are just added in for convenience. The user may destroy or rename them.
  {
    InputNodeEntity* in = new_ff InputNodeEntity(this, kMainInputNodeName);
    EntityConfig config2;
    config2.visible = true;
    config2.unconnected_value = QJsonObject();
    in->create_internals(config2);
  }
  {
    OutputNodeEntity* out = new_ff OutputNodeEntity(this, kMainOutputNodeName);
    EntityConfig config2;
    config2.visible = true;
    out->create_internals(config2);
  }
}

void LinkEntity::create_internals(const EntityConfig& config) {
  // Our components.
  // Gui components.
  if (config.visible) {
    new_ff LinkShape(this);
  }
}

void WaitNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff WaitNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui components.
  if (config.visible) {
    new_ff RectNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

void DotNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff DotNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui components.
  if (config.visible) {
    new_ff DotNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

void DataNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff DataNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui components.
  if (config.visible) {
    new_ff InputNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

void PasswordDataNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff PasswordDataNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui components.
  if (config.visible) {
    new_ff InputNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

void InputNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff InputNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui components.
  if (config.visible) {
    new_ff InputNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

void PasswordInputNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff PasswordInputNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui components.
  if (config.visible) {
    new_ff InputNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

void OutputNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff OutputNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);

  // Gui components.
  if (config.visible) {
    OutputNodeShape* os = new_ff OutputNodeShape(this);
    os->set_pos(glm::vec2(0, -700));
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

void ComputeNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  // The compute component::did must be set with a call to set_compute_did before calling this method.
  Compute* c = static_cast<Compute*>(ComponentInstancer::instance_imp(this, config.compute_did));
  c->create_inputs_outputs(config);

  // Input and output gatherers.
  new_ff Inputs(this);
  new_ff Outputs(this);

  // Gui components.
  if (config.visible) {
    new_ff RectNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }
}

Compute* ComputeNodeEntity::get_compute() {
  return get<Compute>();
}

void InputEntity::create_internals(const EntityConfig& config) {
  // Our components.
  InputCompute* ic = new_ff InputCompute(this);
  ic->set_exposed(config.expose_plug);
  ic->set_unconnected_value(config.unconnected_value);

  // Gui components.
  if (config.visible) {
    new_ff InputShape(this);
    // Our sub entities.
    {
      InputLabelEntity* label = new_ff InputLabelEntity(this, "label");
      label->create_internals();
    }
  }
}

void InputLabelEntity::create_internals(const EntityConfig& config) {
  // Our components.
  // Gui components.
  if (config.visible) {
    new_ff InputLabelShape(this);
  }
}

void OutputEntity::create_internals(const EntityConfig& config) {
  // Our components.
  OutputCompute* oc = new_ff OutputCompute(this);
  oc->set_exposed(config.expose_plug);

  // Gui components.
  if (config.visible) {
    new_ff OutputShape(this);
    // Our sub entities.
    {
      Entity* label = new_ff OutputLabelEntity(this, "label");
      label->create_internals();
    }
  }
}

void OutputLabelEntity::create_internals(const EntityConfig& config) {
  // Our components.
  // Gui components.
  if (config.visible) {
    new_ff OutputLabelShape(this);
  }
}

void PublicMacroNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs(config);
  new_ff Inputs(this);
  new_ff Outputs(this);
  // Gui related.
  if (config.visible) {
    new_ff GroupInteraction(this);
    new_ff CompShapeCollective(this);
    new_ff GroupNodeShape(this);
    new_ff InputTopology(this);
    new_ff OutputTopology(this);
  }

  // Unlike our base class don't create the in and out nodes.
}

void PublicMacroNodeEntity::save(SimpleSaver& saver) const {
  pre_save(saver);
  save_components(saver);

  // We only save our inputs and outputs namespace child entities.
  // The rest of the children will be loaded from the macro file.
  Entity* inputs = get_child(kInputsFolderName);
  Entity* outputs = get_child(kOutputsFolderName);

  // Determine if we have inputs and outputs.
  size_t num_children = 0;
  if (inputs) {
    ++num_children;
  }
  if (outputs) {
    ++num_children;
  }

  // Save our inputs and outputs namespace children.
  saver.save(num_children);
  if (inputs) {
    inputs->save(saver);
  }
  if (outputs) {
    outputs->save(saver);
  }

  // Save our macro name.
  saver.save(_macro_name);
}

void PublicMacroNodeEntity::load_helper(SimpleLoader& loader) {
  // Load components.
  load_components(loader);

  // Load our inputs and outputs namespace children.
  paste_with_merging(loader);

  // Load our internal child nodes.
  loader.load(_macro_name);
  load_internals(_macro_name);
}

QByteArray PublicMacroNodeEntity::load_file_contents() const {
  // Read all the bytes from the file.
  std::string filename = AppConfig::get_public_macros_dir().toStdString() + "/" + _macro_name;
  QFile file(filename.c_str());
  file.open(QIODevice::ReadOnly);
  return file.readAll();
}

void PublicMacroNodeEntity::load_internals(const std::string& macro_name) {
  _macro_name = macro_name;

  QByteArray contents = load_file_contents();

  // Load our other children from the macro file.
  {
    // Now load the data into the app root entity.
    Bits* bits = create_bits_from_raw(contents.data(),contents.size());
    SimpleLoader loader2(bits);

    // Read off the version number
    size_t major;
    size_t minor;
    size_t patch;
    size_t tweak;

    loader2.load(major);
    loader2.load(minor);
    loader2.load(patch);
    loader2.load(tweak);

    // Read off the description.
    std::string description;
    loader2.load(description);
    std::cerr << "xxxxxxxxxxxxxxxxxxx: " << description << "\n";

    // Read off the did and name.
    {
      EntityDID did;
      loader2.load(did);
      std::string name;
      loader2.load(name);

      // The top did of the macro should always be a regular group node.
      assert(did == EntityDID::kGroupNodeEntity);

      std::cerr << "did: " << (int)did << "\n";
      std::cerr << "name: " << name << "\n";
    }

    // Skip the components.
    Entity* dummy = new_ff Entity(get_app_root(), "__dummy_entity_for_loading__");
    dummy->load_components(loader2);
    delete_ff(dummy);

    std::cerr << "skipped components\n";

    // Load the children in the macro file except for the inputs and outputs namespace.
    {
      // Load the number of entities.
      size_t num_entities;
      loader2.load(num_entities);
      std::cerr << "the number of child entities is: " << num_entities << "\n";

      // Record the set of entities that we've pasted.
      std::unordered_set<Entity*> entities_loaded;

      // Load the entities.
      for (size_t i=0; i<num_entities; ++i) {
        // Peak at the did and name.
        EntityDID did;
        loader2.load(did);
        std::string name;
        loader2.load(name);
        loader2.rewind(name);
        loader2.rewind(did);
        if (did == EntityDID::kBaseNamespaceEntity) {
          if (name == kInputsFolderName) {
            Entity* inputs = get_child(kInputsFolderName);
            if (inputs) {
              // Skip the inputs entity.
              {
                Entity* dummy = new_ff Entity(get_app_root(), "__dummy_entity_for_loading__");
                dummy->paste_entity_with_merging(loader2);
                delete_ff(dummy);
                std::cerr << "skipped inputs\n";
              }
              entities_loaded.insert(inputs);
              continue;
            }
          } else if (name == kOutputsFolderName) {
            Entity* outputs = get_child(kOutputsFolderName);
            if (outputs) {
              // Skip the outputs entity.
              {
                Entity* dummy = new_ff Entity(get_app_root(), "__dummy_entity_for_loading__");
                dummy->paste_entity_with_merging(loader2);
                delete_ff(dummy);
                std::cerr << "skipped outputs\n";
              }
              entities_loaded.insert(outputs);
              continue;
            }
          }
        }
        Entity* e = paste_entity_with_merging(loader2);
        entities_loaded.insert(e);
      }

      // Find and collect any internal nodes which were not specified in the file
      // and are not used by the inputs and outputs.
      // Note the input and output nodes share the same namespace as the internal nodes.
      std::unordered_set<Entity*> entities_to_destroy;
      for (auto &e: get_children()) {
        if (entities_loaded.count(e.second) == 0) {
          entities_to_destroy.insert(e.second);
        }
      }

      // Delete nodes not specified in the file.
      for (Entity* c: entities_to_destroy) {
        delete_ff(c);
      }
    }
  }
}

QByteArray PrivateMacroNodeEntity::load_file_contents() const {
  // Read all the bytes from the file.
  std::string filename = "../" + AppConfig::kPrivateMacrosDir.toStdString() + "/" + _macro_name;
  QMLAppEntity* app = static_cast<QMLAppEntity*>(this->get_app_root());
  CryptoLogic * crypto_logic = app->get_crypto_logic();
  return crypto_logic->load_file(filename.c_str());
}

QByteArray AppMacroNodeEntity::load_file_contents() const {
  // Read all the bytes from the file.
  std::string filename = AppConfig::get_app_macros_dir().toStdString() + "/" + _macro_name;
  QFile file(filename.c_str());
  file.open(QIODevice::ReadOnly);
  return file.readAll();
}

}
