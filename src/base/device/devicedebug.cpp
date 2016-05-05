#include <base/device/devicedebug.h>
#include <base/glewhelper/glewhelper.h>
#include <base/device/deviceheadersgl.h>

#if (ARCH == ARCH_WINDOWS) && (ARCH == ARCH_LINUX)
  #include <GL/glu.h> // desktop glu
#endif

#include <boost/lexical_cast.hpp>

#include <cassert>
#include <iostream>
#include <sstream>

//#include <QtCore/QDebug>

#define CAPTURE_ALL

namespace ngs {



CaptureDeviceState::CaptureDeviceState():
  _program_name(0) {
#ifdef CAPTURE_ALL
  glGetIntegerv(GL_ACTIVE_TEXTURE, &_active_texture);

  for (int i = 0; i < Device::get_num_texture_units(); i++) {
    Device::set_active_texture_unit(i);
    // Get the previous bound texture name so that we can restore it.
    GLint name;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &name);
    _textures.push_back(name);
  }
  // restore the active texture
  glActiveTexture(_active_texture);

  glGetIntegerv(GL_CURRENT_PROGRAM, &_program_name);

  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &_bound_array_buffer_name);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &_bound_element_array_buffer_name);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_bound_fbo_name);
  glGetIntegerv(GL_RENDERBUFFER_BINDING, &_bound_rbo_name);
#endif
}

CaptureDeviceState::~CaptureDeviceState() {
#ifdef CAPTURE_ALL
  for (int i = 0; i < Device::get_num_texture_units(); i++) {
    Device::set_active_texture_unit(i);
    // Finally restore the previous binding.
    if (i) {
      glBindTexture(GL_TEXTURE_2D, _textures[i]);
    }
  }

  glUseProgram(_program_name);
  glActiveTexture(_active_texture);
  glBindBuffer(GL_ARRAY_BUFFER, _bound_array_buffer_name);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bound_element_array_buffer_name);
  glBindFramebuffer(GL_FRAMEBUFFER, _bound_fbo_name);
  glBindRenderbuffer(GL_RENDERBUFFER, _bound_rbo_name);
#endif

  // Qt seems to keep all the vertex divisors at zero.
  // And we use up to 8 in gles shader programs right now.
  // We really should query the divisor values to be complete,
  // but for speed reasons we just set all of them to zero.
  for (GLuint i = 0; i < 8; ++i) {
    #if (GLES_MAJOR_VERSION == 2)
      #if GLES_USE_ANGLE == 1
          gpu(glVertexAttribDivisorANGLE(i, 0));
      #else
          gpu(glVertexAttribDivisorEXT(i, 0));
      #endif
    #else
      #if ARCH == ARCH_MACOS
        gpu(glVertexAttribDivisorARB(i, 0));
      #else
        gpu(glVertexAttribDivisor(i, 0));
      #endif
    #endif
  }
}







// Static variables.
GLenum Device::_last_error_code = GL_NO_ERROR;

// Info.

std::string Device::get_version() {
  gpu(const char* version=reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  if (version) {
    return version;
  }
  return "";
}

// Glew.

void Device::initialize_glew() {
#if GLES_MAJOR_VERSION==0
  // Initialize glew.
  GLenum err=glewInit();
  if(err!=GLEW_OK)
  {
      // Problem: glewInit failed, something is seriously wrong.
      std::cerr<<glewGetErrorString(err)<<"\n";
      exit(1);
  }
#endif
}

//float Device::read_pixel(int screen_gl_x, int screen_gl_y) {
//  float screen_z=0;
//  gpu(glReadPixels(screen_gl_x, screen_gl_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screen_z));
//  return screen_z;
//}
//
//void Device::set_viewport(float x, float y, float width, float height) {
//  glViewport(x, y, width, height);
//}

// Texture units.

int Device::get_num_texture_units() {
  int num;
  //gpu(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&num));
  gpu(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&num));
  return num;
}

void Device::set_active_texture_unit(unsigned int texture_unit_number) {
  gpu(glActiveTexture(GL_TEXTURE0+texture_unit_number));

}

void Device::disable_texture_targets(unsigned int texture_unit_number) {
  set_active_texture_unit(texture_unit_number);

  // Disable all the targets we know about.

#if GLES_MAJOR_VERSION >= 1
  // Although GL_TEXTURE_2D and GL_TEXTURE_CUBE_MAP are valid on gles 2
  // they cannot be enabled or disabled.
#endif
#if GLES_MAJOR_VERSION >= 3
  // Although GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, and GL_TEXTURE_3D
  // are valid on gles 3, they cannot be enabled or disabled.
#endif
#if GLES_MAJOR_VERSION > 100
  gpu(glDisable(GL_TEXTURE_2D));
  gpu(glDisable(GL_TEXTURE_CUBE_MAP));
  gpu(glDisable(GL_TEXTURE_3D));

  gpu(glDisable(GL_TEXTURE_1D));
  gpu(glDisable(GL_TEXTURE_RECTANGLE));
#endif
}

void Device::disable_all_texture_targets() {
  for (int i = 0; i < get_num_texture_units(); i++) {
    disable_texture_targets(i);
  }
  set_active_texture_unit(0);
}

void Device::enable_texture_target(unsigned int texture_unit_number,
                                   GLenum target) {
  set_active_texture_unit(texture_unit_number);

  switch (target) {
#if GLES_MAJOR_VERSION >=1
  // In gles2 many glenable states are not present. Only the following can be enabled/disabled.
  //  GL_BLEND
  //  GL_CULL_FACE
  //  GL_DEPTH_TEST
  //  GL_DITHER
  //  GL_POLYGON_OFFSET_FILL
  //  GL_SAMPLE_ALPHA_TO_COVERAGE
  //  GL_SAMPLE_COVERAGE
  //  GL_SCISSOR_TEST
  //  GL_STENCIL_TEST
#endif
#if GLES_MAJOR_VERSION >= 3
  // GLES3 has GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_3D, but
  // they cannot be enabled or disabled.
#endif
#if GLES_MAJOR_VERSION > 100
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP:
	case GL_TEXTURE_3D:
	case GL_TEXTURE_1D:
	case GL_TEXTURE_RECTANGLE:
#endif
    {
      gpu(glEnable(target));
    }
      break;
    default: {
      // do nothing. for the newer targets like texture arrays, it is an error to enable them on texture units.
      // so only need to do glBindTexture(GL_TEXTURE_2D_ARRAY,name) on them.
    }
      break;
  }
}

void Device::capture_bound_textures(std::vector<GLint>& names) {
  for (int i = 0; i < get_num_texture_units(); i++) {
    set_active_texture_unit(i);
    // Get the previous bound texture name so that we can restore it.
    GLint prev_texture_name;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_texture_name);
    names.push_back(prev_texture_name);
    std::cerr << "capture texture at: " << i << " is: " << prev_texture_name << "\n";
  }
}

void Device::restore_bound_textures(const std::vector<GLint>& names) {
  for (int i = 0; i < get_num_texture_units(); i++) {
    set_active_texture_unit(i);
    // Finally restore the previous binding.
    if (i) {
      glBindTexture(GL_TEXTURE_2D, names[i]);
    }
  }
}


// Attachments.
#if GLES_MAJOR_VERSION >= 3
int Device::get_max_color_attachments() {
  GLint max;
  gpu(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max););
  return max;
}
#endif

// Filtering.
#if GLES_MAJOR_VERSION >= 100
float Device::get_max_texture_anisotropy() {
  float max;
  gpu(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&max));
  return max;
}
#endif

// Device Errors.

// Note if this is called outside a valid opengl context, it will loop infinitely.
void Device::clear_errors() {
  // Retrieve error codes from the device until there is no more. 
  GLenum error = glGetError();
  while (error != GL_NO_ERROR) {
    error = glGetError();
  }

  // Clears the last error state.
  _last_error_code = GL_NO_ERROR;
}

void Device::check_error() {
  // Retrieve error code from the device.
  GLenum error = glGetError();
  if (error == GL_NO_ERROR) {
    return;
  }

  // If there is an error, we record it.
  _last_error_code = error;
}

bool Device::has_error() {
  check_error();
  if (_last_error_code != GL_NO_ERROR) {
    return true;
  }
  return false;
}

GLenum Device::get_last_error_code() {
  return _last_error_code;
}

bool Device::warn_on_error() {
  if (has_error()) {
    std::cerr << "Warning: Pre-existing opengl error was detected: " << _last_error_code << "\n";
#ifdef ANDROID_ARCH
    __android_log_assert("gl warn", "NODEGRAPH", "Warning: Pre-existing opengl error was detected: %d", _last_error_code);
#endif
	assert(false);

    clear_errors();
    return true;
  }
  return false;
}

void Device::die_on_error() {
  if (has_error()) {
    std::cerr << "Error: Application has generated an opengl error: " << _last_error_code << "\n";
#ifdef ANDROID_ARCH
    __android_log_assert("gl error", "NODEGRAPH", "Error: Application has generated an opengl error: %d", _last_error_code);
#endif
    clear_errors();

    // Exit.
    assert(false);
  }
}

// These are not available on gles.
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

//GLint Device::get_total_memory_kb() {
//  GLint total_memory_kb = 0;
//  //gpu(glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,&total_memory_kb));
//  return total_memory_kb;
//}
//
//GLint Device::get_available_memory_kb() {
//  GLint available_memory_kb = 0;
//  //gpu(glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,&available_memory_kb));
//  return available_memory_kb;
//}

}
