#include <base/glewhelper/glewhelper.h>
#include <base/objectmodel/basefactory.h>
#include <gui/widget/nodegrapheditor.h>
#include <components/entities/guientities.h>
#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>
#include <components/interactions/graphbuilder.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QMainWindow>

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  using namespace ngs;

  // Startup the memory tracker.
  bootstrap_memory_tracker();

  {
    // Create our app.
    QApplication app(argc, argv);

#if (GLES_MAJOR_VERSION <= 3)
  // This sets up the app to use opengl es.
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::OpenGLES);
  QSurfaceFormat::setDefaultFormat(format);
#endif

    // Set our app style.
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Set up our palette.
    QPalette p;

    // Set our active colors.
    p.setColor(QPalette::Window, QColor(50,50,50)); // general background color of widgets.
    p.setColor(QPalette::WindowText, QColor(255,255,255)); // general foreground color of text.
    p.setColor(QPalette::Base, QColor(100,100,100)); // background color of text entry widgets.
    p.setColor(QPalette::AlternateBase, QColor(30,30,30)); // alternate background color in views with alternating row colors.
    p.setColor(QPalette::ToolTipBase, QColor(100,100,100)); // background color for QToolTip and QWhatsThis.
    p.setColor(QPalette::ToolTipText, QColor(255,255,255)); // foreground color for QToolTip and QWhatsThis.
    p.setColor(QPalette::Text, QColor(255,255,255)); // foreground color used with base. must provide good contrast with Window and Base. and is usually the same as WindowText.
    p.setColor(QPalette::Button, QColor(100,100,100)); // general background color for butttons.
    p.setColor(QPalette::ButtonText, QColor(255,255,255)); // foreground color used with Button color.
    p.setColor(QPalette::BrightText, Qt::red); // color used for text drawn where Text or WindowText would give poor contrast. such as pushed buttons.
    // We let the following get calculated by qt.
    // QPalette::Light
    // QPalette::Midlight
    // QPalette::Dark
    // QPalette::Mid
    // QPalette::Shadow
    p.setColor(QPalette::Highlight, QColor(204,204,0)); // color used to indicate selected item or current item.
    p.setColor(QPalette::HighlightedText, QColor(0,0,0)); // text color that contrasts with highlight.
    p.setColor(QPalette::Link, Qt::blue); // color for unvisited links.
    p.setColor(QPalette::LinkVisited, Qt::magenta); // color for visited links.

    // Set our disabled colors.
    p.setColor(QPalette::Disabled,QPalette::Window, QColor(25,25,25)); // general background color of widgets.
    p.setColor(QPalette::Disabled,QPalette::WindowText, QColor(127,127,127)); // general foreground color of text.
    p.setColor(QPalette::Disabled,QPalette::Base, QColor(50,50,50)); // background color of text entry widgets.
    p.setColor(QPalette::Disabled,QPalette::AlternateBase, QColor(15,15,15)); // alternate background color in views with alternating row colors.
    p.setColor(QPalette::Disabled,QPalette::ToolTipBase, QColor(50,50,50)); // background color for QToolTip and QWhatsThis.
    p.setColor(QPalette::Disabled,QPalette::ToolTipText, QColor(127,127,127)); // foreground color for QToolTip and QWhatsThis.
    p.setColor(QPalette::Disabled,QPalette::Text, QColor(127,127,127)); // foreground color used with base. must provide good contrast with Window and Base. and is usually the same as WindowText.
    p.setColor(QPalette::Disabled,QPalette::Button, QColor(50,50,50)); // general background color for butttons.
    p.setColor(QPalette::Disabled,QPalette::ButtonText, QColor(127,127,127)); // foreground color used with Button color.
    p.setColor(QPalette::Disabled,QPalette::BrightText, Qt::red); // color used for text drawn where Text or WindowText would give poor contrast. such as pushed buttons.
    // We let the following get calculated by qt.
    // QPalette::Light
    // QPalette::Midlight
    // QPalette::Dark
    // QPalette::Mid
    // QPalette::Shadow
    p.setColor(QPalette::Disabled,QPalette::Highlight, QColor(102,102,0)); // color used to indicate selected item or current item.
    p.setColor(QPalette::Disabled,QPalette::HighlightedText, QColor(0,0,0)); // text color that contrasts with highlight.
    p.setColor(QPalette::Disabled,QPalette::Link, Qt::blue); // color for unvisited links.
    p.setColor(QPalette::Disabled,QPalette::LinkVisited, Qt::magenta); // color for visited links.
    qApp->setPalette(p);

    // Create our window.
    QMainWindow* window = new_ff QMainWindow();

    // Build the app root.
    Entity* app_root = new_ff QtAppEntity(NULL, "app");
    app_root->create_internals();

    // Build the root group.
    Entity* root_group = new_ff GroupNodeEntity(app_root, "root");
    root_group->create_internals();

    // Build our first gl widget for sharing purposes.
    QGLWidget* shared_widget = new_ff QGLWidget(window);

    // Build our node graph editor.
    NodeGraphEditor* ng_editor = new_ff NodeGraphEditor(app_root, window, shared_widget);

    // Update the dependencies.
    app_root->initialize_deps();
    app_root->update_deps_and_hierarchy();

    // Build a test node graph.
    ng_editor->build_test_graph();


    // Set the node graph editor as the central widget.
    window->setCentralWidget(ng_editor);
    window->resize(950, 950);
    window->show();

    // Start the qt event loop.
    app.exec();

    // Now we destroy everything.
    // Make sure the node graph editor's gl context is current during destruction.
    ng_editor->makeCurrent();
    app_root->uninitialize_gl();
    delete_ff(app_root);
    delete_ff(window);

    // Finish using glew. The NodeGraphEditor starts using glew in intializeGL(), not intialize_gl().
#if !((ARCH == ARCH_ANDROID) || (GLES_USE_ANGLE == 1) || (ARCH == ARCH_MACOS))
    finish_glew();
#endif
  }

  // Cleanup.
  shutdown_memory_tracker();
  return 0;
}
