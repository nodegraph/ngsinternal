#include <iostream>

#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>
#include <base/glewhelper/glewhelper.h>
#include <gui/qt/testwindow.h>

#include <QtWidgets/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QtDebug>
#include <QtWidgets/QMainWindow>

#include <iostream>

#include "testmemory.h"
#include "testtexture.h"
#include "testtexture2.h"
#include "testrenderoutputs.h"
#include "testmultisampling.h"
#include "testasyncquery.h"
#include "testgeometryshader.h"

using namespace ngs;

void test_texture_gles2(){
  TestTexture test_texture8(UCharElement, false);
  TestTexture _test_pipeline8(UCharElement, true);
}

void test_texture_gles3_plus() {
  // Test pipeline with unnormalized texture values.
  // Note float,int, and unsigned int textures are always accessed unnormalized.
  TestTexture test_texture1(FloatElement, false);
  TestTexture test_texture2(HalfElement, false);
  TestTexture test_texture3(IntElement, false);
  TestTexture test_texture4(UIntElement, false);
  TestTexture test_texture5(ShortElement, false);
  TestTexture test_texture6(UShortElement, false);
  TestTexture test_texture7(CharElement, false);
  TestTexture test_texture8(UCharElement, false);

  // Test pipeline with normalized texture values.
  // Note float,int, and unsigned int textures are always accessed unnormalized.
  TestTexture _test_texture1(FloatElement, true);
  TestTexture _test_pipeline2(HalfElement, true);
  TestTexture _test_texture3(IntElement, true);
  TestTexture _test_texture4(UIntElement, true);
  //TestTexture _test_texture5(ShortElement,true); // FBOs are not required to support snorm textures.
  //TestTexture _test_texture6(UShortElement, true);
  //TestTexture _test_texture7(CharElement,true); // FBOs are not required to support snorm textures.
  //TestTexture _test_pipeline8(UCharElement, true);
}

void test_texture2_gles2() {
  TestTexture2 test_texture8(UCharElement, false);
  TestTexture2 _est_pipeline8(UCharElement, true);
}

void test_texture2_gles3_plus() {
  // Test pipeline with unnormalized texture values.
  // Note float,int, and unsigned int textures are always accessed unnormalized.
  TestTexture2 test_texture1(FloatElement, false);
  TestTexture2 test_texture2(HalfElement, false);
  TestTexture2 test_texture3(IntElement, false);
  TestTexture2 test_texture4(UIntElement, false);
  TestTexture2 test_texture5(ShortElement, false);
  TestTexture2 test_texture6(UShortElement, false);
  TestTexture2 test_texture7(CharElement, false);
  TestTexture2 test_texture8(UCharElement, false);

  // Test pipeline with normalized texture values.
  // Note float,int, and unsigned int textures are always accessed unnormalized.
  TestTexture2 _test_texture1(FloatElement, true);
  TestTexture2 _test_texture2(HalfElement, true);
  TestTexture2 _test_texture3(IntElement, true);
  TestTexture2 _test_texture4(UIntElement, true);
  //TestTexture<short> _test_texture5(true); // FBOs are not required to support snorm textures.
  //TestTexture2 _test_texture6(UShortElement, true);
  //TestTexture<char> _test_texture7(true); // FBOs are not required to support snorm textures.
  //TestTexture2 _est_pipeline8(UCharElement, true);
}



#if GLES_MAJOR_VERSION >= 3
void test_render_outputs_gles3_plus() {
  // Test pipeline with unnormalized texture values.
  // Note float,int, and unsigned int textures are always accessed unnormalized.
  TestRenderOutputs test_pipeline1(FloatElement, false);
  TestRenderOutputs test_pipeline2(HalfElement, false);
  TestRenderOutputs test_pipeline3(IntElement, false);
  TestRenderOutputs test_pipeline4(UIntElement, false);
  TestRenderOutputs test_pipeline5(ShortElement, false);
  TestRenderOutputs test_pipeline6(UShortElement, false);
  TestRenderOutputs test_pipeline7(CharElement, false);
  TestRenderOutputs test_pipeline8(UCharElement, false);

  // Test pipeline with normalized texture values.
  // Note float,int, and unsigned int textures are always accessed unnormalized.
  TestRenderOutputs _test_pipeline1(FloatElement, true);
  TestRenderOutputs _test_pipeline2(HalfElement, true);
  TestRenderOutputs _test_pipeline3(IntElement, true);
  TestRenderOutputs _test_pipeline4(UIntElement, true);
  //TestRenderOutputs<short> _test_pipeline5(true); // FBOs are not required to support snorm textures.
  //TestRenderOutputs _test_pipeline6(UShortElement, true);
  //TestRenderOutputs<char> _test_pipeline7(true); // FBOs are not required to support snorm textures.
  //TestRenderOutputs _test_pipeline8(UCharElement, true);
}
#elif GLES_MAJOR_VERSION < 3
void test_render_outputs_gles2() {
//  TestRenderOutputs test_pipeline1(CharElement, false);
//  TestRenderOutputs test_pipeline2(CharElement, true);
}
#endif

// The opengl window and context should be current at this point.
void test() {
	// Bootstrap all the global objects.
  bootstrap_memory_tracker();
  {
    // Initialize glew.
#if !((ARCH == ARCH_ANDROID) || (GLES_USE_ANGLE == 1) || (ARCH == ARCH_MACOS))
    start_glew();
#endif

    // Test gpu memory.
    std::cerr << "testinging gpu memory ...\n";
    TestMemory test_memory;

#if GLES_MAJOR_VERSION >= 3
    std::cerr << "testing gpu textures ...\n";
    test_texture_gles3_plus();
    std::cerr << "testing gpu textures2 ...\n";
    test_texture2_gles3_plus();
    std::cerr << "testing gpu render outputs ...\n";
    test_render_outputs_gles3_plus();
    std::cerr << "testing gpu multi-sampling ...\n";
    TestMultiSampling test_multi_sampling;
    std::cerr << "testing gpu asynchronous queries ...\n";
    TestAsyncQuery test_async_query;
    std::cerr << "testing gpu geometry shaders ...\n";
    TestGeometryShader test_geometry_shader;
    std::cerr << "device testing has pass. 100%\n";
#elif GLES_MAJOR_VERSION < 3
    std::cerr << "testing gpu textures ...\n";
    test_texture_gles2();
    std::cerr << "testing gpu textures2 ...\n";
    test_texture2_gles2();
    std::cerr << "testing gpu render outputs ...\n";
    test_render_outputs_gles2();
#endif

  }

#if !((ARCH == ARCH_ANDROID) || (GLES_USE_ANGLE == 1) || (ARCH == ARCH_MACOS))
  // Finish using glew after all the gl related have been destroyed.
  finish_glew();
#endif

  // Cleanup all the global objects.
  shutdown_memory_tracker();
}

int main(int argc, char **argv) {

  QApplication app(argc, argv);

#if (ARCH == ARCH_MACOS)
  // On Mac OS setting the default surface format won't stick.
  // So you need to set the format in each gui widget which will use opengl.
#elif (GLES_MAJOR_VERSION <= 3)
  // This sets up the app to use opengl es.
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::OpenGLES);
  QSurfaceFormat::setDefaultFormat(format);
#endif


  qDebug() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();

  //int dummy;
  //std::cin >> dummy;

  QMainWindow* window = new_ff QMainWindow();
  TestWindow *test_window = new_ff TestWindow(window);
  test_window->test_callback = &test;

  window->setCentralWidget(test_window);
  window->resize(512, 512);
  window->show();

  app.exec();

  delete_ff(test_window);
  return 0;
}

