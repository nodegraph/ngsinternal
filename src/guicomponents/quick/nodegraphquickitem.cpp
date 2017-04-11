#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/eventtoinfo.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/texturedisplaynode.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <components/computes/groupnodecompute.h>
#include <guicomponents/comms/licensechecker.h>

#include <ngsversion.h>
#include <base/memoryallocator/taggednew.h>
#include <base/glewhelper/glewhelper.h>
#include <base/device/devicedebug.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/entity.h>

#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>

#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/nodeshape.h>

#include <QtQuick/QQuickWindow>
#include <QtCore/QRunnable>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QClipboard>

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
      _manipulator(this),
      _last_node_shape(this) {

  get_dep_loader()->register_fixed_dep(_fbo_worker, Path());
  get_dep_loader()->register_fixed_dep(_selection, Path());
  get_dep_loader()->register_fixed_dep(_canvas, Path());
  get_dep_loader()->register_fixed_dep(_factory, Path());
  get_dep_loader()->register_fixed_dep(_file_model, Path());
  get_dep_loader()->register_fixed_dep(_license_checker, Path());
  get_dep_loader()->register_fixed_dep(_manipulator, Path());

  _device_pixel_ratio = static_cast<GLsizei>(QGuiApplication::primaryScreen()->devicePixelRatio());

  // Configure.
  setAcceptedMouseButtons(Qt::AllButtons);
  setAcceptHoverEvents(true);
  setFlag(QQuickItem::ItemHasContents, true);
  setFlag(QQuickItem::ItemAcceptsInputMethod, true);
  this->setFocus(true);

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

void NodeGraphQuickItem::copy_to_clipboard(const QString& text) {
  QClipboard *clipboard = QGuiApplication::clipboard();
  clipboard->setText(text);
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
  //std::cerr << "ng quick item locking links\n";
  //lock_links(_file_model->get_work_setting(FileModel::kLockLinksRole).toBool());
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
  if (!get_current_interaction()) {
    return;
  }
  if (_manipulator->is_busy_cleaning()) {
    emit set_error_message("Please wait while the node graph finishes cleaning.");
    emit show_error_page();
    return;
  }

  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);

  // Note that mousePressEvent is called twice before a double click is triggered.
  if (_last_node_shape) {
    if (_last_node_shape->our_entity()->has_group_related_did()) {
      _manipulator->dive_into_group(_last_node_shape->get_name());
    }
  } else {
    _manipulator->surface_from_group();
  }
  update();
}

void NodeGraphQuickItem::mouseMoveEvent(QMouseEvent * event) {
  internal();
  if (!get_current_interaction()) {
    return;
  }
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::hoverMoveEvent(QHoverEvent * event) {
  internal();
  if (!get_current_interaction()) {
    return;
  }
  MouseInfo info = get_hover_info(event);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::mousePressEvent(QMouseEvent * event) {
  internal();
  if (!get_current_interaction()) {
    return;
  }
  _long_press_timer.start();
  _last_press = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->update_mouse_info(_last_press);

  HitRegion region;

  if (_file_model->links_are_locked() && _last_press.left_button) {
    // We only allow the press to go through if a node is pressed
    // or the background is pressed.
    if (get_current_interaction()->node_hit(_last_press) ||
        get_current_interaction()->bg_hit(_last_press)) {
      _last_node_shape = get_current_interaction()->pressed(_last_press, region);
    }
  } else {
    if (_last_press.control_modifier || _last_press.middle_button) {
      // Simulate touching with two fingers on a desktop.
      get_current_interaction()->accumulate_select(_last_press, _last_press);
    } else if (_last_press.right_button) {
      // Simulate a long press touch.
      _last_node_shape = get_current_interaction()->pressed(_last_press, region);
      _long_press_timer.stop();
      popup_context_menu();
    } else {
      _last_node_shape = get_current_interaction()->pressed(_last_press, region);
    }
  }
  update();

  switch(region) {
    case HitRegion::kEditMarkerRegion: {
      edit_node();
      break;
    }
    case HitRegion::kViewMarkerRegion: {
      view_node();
      break;
    }
    case HitRegion::kProcessingMarkerRegion: {
      // do nothing.
      break;
    }
    case HitRegion::kCleanMarkerRegion: {
      view_node();
      break;
    }
    case HitRegion::kErrorMarkerRegion: {
      emit show_error_page();
      break;
    }
  }
}

void NodeGraphQuickItem::mouseReleaseEvent(QMouseEvent * event) {
  internal();
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
  if (!get_current_interaction()) {
    return;
  }
  WheelInfo info = get_wheel_info(event);
  get_current_interaction()->wheel_rolled(info);
  update();
}

#define if_busy_return() \
    if (_manipulator->is_busy_cleaning()) {\
        emit set_error_message("Please wait while the node graph finishes cleaning.");\
        emit show_error_page();\
        return;\
      }

// Key overrides.
void NodeGraphQuickItem::keyPressEvent(QKeyEvent * event) {
  internal();
  if (!get_current_interaction()) {
    return;
  }
  // See if the pressed key is a hotkey that we can handle.
  KeyInfo info = get_key_info_qt(event);
  int kc = info.key_code;
  bool ctrl = info.control_modifier;
  const MouseInfo& mouse_info = get_current_interaction()->get_mouse_over_info();
  Entity* entity = get_current_interaction()->entity_hit(mouse_info);
  if (entity) {
    if (kc == Qt::Key_E && !ctrl) {
      if_busy_return();
      _last_node_shape = get_dep<NodeShape>(entity);
      edit_node();
      return;
    } else if (kc == Qt::Key_V && !ctrl) {
      if_busy_return();
      _last_node_shape = get_dep<NodeShape>(entity);
      view_node();
      return;
    } else if (kc == Qt::Key_C && !ctrl) {
      if_busy_return();
      _last_node_shape = get_dep<NodeShape>(entity);
      clean_node();
      return;
    } else if (kc == Qt::Key_R && !ctrl) {
      if_busy_return();
      _last_node_shape = get_dep<NodeShape>(entity);
      reclean_node();
      return;
    } else if (kc == Qt::Key_D && !ctrl) {
      if_busy_return();
      _last_node_shape = get_dep<NodeShape>(entity);
      dirty_node();
      return;
    } else if (kc == Qt::Key_X && !ctrl) {
      if_busy_return();
      explode_group(entity);
    }
  }

  if (kc == Qt::Key_C  && !ctrl) {
    // We don't allow this, as it's too easy to perform accidentally.
    //if_busy_return();
    //clean_group();
    return;
  } else if (kc == Qt::Key_R  && !ctrl) {
    // We don't allow this, as it's too easy to perform accidentally.
    //if_busy_return();
    //reclean_group();
    return;
  } else if (kc == Qt::Key_D  && !ctrl) {
    // We don't allow this, as it's too easy to perform accidentally.
    //if_busy_return();
    //dirty_group();
    return;
  } else if (kc == Qt::Key_F && !ctrl) {
    if_busy_return();
    frame_all();
    return;
  } else if (kc == Qt::Key_F && ctrl) {
    if_busy_return();
    frame_selected();
    return;
  } else if (kc == Qt::Key_C && ctrl) {
    if_busy_return();
    copy();
    return;
  } else if (kc == Qt::Key_X && ctrl) {
    if_busy_return();
    cut();
    return;
  } else if (kc == Qt::Key_V && ctrl) {
    if_busy_return();
    paste(true);
    return;
  } else if (kc == Qt::Key_A && ctrl) {
    if_busy_return();
    select_all();
    return;
  } else if (kc == Qt::Key_D && ctrl) {
    if_busy_return();
    deselect_all();
    return;
  } else if (kc == Qt::Key_P && !ctrl) {
    if_busy_return();
    collapse_to_group();
  } else if (kc == Qt::Key_Delete || kc == Qt::Key_Backspace) {
    if_busy_return();
    destroy_selection();
  }

  update();
}

void NodeGraphQuickItem::keyReleaseEvent(QKeyEvent * event) {
  internal();
  if (!get_current_interaction()) {
    return;
  }
  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphQuickItem::touchEvent(QTouchEvent * event) {
  internal();
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

          HitRegion region;
          if (_file_model->links_are_locked()) {
            // We only allow the press to go through if a node is pressed
            // or the background is pressed.
            if (get_current_interaction()->node_hit(_last_press) ||
                get_current_interaction()->bg_hit(_last_press)) {
              _last_node_shape = get_current_interaction()->pressed(_last_press, region);
            }
          } else {
            _last_node_shape = get_current_interaction()->pressed(_last_press, region);
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
    if (_last_node_shape) {
      Entity* e = _last_node_shape->our_entity();
      EntityDID did = e->get_did();
      // Send out context menu request signals.
      if (e->has_group_related_did() && (!e->has_macro_related_did())) {
        // Show the group context menu.
        emit switch_to_mode(to_underlying(GUITypes::AppMode::GroupNodeMenuMode));
      } else if (e->has<Compute>()) {
        Dep<CompShape> cs = get_dep<CompShape>(e);
        // Show the node context menu.
        if (cs->is_linkable()) {
          emit switch_to_mode(to_underlying(GUITypes::AppMode::NodeMenuMode));
        }
      }
    } else {
      // Show the node graph context menu.
      emit switch_to_mode(to_underlying(GUITypes::AppMode::NGMenuMode));
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
  if (!_last_node_shape) {
    return;
  }
  get_current_interaction()->toggle_selection_under_press(_last_node_shape);
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

void NodeGraphQuickItem::dirty_node() {
  external();
  if (_manipulator->is_busy_cleaning()) {
    return;
  }
  // Return if don't have a last pressed shape.
  if (!_last_node_shape) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_node_shape->our_entity());
  if(compute) {
    // Update our node graph selection object which also tracks and edit and view nodes.
    compute->dirty_state();
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::dirty_group() {
  external();
  Entity* group = _factory->get_current_group();
  Dep<GroupNodeCompute> compute = get_dep<GroupNodeCompute>(group);
  compute->dirty_all_nodes_in_group();
  update();
}

void NodeGraphQuickItem::dirty_group_recursively() {
  external();
  Entity* group = _factory->get_current_group();
  Dep<GroupNodeCompute> compute = get_dep<GroupNodeCompute>(group);
  compute->dirty_all_nodes_in_group_recursively();
  update();
}

void NodeGraphQuickItem::clean_group() {
  external();
  Entity* group = _factory->get_current_group();
  _manipulator->set_ultimate_targets(group, true);
}

void NodeGraphQuickItem::reclean_group() {
  external();
  dirty_group_recursively();
  clean_group();
}

void NodeGraphQuickItem::clean_node() {
  external();
  if (_manipulator->is_busy_cleaning()) {
    return;
  }
  // Return if don't have a last pressed shape.
  if (!_last_node_shape) {
    return;
  }

  Dep<Compute> compute = get_dep<Compute>(_last_node_shape->our_entity());
  if(compute) {
    // Update our node graph selection object which also tracks and edit and view nodes.
    _manipulator->set_ultimate_targets(compute->our_entity(), true);
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::reclean_node() {
  external();
  if (_manipulator->is_busy_cleaning()) {
    return;
  }
  // Return if don't have a last pressed shape.
  if (!_last_node_shape) {
    return;
  }


  dirty_group_recursively();
  clean_node();
}

void NodeGraphQuickItem::view_node() {
  external();
  if (_manipulator->is_busy_cleaning()) {
    return;
  }
  // Return if don't have a last pressed shape.
  if (!_last_node_shape) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_node_shape->our_entity());
  if(compute) {
    QString node_path = compute->get_path_as_string().c_str();
    emit view_node_outputs(node_path, compute->get_outputs());
    // Update our node graph selection object which also tracks and edit and view nodes.
    get_current_interaction()->view(_last_node_shape);
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::edit_node() {
  external();
  if (_manipulator->is_busy_cleaning()) {
    return;
  }
  // Return if don't have a last pressed shape.
  if (!_last_node_shape) {
    return;
  }

  Entity* entity = _last_node_shape->our_entity();
  Dep<Compute> compute(this);
  compute = get_dep<Compute>(entity);

  if(compute) {
    QString node_path = compute->get_path_as_string().c_str();
    emit edit_node_inputs(node_path,
                          compute->get_editable_inputs(),
                          compute->get_hints(),
                          compute->get_input_exposure());
    // Update our node graph selection object which also tracks and edit and view nodes.
    get_current_interaction()->edit(_last_node_shape);
    update();
  }
}

void NodeGraphQuickItem::edit_main_macro_node() {
  Entity* e = has_entity(Path({"root","main_macro"}));
  if (!e) {
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(e);
  if (!compute) {
    return;
  }

  emit edit_node_inputs("/root/main_macro",
                        compute->get_editable_inputs(),
                        compute->get_hints(),
                        compute->get_input_exposure());
}


void NodeGraphQuickItem::set_editable_inputs(const QString& node_path, const QJsonObject& values) {
  Path path = Path::string_to_path(node_path.toStdString());
  Entity* entity = get_app_root()->get_entity(path);
  Dep<Compute> compute = get_dep<Compute>(entity);
  if(compute) {
    compute->set_editable_inputs(values);
  }
}

void NodeGraphQuickItem::set_input_exposure(const QString& node_path, const QJsonObject& values) {
  Path path = Path::string_to_path(node_path.toStdString());
  Entity* entity = get_app_root()->get_entity(path);
  Dep<Compute> compute = get_dep<Compute>(entity);
  if(compute) {
    compute->set_input_exposure(values);
    _factory->get_current_group()->clean_wires();
    _factory->get_current_group()->clean_dead_entities();
    update();
  }
}

void NodeGraphQuickItem::destroy_selection() {
  external();

  // Copy the current selection.
  const DepUSet<NodeShape> selection = _selection->get_selected();

  // Clear the selection.
  _selection->clear_selection();

  // Destroy the selected nodes.
  for(const Dep<NodeShape>& cs: selection) {
    if (!cs) {
      continue;
    }
    Entity* e = cs->our_entity();
    Entity* group = e->get_parent();

    // Nodes that are not visible can never be selected. However just in case we skip invisible nodes.
    if (!cs->is_visible()) {
      continue;
    }

    // Nodes in certain groups cannot be destroyed.
    if (e->is_topologically_fixed()) {
      QString msg = "Some nodes are required by the surrounding group and cannot be removed.";
      emit set_error_message(msg);
      emit show_error_page();
      continue;
    }

    // Otherwise we destroy it.
    delete_ff(e);
  }

  get_app_root()->clean_dead_entities();
  update();
}

void NodeGraphQuickItem::dive_into_group(const std::string& child_group_name) {
  // Find the child group.
  Entity* group_entity =_factory->get_current_group()->get_child(child_group_name);
  if (!group_entity) {
	  std::cerr << "Warning: could't find group to dive into: " << child_group_name << "\n";
	  return;
  }

  // If the child isn't a group or doesn't exist, then return.
  if (!get_dep<GroupInteraction>(group_entity)) {
    return;
  }

  // We don't allow diving into app and user macros.
  if ((group_entity->get_did() == EntityDID::kPublicMacroNodeEntity) ||
      (group_entity->get_did() == EntityDID::kPrivateMacroNodeEntity) ||
      (group_entity->get_did() == EntityDID::kAppMacroNodeEntity)) {
    QString msg = "Diving into app and user macros is not permitted.";
    emit set_error_message(msg);
    emit show_error_page();
    return;
  }

  // Dive into the group.
  _canvas->dive(group_entity);

  // We clear the selection because we don't allow inter-group selections.
  _selection->clear_selection();
  _selection->clear_error_node();
  frame_all();
  update();
}

void NodeGraphQuickItem::surface() {
  external();

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
  get_current_interaction()->select(_last_node_shape);
  update();
}

void NodeGraphQuickItem::deselect_last_press() {
  external();
  get_current_interaction()->deselect(_last_node_shape);
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

bool NodeGraphQuickItem::save() {
  external();
  if (!_file_model->save_graph()) {
    size_t num_nodes = get_root_group()->get_num_nodes();
    QString msg = "Unable to save graph.\n";
    msg += "With the Lite license, only graphs with a maximum of " + QString::number(FileModel::kMaxSaveableNodes)+ " nodes can be saved.";
    msg += " There are currently " + QString::number(num_nodes) + " nodes in this graph. Some are invisible.";
    emit set_error_message(msg);
    emit show_error_page();
    return false;
  }
  return true;
}

bool NodeGraphQuickItem::destroy_graph() {
  external();
  if (!_file_model->destroy_graph()) {
    QString msg = "Unable to remove the current graph.\n";
    msg += "This is the last remaining graph and the app requires at least one graph to operate.";
    emit set_error_message(msg);
    emit show_error_page();
    return false;
  }
  return true;
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
  destroy_selection();
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
  if (!_selection->paste(group)) {
    QString msg = "Unable to paste here as the surrounding group context "
        "does not match the group context from which the nodes were copied.";
    emit set_error_message(msg);
    emit show_error_page();
    return;
  }

  // debugging
  {
    const std::unordered_set<Entity*>& pasted = group->get_last_pasted();
    for (Entity* e : pasted) {
      Dep<NodeShape> ns = get_dep<NodeShape>(e);
      if (ns) {
        std::cerr << "ns name: " << ns->get_name() << "\n";
        ns->clean_state(); // we need them clean as we'll be using their bounds.
      }
    }
  }

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
  const std::unordered_set<Entity*> pasted = group->get_last_pasted();
  group->clear_last_pasted();

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

  // We don't allow exploding macros.
  const DepUSet<NodeShape>& selected = _selection->get_selected();
  if (selected.empty()) {
    return;
  }
  Entity *e = (*selected.begin())->our_entity();
  explode_group(e);
}

void NodeGraphQuickItem::explode_group(Entity* e) {
  if (!e->has_group_related_did()) {
    QString msg = "Only group nodes can be exploded.";
    emit set_error_message(msg);
    emit show_error_page();
    return;
  }

  EntityDID did = e->get_did();
  if ((did == EntityDID::kBrowserGroupNodeEntity) ||
      (did == EntityDID::kMQTTGroupNodeEntity)) {
    QString msg = "Special protocol groups for the browser or mqtt cannot be exploded.";
    emit set_error_message(msg);
    emit show_error_page();
    return;
  }

//  if ((e->get_did() == EntityDID::kAppMacroNodeEntity) ||
//      (e->get_did() == EntityDID::kUserMacroNodeEntity)) {
//    QString msg = "App and user macros cannot be exploded.";
//    emit set_error_message(msg);
//    emit show_error_page();
//    return;
//  }

  // Otherwise explode the selected.
  get_current_interaction()->explode(e);
  update();
}

//// Lock Graph.
//void NodeGraphQuickItem::lock_links(bool locked) {
//  external();
//  _link_locked = locked;
//}
//
//bool NodeGraphQuickItem::links_are_locked() const {
//  external();
//  return _link_locked;
//}

void NodeGraphQuickItem::view_node_poke() {
  _last_node_shape = _selection->get_view_node();
  if (!_last_node_shape) {
    emit view_node_outputs(QString("no last viewed"), QJsonObject());
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_node_shape->our_entity());
  QString node_path = compute->get_path_as_string().c_str();
  emit view_node_outputs(node_path, compute->get_outputs());
}

void NodeGraphQuickItem::edit_node_poke() {
  _last_node_shape = _selection->get_edit_node();
  if (!_last_node_shape) {
    emit edit_node_inputs(QString("no last edited"), QJsonObject(), QJsonObject(), QJsonObject());
    return;
  }
  Dep<Compute> compute = get_dep<Compute>(_last_node_shape->our_entity());
  QString node_path = compute->get_path_as_string().c_str();
  emit edit_node_inputs(node_path, compute->get_editable_inputs(), compute->get_hints(), compute->get_input_exposure());
}

bool NodeGraphQuickItem::can_rename_node() {
  if (!_last_node_shape) {
    return false;
  }
  // Nodes in certain groups cannot be renamed.
  if (_last_node_shape->our_entity()->is_topologically_fixed()) {
    return false;
  }
  return true;
}

void NodeGraphQuickItem::rename_node(const QString& next_name) {
  if (!_last_node_shape) {
    return;
  }
  // Nodes in certain groups cannot be renamed.
  if (_last_node_shape->our_entity()->is_topologically_fixed()) {
    QString msg = "This node's name cannot be changed. The surrounding group looks for it under this name.";
    emit set_error_message(msg);
    emit show_error_page();
    return;
  }

  _last_node_shape->our_entity()->rename(next_name.toStdString());
  _last_node_shape->dirty_state();
  update();
}

}

