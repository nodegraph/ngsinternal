#include <base/device/device_export.h>
#include <cstddef>

// Copy of typedefs from glew.h.
// This is to avoid including the entire glew.h header file in our class definitions,
// as this causes conflicts with qt's opengl classes.

typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

typedef char GLchar;

#define GL_STREAM_DRAW 0x88E0

#if GLES_MAJOR_VERSION >= 3

//#if !defined(_W64)
//#  if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && defined(_MSC_VER) && _MSC_VER >= 1300
//#    define _W64 __w64
//#  else
//#    define _W64
//#  endif
//#endif
//
//#if !defined(_PTRDIFF_T_DEFINED) && !defined(_PTRDIFF_T_)
//  #ifdef _WIN64
//    typedef __int64 ptrdiff_t;
//  #else
//    typedef _W64 int ptrdiff_t;
//  #endif
//  #define _PTRDIFF_T_DEFINED
//  #define _PTRDIFF_T_
//#endif
//
//#if (ARCH == ARCH_ANDROID)
//  typedef signed long int GLsizeiptr;
//  typedef signed long int GLintptr;
//#elif (ARCH == ARCH_MACOS)
//  typedef intptr_t GLsizeiptr;
//  typedef intptr_t GLintptr;
//#else
//  typedef ptrdiff_t GLsizeiptr;
//  typedef ptrdiff_t GLintptr;
//#endif

#if (ARCH == ARCH_ANDROID)
  typedef signed long int GLsizeiptr;
  typedef signed long int GLintptr;
#else
  typedef ptrdiff_t GLsizeiptr;
  typedef ptrdiff_t GLintptr;
#endif

#endif

DEVICE_EXPORT extern const unsigned int PREDEFINED_GL_TRIANGLES;
DEVICE_EXPORT extern const unsigned int PREDEFINED_GL_UNSIGNED_INT;
DEVICE_EXPORT extern const unsigned int PREDEFINED_GL_STREAM_DRAW;


