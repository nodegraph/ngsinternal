#pragma once

#if (ARCH == ARCH_LINUX)
    #include <GL/glew.h>
    #include <GL/gl.h>
    #include <base/glewhelper/glewhelper.h>
#endif



#if (ARCH == ARCH_WINRT) || (ARCH == ARCH_WINDOWS)
#define GL_GLEXT_PROTOTYPES
  #if GLES_MAJOR_VERSION==2
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
  #elif GLES_MAJOR_VERSION==3
    #include <GLES3/gl3.h>
    //#include <GLES3/gl3ext.h>
  #elif GLES_MAJOR_VERSION>=100
    This configuration is invalid.
    assert(false);
  #endif
#undef GL_GLEXT_PROTOTYPES
#endif

#if ARCH == ARCH_ANDROID
  #if GLES_MAJOR_VERSION==2
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
  #elif GLES_MAJOR_VERSION==3
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>
  #elif GLES_MAJOR_VERSION>=100
    This is configuration is invalid.
    assert(false);
  #endif
#endif


