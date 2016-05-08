//#include <base/device/deviceheadersgl.h>
#include <base/memoryallocator/taggednew.h>
#include <base/glewhelper/glewhelper.h>

#include <components/entities/entityids.h>
#include <components/computes/compute.h>
#include <components/resources/resources.h>
#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>
#include <components/interactions/graphbuilder.h>

#include <gui/quick/fboworker.h>
#include <gui/quick/fborenderer.h>
#include <gui/quick/texturedisplaynode.h>
#include <gui/widget/eventtoinfo.h>
#include <gui/widget/splashscreen.h>


#include <base/device/transforms/wheelinfo.h>
#include <base/device/transforms/keyinfo.h>
#include <base/device/devicedebug.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/entity.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <base/utils/bits.h>

#include <components/compshapes/compshape.h>
#include <components/compshapes/compshapecollective.h>

#include <gui/quick/nodegraphquickitem.h>
#include <gui/quick/nodegraphquickitemglobals.h>

#include <base/objectmodel/basefactory.h>
#include <components/compshapes/nodegraphselection.h>
#include <components/interactions/viewcontrols.h>

#include <QtQuick/QQuickWindow>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QRunnable>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <iostream>
#include <cassert>

#include <iostream>
#include <sstream>
#include <fstream>

namespace ngs {



const int NodeGraphQuickItem::kLongPressTimeThreshold = 400; // in milli seconds.
const float NodeGraphQuickItem::kLongPressDistanceThreshold = 30; // in object space.
const int NodeGraphQuickItem::kDoublePressTimeThreshold = 300; // max time between release events for a double tap/click.

const QString NodeGraphQuickItem::kLinkableFile = "com.octoplier.app.nodegraph.lkb";
const QString NodeGraphQuickItem::kLinkableDir = "com.octoplier.app";

const int NodeGraphQuickItem::kMinimumIdleLength = 1000; // 1 second in milli seconds.

class CleanUpGL : public QRunnable
{
public:
  void run () {
    std::cerr << "CleanUpGL is running !!!!!!!!!!!!!!!!!!!!\n";
    g_app_root->uninitialize_gl();
  }
};

NodeGraphQuickItem::NodeGraphQuickItem(QQuickItem *parent)
    : Component(g_app_root, kIID(), kDID()),
      QQuickItem(parent),
      _fbo_worker(this),
      _selection(this),
      _canvas(this),
      _factory(this),
      _graph_builder(this),
      _last_pressed_shape(this),
      _pinch_mode(false) {

  get_dep_loader()->register_fixed_dep(_fbo_worker, "");
  get_dep_loader()->register_fixed_dep(_selection, "");
  get_dep_loader()->register_fixed_dep(_canvas, "");
  get_dep_loader()->register_fixed_dep(_factory, "");
  get_dep_loader()->register_fixed_dep(_graph_builder, "");

  _device_pixel_ratio = static_cast<GLsizei>(QGuiApplication::primaryScreen()->devicePixelRatio());

  // Configure.
  setAcceptedMouseButtons(Qt::AllButtons);
  setAcceptHoverEvents(true);
  setFlag(QQuickItem::ItemHasContents, true);

  // Connect a slot to perform cleanup prior to destruction of the QQuickWindow.
  connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handle_window_changed(QQuickWindow*)));

  // Setup the timer to trigger on long presses.
  _long_press_timer.setSingleShot(true);
  _long_press_timer.setInterval(kLongPressTimeThreshold);
  connect(&_long_press_timer,SIGNAL(timeout()),this,SLOT(on_long_press()));

//  // Setup the idle time to save when the event loop is idle.
//  connect(&_idle_timer, SIGNAL(timeout()), this, SLOT(on_save()));
//  _idle_timer.start();
//  _idle_length.start();

  initialize_fixed_deps();

  //connect(this,SIGNAL(mark_progress()), g_splash_screen, SLOT(mark_progress()));
}

NodeGraphQuickItem::~NodeGraphQuickItem() {
}

void NodeGraphQuickItem::initialize_fixed_deps() {
  Component::initialize_fixed_deps();
  _entity_instancer = _factory->get_entity_instancer();

//  #if ARCH == ARCH_ANDROID
//      {
//        QSurfaceFormat format = window()->format();
//        format.setProfile(QSurfaceFormat::CoreProfile);
//        format.setRenderableType(QSurfaceFormat::OpenGLES);
//        format.setVersion(3, 0);
//        window()->setFormat(format);
//      }
//  #endif
}

void NodeGraphQuickItem::build_test_graph() {
  // Build a test graph.
  _graph_builder->build_test_graph();
}

// This gets called from the scene graph render thread.
void NodeGraphQuickItem::cleanup() {
  qDebug() <<  "node graph quick item doing cleanup \n";

  g_app_root->uninitialize_gl();
  g_app_root->uninitialize_deps();

  finish_glew();
}

void NodeGraphQuickItem::releaseResources() {
  window()->scheduleRenderJob(new CleanUpGL(), QQuickWindow::BeforeSynchronizingStage);
}

void NodeGraphQuickItem::handle_window_changed(QQuickWindow *win) {
  if (win) {

    qDebug() << "handling window changed !!!!!!!\n";

    //connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
    connect(win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
    win->setClearBeforeRendering(false);
  }
}

// This method runs on the scene graph render thread.
// (Note this is different from the main gt gui thread.)
// GL related work is usually done in scene graph render thread and from this method.
// The main gui thread will be paused in this method, so you can
// sync/update the gl state with the QQuickItem state in this class.
// However we do our GL related in a separate thread, but we block in this thread while we wait.
// This makes it easier for us to implement a game loop in the future where
// the scene is continuously being rendered in a separate thread.
// Note we render to a texture and use a QSGSimpleTextureNode to display
// it in the scenegraph.
QSGNode* NodeGraphQuickItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {

  {
    //std::cerr << "OpenGL version " << glGetString(GL_VERSION)<< endl;
    //std::cerr << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
  }

  // Since we render in Qt's opengl context we need to make sure
  // that all of the gl state is left the way we found it.
  // This will restore the gl state on destruction.
  CaptureDeviceState capture_state;
  gpu();

  // Initialize the opengl objects for the fbo render thread.
  if (!_canvas->is_initialized_gl()) {

    if (glewGetContext()==NULL) {
      start_glew(); // Initialize glew for the scene graph render thread.
      gpu();
    }

    g_app_root->initialize_deps();
    g_app_root->update_deps_and_hierarchy();
    g_app_root->initialize_gl();

  }

  // We can only clean ourself on the rendering thread, due to components
  // making use of the gl context.
  //clean();

  // Create a texture display node.
  TextureDisplayNode* node = static_cast<TextureDisplayNode*>(oldNode);
  if (!node) {
    clean();
    node = new_ff TextureDisplayNode();
    QSGTexture* test = _fbo_worker->get_display_texture();
    std::cerr << "fbo worker's qsgtexture is: " << test << "\n";
    node->setTexture(_fbo_worker->get_display_texture());
    node->setRect(boundingRect());
    // Set the initial size.
    get_current_interaction()->resize_gl(_device_pixel_ratio*width(), _device_pixel_ratio*height());
  }

  // Render to a texture in another thread, but we block until it finishes.
  _fbo_worker->render_next_texture();

  // Swap the textures and retrieve the current display texture.
  QSGTexture* display_texture = _fbo_worker->swap_buffers();

  // Update our display node with the newly rendered texture.
  node->setTexture(display_texture);
  node->setRect(boundingRect());
  return node;
}

void NodeGraphQuickItem::geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) {
  QQuickItem::geometryChanged(newGeometry, oldGeometry);
  const Dep<GroupInteraction>& interaction = get_current_interaction();
  if (interaction) {
    interaction->resize_gl(_device_pixel_ratio*newGeometry.width(), _device_pixel_ratio*newGeometry.height());
  }
  update();
}

// Focus overrides.
void NodeGraphQuickItem::focusInEvent(QFocusEvent * event) {
	update();
}

void NodeGraphQuickItem::focusOutEvent(QFocusEvent * event) {
  update();
}


// Mouse overrides.
void NodeGraphQuickItem::mouseDoubleClickEvent(QMouseEvent * event) {
  std::cerr << "received double click event\n";
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  update();
}

void NodeGraphQuickItem::mouseMoveEvent(QMouseEvent * event) {
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::hoverMoveEvent(QHoverEvent * event) {
  MouseInfo info = get_hover_info(event);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::mousePressEvent(QMouseEvent * event) {
  _long_press_timer.start();
  _last_press = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->update_mouse_info(_last_press);
  if (_last_press.middle_button) {
    // Simulate touching with two fingers on a desktop.
    get_current_interaction()->accumulate_select(_last_press, _last_press);
  } else if (_last_press.right_button) {
    // Simulate a long press touch.
    _last_pressed_shape = get_current_interaction()->pressed(_last_press);
    on_long_press();
  } else {
    _last_pressed_shape = get_current_interaction()->pressed(_last_press);
  }
  update();
}

void NodeGraphQuickItem::mouseReleaseEvent(QMouseEvent * event) {
  // Stop the long press timer if
  if (_long_press_timer.isActive()) {
    _long_press_timer.stop();
  }

  // Deal with the release event.
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->released(info);
  update();

  // Save the node graph changes.
  save();
}

void NodeGraphQuickItem::wheelEvent(QWheelEvent *event) {
  WheelInfo info = get_wheel_info(event);
  get_current_interaction()->wheel_rolled(info);
  update();
}

// Key overrides.
void NodeGraphQuickItem::keyPressEvent(QKeyEvent * event) {
  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphQuickItem::keyReleaseEvent(QKeyEvent * event) {
  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphQuickItem::touchEvent(QTouchEvent * event) {
  switch (event->type()) {
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd:
  {
      qDebug() << "got touch event: " << event->type() << "\n";

      QList<QTouchEvent::TouchPoint> touch_points = event->touchPoints();
      qDebug() << "num touch points: " << touch_points.count() << "\n";

      if (!_pinch_mode && (touch_points.count() == 1)) {
        Qt::TouchPointStates state = event->touchPointStates();
        if (state&Qt::TouchPointPressed) {
          _long_press_timer.start();
          _last_press = get_mouse_info(event, _device_pixel_ratio);
          get_current_interaction()->update_mouse_info(_last_press);
          _last_pressed_shape = get_current_interaction()->pressed(_last_press);
          qDebug() << "111 - mouse pressed\n";
          update();
        } else if (state&Qt::TouchPointReleased) {
          // Stop the long press timer if we get a release event
          // before the long press timer fires.
          if (_long_press_timer.isActive()) {
            _long_press_timer.stop();
          }

          // Deal with the release event.
          MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
          get_current_interaction()->released(info);
          qDebug() << "111 - mouse released\n";
          update();
        } else if (state&Qt::TouchPointMoved) {
          MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
          get_current_interaction()->moved(info);
          qDebug() << "111 - mouse moved\n";
          update();
        } else if (state&Qt::TouchPointStationary) {
          qDebug() << "YYYYYYYYYYYYYYYYYYYYYYYYY\n";
        }
      } else if (touch_points.count() == 2) {
        // Wipe any accumulated state from single touches.
        // Note that a double touch is usually preceeded by a single touch
        // as it is difficult to press the screen with two fingers at exactly the same time.
        get_current_interaction()->reset_state();

        // Get the two touch points at two different points in time.
        const QTouchEvent::TouchPoint &touch_a = touch_points.first();
        const QTouchEvent::TouchPoint &touch_b = touch_points.last();
        glm::vec2 a(touch_a.pos().x(),touch_a.pos().y());
        glm::vec2 b(touch_b.pos().x(),touch_b.pos().y());
        glm::vec2 original_a(touch_a.startPos().x(),touch_a.startPos().y());
        glm::vec2 original_b(touch_b.startPos().x(),touch_b.startPos().y());

        // Figure out the zoom facter and origin of the zoom.
        float zoom_factor = glm::length(b-a)/glm::length(original_b-original_a);
        glm::vec2 zoom_origin = 0.5f*(original_a+original_b);

        Qt::TouchPointStates state = event->touchPointStates();
        if (state&Qt::TouchPointPressed) {
          qDebug() << "222 - mouse pressed\n";
          // Do nothing.
        } else if (state&Qt::TouchPointReleased) {
          if ((zoom_factor > 0.95) && (zoom_factor < 1.05)) {
            MouseInfo a = get_mouse_info(event, _device_pixel_ratio, 0);
            MouseInfo b = get_mouse_info(event, _device_pixel_ratio, 1);
            get_current_interaction()->accumulate_select(a,b);
            qDebug() << "222 - mouse released\n";
          }
        } else if (state&Qt::TouchPointMoved) {
          // We official enter into pinch mode.
          _pinch_mode = true;

          // Stop the long press timer.
          if (_long_press_timer.isActive()) {
            _long_press_timer.stop();
          }

          // Perform the pinch zoom.
          get_current_interaction()->pinch_zoom(zoom_origin,zoom_factor);
          qDebug() << "222 - mouse moved\n";
          update();
        } else if (state&Qt::TouchPointStationary) {
          qDebug() << "YYYYYYYYYYYYYYYYYYYYYYYYY\n";
        }
      }

      if (event->type() == QEvent::TouchEnd) {
        _pinch_mode = false;
        // Save the node graph changes.
        save();
      }


      break;
  }
  default:
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
    QQuickItem::touchEvent(event);
      break;
  }
}

//void NodeGraphQuickItem::on_switch_to_mode(int mode) {
//  qDebug() << " NodeGraphQuickItem on switch to mode called with: " << mode << "\n";
//  if (mode == 0) {
//    qDebug() << " NodeGraphQuickItem should now be visible\n";
//    setProperty("visible", true);
//  } else {
//    setProperty("visible", false);
//    qDebug() << " NodeGraphQuickItem should now be hidden\n";
//  }
//  update();
//}

void NodeGraphQuickItem::on_long_press() {
  glm::vec2 d = get_current_interaction()->get_drag_delta();
  // If we haven't moven't that much from the mouse press
  // point during the timer's count down, then we have a
  // real long press action by the user.
  if (glm::length(d)<kLongPressDistanceThreshold) {
    // Revert to selection, before this node was pressed.
    // Pressing on unselected nodes, clears the selection and selects this one node.
    // Long presses reverses that selection change, so that user can decide what to do.
    {
      get_current_interaction()->revert_to_pre_pressed_selection();
      update();
    }

    // Depending on the entity type that the long press is over, we emit
    // signals to request different types of context menus.
    {
      if (_last_pressed_shape) {
        Entity* e = _last_pressed_shape->our_entity();
        size_t did = e->get_did();
        // Send out context menu request signals.
        if (did == kGroupNodeEntity) {
          // Show the group context menu.
          emit group_node_context_menu_requested();
        } else if (e->has<Compute>()) {
          // Show the node context menu.
          emit node_context_menu_requested();
        }
      } else {
        // Show the node graph context menu.
        emit node_graph_context_menu_requested(false);
      }
    }
  }
}

const Dep<GroupInteraction>& NodeGraphQuickItem::get_current_interaction() {
  return _canvas->get_current_interaction();
}

void NodeGraphQuickItem::toggle_selection_under_long_press() {
  get_current_interaction()->toggle_selection_under_press(_last_pressed_shape);
  update();
}

void NodeGraphQuickItem::finish_creating_node(Entity* e, bool centered) {
  e->create_internals();
  e->initialize_deps();
  Dep<CompShape> cs = get_dep<CompShape>(e);
  if (centered) {
    get_current_interaction()->centralize(cs);
  } else {
    cs->set_pos(_last_press.object_space_pos.xy());
  }
  // The parenting group node needs to update its inputs and outputs.
  get_app_root()->update_deps_and_hierarchy();
  update();
}

void NodeGraphQuickItem::create_group_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "group", kGroupNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_input_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "input", kInputNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_output_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "output", kOutputNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_dot_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "dot", kDotNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_mock_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "mock", kMockNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::destroy_selection() {
  _selection->destroy_selection();
  get_app_root()->update_deps_and_hierarchy();
  update();

  // Save the node graph changes.
  save();
}

void NodeGraphQuickItem::dive() {
  // When switching groups we clear the selection.
  // We don't allow inter-group selections.
  _canvas->dive(_last_pressed_shape->our_entity());
  _selection->clear_selection();
  update();
}

void NodeGraphQuickItem::surface() {
  // When switching groups we clear the selection.
  // We don't allow inter-group selections.
  _canvas->surface();
  _selection->clear_selection();
  update();
}

void NodeGraphQuickItem::select_all() {
  get_current_interaction()->select_all();
  update();
}

void NodeGraphQuickItem::deselect_all() {
  get_current_interaction()->deselect_all();
  update();
}

void NodeGraphQuickItem::frame_all() {
  get_current_interaction()->frame_all();
  update();
}

void NodeGraphQuickItem::frame_selected() {
  get_current_interaction()->frame_selected(_selection->get_selected());
  update();
}

QString NodeGraphQuickItem::get_linkable_file() {
  // Make sure the home dir exists.
  QString home_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  if (!QDir(home_dir).exists()) {
    QDir().mkpath(home_dir);
  }

  // Make sure the octoplier dir exists.
  QString linkable_dir = home_dir + "/" + kLinkableDir;
  QFileInfo info(linkable_dir);
  if (!info.exists()) {
    QDir().mkpath(linkable_dir);
  }

  // Return the octoplier file.
  return linkable_dir + "/" + kLinkableFile;
}

void NodeGraphQuickItem::make_save_point() {

}

void NodeGraphQuickItem::save() {
//  // There are times where it is not safe to save,
//  // like in the middle of dragging entities.
//  Interaction* current_interaction = _canvas->get_current_interaction();
//  if (!current_interaction->is_safe_to_save()) {
//    return;
//  }
//
//  // Save out our data to a string.
//  std::stringstream ss;
//  {
//    SimpleSaver saver(ss);
//    GroupSerializer* serializer = _ng_state->switch_to<GroupSerializer>();
//    serializer->save(saver);
//  }
//
//  // If the raw data is exactly the same as our
//  // last save, then we can return right away.
//  if (ss.str() == _last_save_raw) {
//    //qDebug() << "nothing to save";
//    return;
//  } else {
//    qDebug() << "found something different to save !!!!";
//    _last_save_raw = ss.str();
//  }
//
//  // Open the octoplier file.
//  QString linkable_file = get_linkable_file();
//  QFile file(linkable_file);
//  file.open(QIODevice::ReadWrite);
//
//  // Save the string to the file.
//  file.write(_last_save_raw.c_str(), _last_save_raw.size());
//  file.close();
}

void NodeGraphQuickItem::load() {
//  QString  linkable_file = get_linkable_file();
//
//  // If this file doesn't exist there is nothing to load.
//  QFileInfo info(linkable_file);
//  if (!info.exists()) {
//    // Prodedurally create the default node graph.
//    restore_default_node_graph();
//    // This will create the octoplier file.
//    //save();
//    return;
//  }
//
//  // Read all the bytes from the file.
//  QFile file(linkable_file);
//  file.open(QIODevice::ReadOnly);
//  QByteArray contents = file.readAll();
//
//  // Now load them.
//  Bits* bits = create_bits_from_raw(contents.data(),contents.size());
//  SimpleLoader loader(bits);
//
////  std::ifstream ifs;
////  open_input_file_stream("/tmp/ng1.ong", ifs);
////  Bits* bits = create_bits_from_input_stream(ifs);
////  SimpleLoader loader(bits);
//
//  GroupSerializer* serializer = _ng_state->switch_to<GroupSerializer>();
//  serializer->load(loader, true);
//  serializer->switch_to<CompShapeCollective>()->update_component();
//  update();
}

void NodeGraphQuickItem::on_save() {
  // We try to perform save only for every 1 second
  // of idle time has passed in total.
  if (_idle_length.elapsed() < kMinimumIdleLength) {
    return;
  }
  _idle_length.restart();
  save();
}

void NodeGraphQuickItem::on_load() {
  load();
}

void NodeGraphQuickItem::shutdown() {
}

void NodeGraphQuickItem::copy() {
  _selection->copy();
  update();
}

void NodeGraphQuickItem::cut() {
  _selection->copy();
  _selection->destroy_selection();
  // Links may need to be cleaned up.
  get_app_root()->update_deps_and_hierarchy();
  update();

}

void NodeGraphQuickItem::paste(bool centered) {
  const Dep<GroupInteraction>& interaction = get_current_interaction();
  Entity* group = interaction->our_entity();
  _selection->paste(group);

  // At this point the pasting is complete, but we now need to perform adjustments
  // like centering it and selecting the pasted nodes.

  // Clear the current selection.
  _selection->clear_selection();

  // Determine our paste_center.
  glm::vec2 paste_center;
  if (centered) {
    paste_center = interaction->get_center_in_object_space();
  } else {
    paste_center = _last_press.object_space_pos.xy();
  }

  // Get the bounds of the newly created nodes.
  const std::unordered_set<Entity*>& pasted = group->get_last_pasted();

  // Gather all the pasted comp shapes.
  DepUSet<CompShape> comp_shapes;
  for (Entity* e : pasted) {
    Dep<CompShape> cs = get_dep<CompShape>(e);
    if (cs) {
      comp_shapes.insert(cs);
      cs->clean(); // we need them clean as we'll be using their bounds.
    }
  }

  // Get the center of the pasted nodes..
  glm::vec2 min;
  glm::vec2 max;
  CompShapeCollective::get_aa_bounds(comp_shapes, min, max);
  glm::vec2 node_center = 0.5f * (min + max);

  // Get the delta between the node center and the paste center..
  glm::vec2 delta = paste_center - node_center;

  // Move center of the pasted nodes to the paste center.
  for (const Dep<CompShape> &node : comp_shapes) {
    node->set_pos(node->get_pos() + delta);
  }

  // Selected the newly pasted nodes.
  for (const Dep<CompShape> &node : comp_shapes) {
    _selection->select(node);
  }
  update();
}

void NodeGraphQuickItem::restore_default_node_graph() {
  // Clear node graph state.
  _selection->clear_all();
	
  // Clear all existing nodes.
  Entity* root_group = our_entity()->get_child("root");
  root_group->destroy_all_children();
	
  // Add our test nodes.
  //_factory->build_test_graph();
  update();

  // Save the node graph changes.
  //save();
}

void NodeGraphQuickItem::collapse_to_group() {
  get_current_interaction()->collapse_selected();
  update();

  // Save the node graph changes.
  save();
}

void NodeGraphQuickItem::explode_group() {
  get_current_interaction()->explode_selected();
  update();

  // Save the node graph changes.
  save();
}

// Lock Graph.
void NodeGraphQuickItem::lock_graph() {
  _selection->lock();
}

void NodeGraphQuickItem::unlock_graph() {
  _selection->unlock();
}

}

