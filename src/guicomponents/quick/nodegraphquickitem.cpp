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
#include <guicomponents/quick/eventtoinfo.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphquickitemglobals.h>
#include <guicomponents/quick/texturedisplaynode.h>
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

const int NodeGraphQuickItem::kMinimumIdleLength = 1000; // 1 second in milli seconds.

class CleanUpGL : public QRunnable
{
public:
  void run () {
    std::cerr << "CleanUpGL is running !!!!!!!!!!!!!!!!!!!!\n";
    g_app_root->uninitialize_gl();
  }
};

NodeGraphQuickItem::NodeGraphQuickItem(Entity* parent)
    : QQuickItem(NULL),
      Component(parent, kIID(), kDID()),
      _fbo_worker(this),
      _selection(this),
      _canvas(this),
      _factory(this),
      _file_model(this),
      _last_pressed_node(this),
      _link_locked(false) {

  get_dep_loader()->register_fixed_dep(_fbo_worker, "");
  get_dep_loader()->register_fixed_dep(_selection, "");
  get_dep_loader()->register_fixed_dep(_canvas, "");
  get_dep_loader()->register_fixed_dep(_factory, "");
  get_dep_loader()->register_fixed_dep(_file_model, "");

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

  initialize_fixed_deps();
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

void NodeGraphQuickItem::update_state() {
  lock_links(_file_model->get_work_setting(FileModel::kLockLinksRole).toBool());
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

    g_app_root->initialize_deps();
    g_app_root->update_deps_and_hierarchy();
    g_app_root->initialize_gl();

    // Set the initial size.
    get_current_interaction()->resize_gl(_device_pixel_ratio*width(), _device_pixel_ratio*height());
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
  clean();

  std::cerr << "received double click event\n";
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  update();
}

void NodeGraphQuickItem::mouseMoveEvent(QMouseEvent * event) {
  clean();

  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::hoverMoveEvent(QHoverEvent * event) {
  clean();

  MouseInfo info = get_hover_info(event);
  get_current_interaction()->moved(info);
  update();
}

void NodeGraphQuickItem::mousePressEvent(QMouseEvent * event) {
  clean();

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
  clean();

  // Stop the long press timer if
  if (_long_press_timer.isActive()) {
    _long_press_timer.stop();
  }

  // Deal with the release event.
  MouseInfo info = get_mouse_info(event, _device_pixel_ratio);
  get_current_interaction()->released(info);

  // Save the node graph changes.
  save();
  update();
}

void NodeGraphQuickItem::wheelEvent(QWheelEvent *event) {
  clean();

  WheelInfo info = get_wheel_info(event);
  get_current_interaction()->wheel_rolled(info);
  update();
}

// Key overrides.
void NodeGraphQuickItem::keyPressEvent(QKeyEvent * event) {
  clean();

  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphQuickItem::keyReleaseEvent(QKeyEvent * event) {
  clean();

  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphQuickItem::touchEvent(QTouchEvent * event) {
  clean();

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

void NodeGraphQuickItem::popup_context_menu() {
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
      size_t did = e->get_did();
      // Send out context menu request signals.
      if (did == kGroupNodeEntity) {
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
  glm::vec2 d = get_current_interaction()->get_drag_delta();

  // If we haven't moven't that much from the mouse press
  // point during the timer's count down, then we have a
  // real long press action by the user.
  if (glm::length(d)<kLongPressDistanceThreshold) {
    popup_context_menu();
  }
}

const Dep<GroupInteraction>& NodeGraphQuickItem::get_current_interaction() const {
  return _canvas->get_current_interaction();
}

void NodeGraphQuickItem::toggle_selection_under_long_press() {
  if (!_last_pressed_node) {
    return;
  }
  get_current_interaction()->toggle_selection_under_press(_last_pressed_node);
  update();
}

QString NodeGraphQuickItem::get_ngs_version() const {
  return NGS_VERSION;
}

size_t NodeGraphQuickItem::get_num_nodes() const {
  // 3 is for the group node entities namespace folders (inputs, outputs, links).
  return get_current_interaction()->our_entity()->get_children().size() -3;
}

void NodeGraphQuickItem::finish_creating_node(Entity* e, bool centered) {
  e->create_internals();
  e->initialize_deps();
  Dep<NodeShape> cs = get_dep<NodeShape>(e);
  if (centered) {
    get_current_interaction()->centralize(cs);
  } else {
    cs->set_pos(_last_press.object_space_pos.xy());
  }
  // The parenting group node needs to update its inputs and outputs.
  get_app_root()->update_deps_and_hierarchy();
  _selection->clear_selection();
  _selection->select(cs);
  // Save the node graph changes.
  save();
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

void NodeGraphQuickItem::create_open_browser_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "open browser", kOpenBrowserNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_close_browser_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "close browser", kCloseBrowserNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_create_set_from_values_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "create set from values", kCreateSetFromValuesNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::create_create_set_from_type_node(bool centered) {
  Entity* e = _entity_instancer->instance(get_current_interaction()->our_entity(), "create set from type", kCreateSetFromTypeNodeEntity);
  finish_creating_node(e, centered);
}

void NodeGraphQuickItem::view_node() {
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }

  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    qDebug() << "performing compute! \n";
    compute->propagate_cleanliness();

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
    emit view_node_outputs(compute->our_entity()->get_name().c_str(), compute->get_results());

    // Update our node graph selection object which also tracks and edit and view nodes.
    get_current_interaction()->view(_last_pressed_node);
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::edit_node() {
  // Return if don't have a last pressed shape.
  if (!_last_pressed_node) {
    return;
  }

  Dep<Compute> compute = get_dep<Compute>(_last_pressed_node->our_entity());
  if(compute) {
    qDebug() << "performing compute! \n";
    compute->propagate_cleanliness();

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
    emit edit_node_params(compute->our_entity()->get_name().c_str(), test);

    // Update our node graph selection object which also tracks and edit and view nodes.
    get_current_interaction()->edit(_last_pressed_node);
    update();
  }else {
    qDebug() << "Error: could not find compute to perform. \n";
  }
}

void NodeGraphQuickItem::destroy_selection() {
  _selection->destroy_selection();
  get_app_root()->update_deps_and_hierarchy();
  // Save the node graph changes.
  save();
  update();
}

void NodeGraphQuickItem::dive() {
  // When switching groups we clear the selection.
  // We don't allow inter-group selections.
  _canvas->dive(_last_pressed_node->our_entity());
  _selection->clear_selection();
  frame_all();
  update();
}

void NodeGraphQuickItem::surface() {
  // When switching groups we clear the selection.
  // We don't allow inter-group selections.
  _canvas->surface();
  _selection->clear_selection();
  update();
}

void NodeGraphQuickItem::surface_to_root() {
  _canvas->surface_to_root();
  update();
}

void NodeGraphQuickItem::select_last_press() {
  get_current_interaction()->select(_last_pressed_node);
  update();
}

void NodeGraphQuickItem::deselect_last_press() {
  get_current_interaction()->deselect(_last_pressed_node);
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
  _canvas->clean();
  get_current_interaction()->frame_all();
  update();
}

void NodeGraphQuickItem::frame_selected() {
  _canvas->clean();
  get_current_interaction()->frame_selected(_selection->get_selected());
  update();
}

void NodeGraphQuickItem::save() {
  _file_model->save_graph();
}

void NodeGraphQuickItem::load() {
  _file_model->load_graph();
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

  // Save the node graph changes.
  save();
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
  DepUSet<NodeShape> node_shapes;
  for (Entity* e : pasted) {
    Dep<NodeShape> ns = get_dep<NodeShape>(e);
    if (ns) {
      node_shapes.insert(ns);
      ns->clean(); // we need them clean as we'll be using their bounds.
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
  // Save the node graph changes.
  save();
  update();
}

void NodeGraphQuickItem::collapse_to_group() {
  get_current_interaction()->collapse_selected();
  // Save the node graph changes.
  save();
  update();
}

void NodeGraphQuickItem::explode_group() {
  get_current_interaction()->explode_selected();
  // Save the node graph changes.
  save();
  update();
}

// Lock Graph.
void NodeGraphQuickItem::lock_links(bool locked) {
  _link_locked = locked;
}

bool NodeGraphQuickItem::links_are_locked() {
  return _link_locked;
}


}

