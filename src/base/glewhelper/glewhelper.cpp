#if (ARCH != ARCH_ANDROID) && (GLES_USE_ANGLE != 1)

#include <base/memoryallocator/taggednew.h>
#include <base/device/deviceheadersgl.h>
//#include <base/device/devicedebug.h>


#include <thread>
#include <cassert>
#include <iostream>


#if (ARCH == ARCH_WINDOWS) 
__declspec(thread) GLEWContext* g_thread_local_glew_context = NULL;
#elif (ARCH == ARCH_LINUX) || (ARCH == ARCH_ANDROID)
thread_local GLEWContext* g_thread_local_glew_context
#else
  // No glew on winrt.
#endif

#if (ARCH != ARCH_WINRT)

// Multi-threaded glew requires a function named gleGetContext which returns
// a glew context object that is associated with the calling thread.
GLEWContext* glewGetContext() {
  return g_thread_local_glew_context;
}

// Initialize glew.
// This should only be called once per thread.
void start_glew() {
  std::cerr << "starting to initialize glew\n";

  // Make sure this hasn't been called before on this thread.
  if (glewGetContext()!=NULL) {
    assert(false);
  }

  // Create the glew context.
  g_thread_local_glew_context = new_ff GLEWContext();
  //std::cerr << "creating thread local glew context: " << g_thread_local_glew_context << "\n";

  // Now initialize glew for this thread.
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cerr << "Error initializing glew: " << glewGetErrorString(err) << "\n";
    exit(1);
  }

  std::cerr << "done initializing glew\n";
}

void finish_glew() {
  std::cerr << "finishing glew\n";
  //std::cerr << "deleting thread local glew context: " << g_thread_local_glew_context << "\n";
  delete_ff(g_thread_local_glew_context);
}
#endif


#endif
