
#include <base/glewhelper/glewhelper.h>

#include "testshapes.h"
#include <base/memoryallocator/taggednew.h>


#include <iostream>
#include <fstream>

//#include <ppapi_simple/ps_main.h>
//#include <ppapi_simple/ps_interface.h>
//#include <ppapi/c/ppb_graphics_3d.h>
//#include <ppapi/c/ppb.h>
//
//#include "ppapi/cpp/graphics_3d.h"


void test() {
  using namespace ngs;
  bootstrap_memory_tracker();
  {
    // Initialize glew.
    start_glew();

    // Run our unit tests.
    TestShapes test;

    finish_glew();
  }
  shutdown_memory_tracker();
}

void NaClGLUnitTest() {
  using namespace ngs;
  bootstrap_memory_tracker();
  std::cerr << "starting the shapes test\n";
  // Run our unit tests.
  ngs::TestShapes test;
  std::cerr << "finished the shapes test\n";
  shutdown_memory_tracker();
}

#ifdef WIN32
int main( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd ) {
  using namespace ngs;
  WindowWIN *window = new_ff WindowWIN(hInstance);
  window->makeCurrent();

  test();

  // Cleanup.
  delete_ff(window);
  return 0;
}
#else
int main(int argc, char **argv) {
  using namespace ngs;
  WindowGLX *window = new_ff WindowGLX();
  window->makeCurrent();

  test();

  // Cleanup.
  delete_ff(window);
  return 0;
}
#endif




