//#include <base/device/deviceheadersgl.h>
#include <base/memoryallocator/taggednew.h>
#include <base/glewhelper/glewhelper.h>
#include <ngsversion.h>

#include <entities/entityids.h>
#include <components/computes/compute.h>
#include <components/resources/resources.h>
#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>
#include <components/interactions/graphbuilder.h>
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
#include <base/objectmodel/basefactory.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/nodeshape.h>
#include <components/interactions/viewcontrols.h>
#include <guicomponents/comms/basegrouptraits.h>

#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/quick/eventtoinfo.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/texturedisplaynode.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>


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



const int NodeGraphQuickItem::kLongPressTimeThreshold = 10000; //400; // in milli seconds.
const float NodeGraphQuickItem::kLongPressDistanceThreshold = 30; // in object space.
const int NodeGraphQuickItem::kDoublePressTimeThreshold = 300; // max time between release events for a double tap/click.

const int NodeGraphQuickItem::kMinimumIdleLength = 1000; // 1 second in milli seconds.

class CleanUpGL : public QRunnable
{
public:
  CleanUpGL(Entity* app_root):
    _app_root(app_root){}
  void run () {
    std::cerr << "Cleaning up opengl resources.\n";
    _app_root->uninitialize_gl();
  }
private:
  Entity* _app_root;
};

NodeGraphQuickItem::NodeGraphQuickItem(Entity* parent)
    : QQuickItem(NULL),
      Component(parent, kIID(), kDID()),
      _fbo_worker(this),
      _selection(this),
      _canvas(this),
      _factory(this),
      _file_model(this),
      _license_checker(this),
      _ng_manipulator(this),
      _last_pressed_node(this),
      _link_locked(false) {

  get_dep_loader()->register_fixed_dep(_fbo_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_selection, Path({}));
  get_dep_loader()->register_fixed_dep(_canvas, Path({}));
  get_dep_loader()->register_fixed_dep(_factory, Path({}));
  get_dep_loader()->register_fixed_dep(_file_model, Path({}));
  get_dep_loader()->register_fixed_dep(_license_checker, Path({}));
  get_dep_loader()->register_fixed_dep(_ng_manipulator, Path({}));

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
}

NodeGraphQuickItem::~NodeGraphQuickItem() {
  setParentItem(NULL);
}

//  #if ARCH == ARCH_ANDROID
//      {
//        QSurfaceFormat format = window()->format();
//        format.setProfile(QSurfaceFormat::CoreProfile);
//        format.setRenderableType(QSurfaceFormat::OpenGLES);
//        format.setVersion(3, 0);
//        window()->setFormat(format);
//      }
//  #endif

bool NodeGraphQuickItem::update_state() {
  internal();
  lock_links(_file_model->get_work_setting(FileModel::kLockLinksRole).toBool());
  return true;
}

// This gets called from the scene graph render thread.
void NodeGraphQuickItem::cleanup() {
  internal();
  qDebug() <<  "node graph quick item doing cleanup \n";
  get_app_root()->uninitialize_gl();
  finish_glew();
}

void NodeGraphQuickItem::releaseResources() {
  internal();
  window()->scheduleRenderJob(new CleanUpGL(get_app_root()), QQuickWindow::BeforeSynchronizingStage);
}

void NodeGraphQuickItem::handle_window_changed(QQuickWindow *win) {
  internal();
  if (win) {
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
  internal();
  {
    //std::cerr << "OpenGL version " << glGetString(GL_VERSION)<< endl;
    //std::cerr << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
  }

  // The width and height should not be empty at this point.
  // Other you will get fbo gl errors.
  assert(width() != 0 && height() != 0);

  // Since we render in Qt's opengl context we need to make sure
  // that all of the gl state is left the way we found it.
  // This will restore the gl state on destruction.
#if ARCH != ARCH_MACOS
  CaptureDeviceState capture_state;
  gpu();
#endif

  // Initialize the opengl objects for the fbo render thread.
  if (!_canvas->is_initialized_gl()) {

    if (glewGetContext()==NULL) {
      start_glew(); // Initialize glew for the scene graph render thread.
      gpu();
    }

    get_app_root()->clean_wires();
    get_app_root()->initialize_gl();

    // Set the initial size.
    get_current_interaction()->resize_gl(_device_pixel_ratio*width(), _device_pixel_ratio*height());
  }

  // Create a texture display node.
  TextureDisplayNode* node = static_cast<TextureDisplayNode*>(oldNode);
  if (!node) {
    node = new_ff TextureDisplayNode();
    QSGTexture* test = _fbo_worker->get_display_texture();
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
  internal();
  QQuickItem::geometryChanged(newGeometry, oldGeometry);
  const Dep<GroupInteraction>& interaction = get_current_interaction();
  if (interaction) {
    interaction->resize_gl(_device_pixel_ratio*newGeometry.width(), _device_pixel_ratio*newGeometry.height());
  }
  update();
}

// Focus overrides.
void NodeGraphQuickItem::focusInEvent(QFocusEvent * event) {
  internal();
	update();
}

void NodeGraphQuickItem::focusOutEvent(QFocusEvent * event) {
  internal();
  update();
}


// Mouse overrides.
void NodeGraphQuickItem::mouseDoubleClickEvent(QMouseEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  update();
}

void NodeGraphQuickItem::mouseMoveEvent(QMouseEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::hoverMoveEvent(QHoverEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  MouseInfo info = get_hover_info(event);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::mousePressEvent(QMouseEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  _long_press_timer.start();
  _last_press = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->update_mouse_info(_last_press);

  if (_link_locked && _last_press.left_button) {
    // We only allow the press to go through if a node is pressed
    // or the background is pressed.
    if (get_current_interaction()->node_hit(_last_press) ||
        get_current_interaction()->bg_hit(_last_press)) {
      _last_pressed_node = get_current_interaction()->pressed(_last_press);
    }
  } else {
    if (_last_press.middle_button) {
      // Simulate touching with two fingers on a desktop.
      get_current_interaction()->accumulate_select(_last_press, _last_press);
    } else if (_last_press.right_button) {
      // Simulate a long press touch.
      _last_pressed_node = get_current_interaction()->pressed(_last_press);
      _long_press_timer.stop();
      popup_context_menu();
    } else {
      _last_pressed_node = get_current_interaction()->pressed(_last_press);
    }
  }
  update();
}

void NodeGraphQuickItem::mouseReleaseEvent(QMouseEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }

  // Stop the long press timer if
  if (_long_press_timer.isActive()) {
    _long_press_timer.stop();
  }

  // Deal with the release event.
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->released(info);
  update();
}

void NodeGraphQuickItem::wheelEvent(QWheelEvent *event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  WheelInfo info = get_wheel_info(event);
  get_current_interaction()->wheel_rolled(info);
  update();
}

// Key overrides.
void NodeGraphQuickItem::keyPressEvent(QKeyEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphQuickItem::keyReleaseEvent(QKeyEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphQuickItem::touchEvent(QTouchEvent * event) {
  internal();
  if (!_license_checker->license_is_valid()) {
    return;
  }
  if (!get_current_interaction()) {
    return;
  }
  switch (event->type()) {
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd:
  {
      qDebug() << "got touch event: " << event->type() << "\n";
      QList<QTouchEvent::TouchPoint> touch_points = event->touchPoints();
      qDebug() << "num touch points: " << touch_points.count() << "\n";

      if (touch_points.count() == 1) {
        Qt::TouchPointStates state = event->touchPointStates();
        if (state&Qt::TouchPointPressed) {
          _long_press_timer.start();
          _last_press = get_mouse_info(event, _device_pixel_ratio);
          get_current_interaction()->update_mouse_info(_last_press);

          if (_link_locked) {
            // We only allow the press to go through if a node is pressed
            // or the background is pressed.
            if (get_current_interaction()->node_hit(_last_press) ||
                get_current_interaction()->bg_hit(_last_press)) {
              _last_pressed_node = get_current_interaction()->pressed(_last_press);
            }
          } else {
            _last_pressed_node = get_current_interaction()->pressed(_last_press);
          }
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
          update();
        } else if (state&Qt::TouchPointMoved) {
          MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
          get_current_interaction()->moved(info);
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

        // Figure out the zoom facter and origin of the zoom.
        glm::vec2 a = (float)_device_pixel_ratio * glm::vec2(touch_a.pos().x(),touch_a.pos().y());
        glm::vec2 b = (float)_device_pixel_ratio * glm::vec2(touch_b.pos().x(),touch_b.pos().y());

        Qt::TouchPointStates state = event->touchPointStates();
        if (state&Qt::TouchPointPressed) {
          _original_pinch_a = a;
          _original_pinch_b = b;
          _original_pinch_center=0.5f*(_original_pinch_a+_original_pinch_b);
        } else if (state&Qt::TouchPointReleased) {
          float zoom_factor = glm::length(b-a)/glm::length(_original_pinch_b-_original_pinch_a);
          if ((zoom_factor > 0.95) && (zoom_factor < 1.05)) {
            MouseInfo a = get_mouse_info(event, _device_pixel_ratio, 0);
            MouseInfo b = get_mouse_info(event, _device_pixel_ratio, 1);
            get_current_interaction()->accumulate_select(a,b);
          }
          get_current_interaction()->finalize_pinch_zoom();
        } else if (state&Qt::TouchPointMoved) {
          // Stop the long press timer.
          if (_long_press_timer.isActive()) {
            _long_press_timer.stop();
          }
          // Perform the pinch zoom.
          float zoom_factor = glm::length(b-a)/glm::length(_original_pinch_b-_original_pinch_a);
          get_current_interaction()->pinch_zoom(_original_pinch_center,zoom_factor);
          update();
        } else if (state&Qt::TouchPointStationary) {
          qDebug() << "YYYYYYYYYYYYYYYYYYYYYYYYY\n";
        }
      }

      if (event->type() == QEvent::TouchEnd) {
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

void NodeGraphQuickItem::popup_context_menu() {
  internal();
  // Make sure the interaction is not in the middle of an action like dragging a link.
  get_current_interaction()->reset_state();

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
    if (_last_pressed_node) {
      Entity* e = _last_pressed_node->our_entity();
      EntityDID did = e->get_did();
      // Send out context menu request signals.
      if (e->has<GroupInteraction>()) {
        // Show the group context menu.
        emit group_node_context_menu_requested();
      } else if (e->has<Compute>()) {
        Dep<CompShape> cs = get_dep<CompShape>(e);
        // Show the node context menu.
        if (cs->is_linkable()) {
          emit node_context_menu_requested();
        }
      }
    } else {
      // Show the node graph context menu.
      emit node_graph_context_menu_requested(false);
    }
  }
}

void NodeGraphQuickItem::on_long_press() {
  internal();
  glm::vec2 d = get_current_interaction()->get_drag_delta();

  // If we haven't moven't that much from the mouse press
  // point during the timer's count down, then we have a
  // real long press action by the user.
  if (glm::length(d)<kLongPressDistanceThreshold) {
    popup_context_menu();
  }
}

const Dep<GroupInteraction>& NodeGraphQuickItem::get_current_interaction() const {
  external();
  return _canvas->get_current_interaction();
}

void NodeGraphQuickItem::toggle_selection_under_long_press() {
  external();
  if (!_last_pressed_node) {
    return;
  }
  get_current_interaction()->toggle_selection_under_press(_last_pressed_node);
  update();
}

QString NodeGraphQuickItem::get_ngs_version() const {
  external();
  return NGS_VERSION;
}

size_t NodeGraphQuickItem::get_num_nodes() const {
  external();
  // 3 is for the group node entities namespace folders (inputs, outputs, links).
  return get_current_interaction()->our_entity()->get_children().size() -3;
}

void NodeGraphQuickItem::finish_creating_node(Entity* e, bool centered) {
  external();
  e->create_internals();
  e->initialize_wires();
  Dep<NodeShape> cs = get_dep<NodeShape>(e);
  if (centered) {
    get_current_interaction()->centralize(cs);
  } else {
    cs->set_pos(_last_press.object_space_pos.xy());
  }
  // The parenting group node needs to update its inputs and outputs.
  //get_app_root()->update_wires();
  _selection->clear_selection();
  _selection->select(cs);

  // A little overkill, but we clean the wires on everything in this group.
  // Otherwise after creating nodes like mock or input or output nodes,
  // they appear with their input and outplugs not showing.
  our_entity()->clean_wires();
  update();
}

void NodeGraphQuickItem::create_group_node(bool centered) {
  external();
  Entity* e = _factory->instance_entity(get_current_interaction()->our_entity(), "group", EntityDID::kGroupNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_web_group_node(bool centered) {
  external();
  Entity* e = _factory->instance_entity(get_current_interaction()->our_entity(), "web group", EntityDID::kWebGroupNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_data_node(bool centered) {
  external();
  Entity* e = _factory->instance_entity(get_current_interaction()->our_entity(), "data", EntityDID::kDataNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_input_node(bool centered) {
  external();
  Entity* e = _factory->instance_entity(get_current_interaction()->our_entity(), "input", EntityDID::kInputNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_output_node(bool centered) {
  external();
  Entity* e = _factory->instance_entity(get_current_interaction()->our_entity(), "output", EntityDID::kOutputNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_dot_node(bool centered) {
  external();
  Entity* e = _factory->instance_entity(get_current_interaction()->our_entity(), "dot", EntityDID::kDotNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_compute_node(bool centered, ComponentDID compute_did) {
  external();
  Entity* e = _factory->create_compute_node(get_current_interaction()->our_entity(), compute_did);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::process_node() {
  external();
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    // Update our node graph selection object which also tracks and edit and view nodes.
    get_current_interaction()->process(_last_pressed_node);
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::dirty_node() {
  external();
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    // Update our node graph selection object which also tracks and edit and view nodes.
    compute->dirty_state();
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::clean_node() {
  external();
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    // Update our node graph selection object which also tracks and edit and view nodes.
    _ng_manipulator->set_ultimate_target(compute->our_entity(), true);
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::view_node() {
  external();
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }

  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    qDebug() << "performing compute! \n";
    //_ng_manipulator->set_ultimate_target(compute->our_entity(), true);

    QVariantMap submap;
    submap.insert("xxnumber int", 123);
    submap.insert("xxstring", "booya man");
    submap.insert("xxnumber float", 234.545);
    submap.insert("xxboolean", true);
    submap.insert("xxboolean2", false);

    QVariantList sublist;
    sublist.push_back(123);
    sublist.push_back("hello");
    sublist.push_back(434.523);
    sublist.push_back(true);
    sublist.push_back(false);

    QVariantMap test;
    test.insert("number int", 123);
    test.insert("string", "booya man");
    test.insert("number float", 234.545);
    test.insert("boolean", true);
    test.insert("boolean2", false);
    test.insert("submap", submap);
    test.insert("subarray", sublist);


    emit view_node_outputs(compute->our_entity()->get_name().c_str(), compute->get_outputs());

    // Update our node graph selection object which also tracks and edit and view nodes.
    get_current_interaction()->view(_last_pressed_node);
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::edit_node() {
  external();
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }

  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    qDebug() << "performing compute! \n";
    //compute->propagate_cleanliness();

    QVariantMap submap;
    submap.insert("xxnumber int", 123);
    submap.insert("xxstring", "booya man");
    submap.insert("xxnumber float", 234.545);
    submap.insert("xxboolean", true);
    submap.insert("xxboolean2", false);

    QVariantList sublist;
    sublist.push_back(123);
    sublist.push_back("hello");
    sublist.push_back(434.523);
    sublist.push_back(true);
    sublist.push_back(false);

    QVariantMap test;
    test.insert("number", 1);
    test.insert("string", "booya man");
    test.insert("number float", 234.545);
    test.insert("boolean", true);
    test.insert("boolean2", false);
    test.insert("submap", submap);
    test.insert("subarray", sublist);

    QVariantMap hints;
    hints[QString::number(to_underlying(HintType::kEnum))] = to_underlying(EnumHint::kWrapType);
    hints[QString::number(to_underlying(HintType::kDescription))] = "this is a bogus parameter for a bogus set of hints do asdfj asfkdj werkj sdkfj asfd asfd wlekrj afnkj weroiu  afslkdj  wer lk sfd werlkj w sdlfkj  flakjsd f wlekrj wler a fsdlkjas dflk woer we r af alskdjf a f wler wler l f aslkdjflksjdf  we rlwkejr s df owiueroi sdfs dlfkjs dlf weor sldjf ";
    QVariantMap all_hints;
    all_hints.insert("number", hints);

//    std::cerr << "hints size: " << compute->get_hints().size() << "\n";
//    QVariantMap test2 = compute->get_hints();
//    for(QVariantMap::const_iterator iter = test2.begin(); iter != test2.end(); ++iter) {
//      qDebug() << "name: " << iter.key();
//      QVariantMap map = iter.value().toMap();
//      for(QVariantMap::const_iterator iter2 = map.begin(); iter2 != map.end(); ++iter2) {
//        qDebug() << iter2.key() << iter2.value();
//      }
//    }

    compute->update_input_flux();
    emit edit_node_inputs(compute->our_entity()->get_name().c_str(),
                          compute->get_editable_inputs(),
                          compute->get_hints(),
                          compute->get_input_exposure());
    //emit edit_node_inputs(compute->our_entity()->get_name().c_str(), test, all_hints);

    // Update our node graph selection object which also tracks and edit and view nodes.
    get_current_interaction()->edit(_last_pressed_node);
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::set_editable_inputs(const QVariantMap& values) {
  if (!_last_pressed_node) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    compute->set_editable_inputs(values);
  }
}

void NodeGraphQuickItem::set_input_exposure(const QVariantMap& values) {
  if (!_last_pressed_node) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    compute->set_input_exposure(values);
    _factory->get_current_group()->clean_wires();
    _factory->get_current_group()->clean_dead_entities();
    update();
  }
}

void NodeGraphQuickItem::destroy_selection() {
  external();
  _selection->destroy_selection();
  get_app_root()->clean_dead_entities();
  update();
}

void NodeGraphQuickItem::dive() {
  external();
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }

  // Let the group traits performs its transition behavior.
  Entity* group_entity = _last_pressed_node->our_entity();
  Dep<BaseGroupTraits> t = get_dep<BaseGroupTraits>(group_entity);
  t->on_enter();

  // Switch to the next group on the canvas.
  _canvas->dive(group_entity);

  // We clear the selection because we don't allow inter-group selections.
  _selection->clear_selection();
  frame_all();
  update();
}

void NodeGraphQuickItem::surface() {
  external();

  // Let the group traits performs its transition behavior.
  Entity* group_entity = _factory->get_current_group();
  Dep<BaseGroupTraits> t = get_dep<BaseGroupTraits>(group_entity);
  t->on_exit();

  // Switch to the next group on the canvas.
  _canvas->surface();

  // We clear the selection, because don't allow inter-group selections.
  _selection->clear_selection();
  update();
}

void NodeGraphQuickItem::surface_to_root() {
  external();
  _canvas->surface_to_root();
  update();
}

void NodeGraphQuickItem::select_last_press() {
  external();
  get_current_interaction()->select(_last_pressed_node);
  update();
}

void NodeGraphQuickItem::deselect_last_press() {
  external();
  get_current_interaction()->deselect(_last_pressed_node);
  update();
}

void NodeGraphQuickItem::select_all() {
  external();
  get_current_interaction()->select_all();
  update();
}

void NodeGraphQuickItem::deselect_all() {
  external();
  get_current_interaction()->deselect_all();
  update();
}

void NodeGraphQuickItem::frame_all() {
  external();
  get_current_interaction()->frame_all();
  update();
}

void NodeGraphQuickItem::frame_selected() {
  external();
  get_current_interaction()->frame_selected(_selection->get_selected());
  update();
}

void NodeGraphQuickItem::save() {
  external();
  _file_model->save_graph();
}

void NodeGraphQuickItem::load() {
  external();
  _file_model->load_graph();
}

void NodeGraphQuickItem::copy() {
  external();
  _selection->copy();
  update();
}

void NodeGraphQuickItem::cut() {
  external();
  _selection->copy();
  _selection->destroy_selection();
  // Clean out dead links and update the shape collective.
  const Dep<GroupInteraction>& interaction = get_current_interaction();
  interaction->clean_dead_links();
  interaction->update_shape_collective();
  update();
}

void NodeGraphQuickItem::paste(bool centered) {
  external();
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
  DepUSet<NodeShape> node_shapes;
  for (Entity* e : pasted) {
    Dep<NodeShape> ns = get_dep<NodeShape>(e);
    if (ns) {
      node_shapes.insert(ns);
      ns->clean_state(); // we need them clean as we'll be using their bounds.
    }
  }

  // Get the center of the pasted nodes..
  glm::vec2 min;
  glm::vec2 max;
  CompShapeCollective::get_aa_bounds(node_shapes, min, max);
  glm::vec2 node_center = 0.5f * (min + max);

  // Get the delta between the node center and the paste center..
  glm::vec2 delta = paste_center - node_center;

  // Move center of the pasted nodes to the paste center.
  for (const Dep<NodeShape> &node : node_shapes) {
    node->set_pos(node->get_pos() + delta);
  }

  // Selected the newly pasted nodes.
  for (const Dep<NodeShape> &node : node_shapes) {
    _selection->select(node);
  }

  // Clean out dead links and update the shape collective.
  interaction->clean_dead_links();
  interaction->update_shape_collective();
  update();
}

void NodeGraphQuickItem::collapse_to_group() {
  external();
  get_current_interaction()->collapse_selected();
  update();
}

void NodeGraphQuickItem::explode_group() {
  external();
  get_current_interaction()->explode_selected();
  update();
}

// Lock Graph.
void NodeGraphQuickItem::lock_links(bool locked) {
  external();
  _link_locked = locked;
}

bool NodeGraphQuickItem::links_are_locked() const {
  external();
  return _link_locked;
}

void NodeGraphQuickItem::view_node_poke() {
  _last_pressed_node = _selection->get_view_node();
  if (!_last_pressed_node) {
    emit view_node_outputs("no view node selected", QVariantMap());
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  emit view_node_outputs(compute->our_entity()->get_name().c_str(), compute->get_outputs());
}

void NodeGraphQuickItem::edit_node_poke() {
  _last_pressed_node = _selection->get_edit_node();
  if (!_last_pressed_node) {
    emit edit_node_inputs("no edit node selected", QVariantMap(), QVariantMap(), QVariantMap());
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  compute->update_input_flux();
  emit edit_node_inputs(compute->our_entity()->get_name().c_str(), compute->get_editable_inputs(), compute->get_hints(), compute->get_input_exposure());
}

}

