#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <string>
#include <vector>

#if ARCH == ARCH_ANDROID
#define DISABLE_GPU_MACRO
#endif

#ifdef DISABLE_GPU_MACRO
#define gpu(device_command) \
    device_command;
#else
// The gpu macro is enabled for desktop platforms.
// If this is used outside a valid opengl context, it will loop infinitely.
#define gpu(device_command) \
    ngs::Device::warn_on_error();\
    device_command;\
    ngs::Device::die_on_error()
#endif


namespace ngs {


class DEVICE_EXPORT CaptureDeviceState {
 public:
  CaptureDeviceState();
  ~CaptureDeviceState();
 private:
  std::vector<GLint> _textures;

  GLint _program_name;
  GLint _active_texture;
  GLint _bound_array_buffer_name;
  GLint _bound_element_array_buffer_name;
  GLint _bound_fbo_name;
  GLint _bound_rbo_name;
};

class DEVICE_EXPORT Device {
 public:
  // Info.
  static std::string get_version();

  // Glew.
  static void initialize_glew();

//  // Some basic functionallity for python.
//  static float read_pixel(int screen_gl_x, int screen_gl_y);
//  static void set_viewport(float x, float y, float width, float height);

  // Texture units.
  static int get_num_texture_units();
  static void set_active_texture_unit(unsigned int texture_unit_number);
  static void disable_texture_targets(unsigned int texture_unit_number);
  static void disable_all_texture_targets();
  static void enable_texture_target(unsigned int texture_unit_number, GLenum target);

  static void capture_bound_textures(std::vector<GLint>& names);
  static void restore_bound_textures(const std::vector<GLint>& names);
  // Attachments.
#if GLES_MAJOR_VERSION >= 3
  int get_max_color_attachments();
#endif

  // Anisotropic filtering.
#if GLES_MAJOR_VERSION >= 100
  static float get_max_texture_anisotropy();
#endif

  // Error checking.
  static void die_on_error();
  static bool warn_on_error();

  // Memory check.
//  static GLint get_total_memory_kb();
//  static GLint get_available_memory_kb();

 private:

  static void clear_errors();
  static void check_error();
  static bool has_error();

  static GLenum get_last_error_code();
  static GLenum _last_error_code;

};

}

