#pragma once
#include <base/device/device_export.h>


/*
 --- Targets ---

 Occlusion Queries.
 GL_SAMPLES_PASSED or GL_ANY_SAMPLES_PASSED

 Primitive Queries.
 GL_PRIMITIVES_GENERATED or GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN

 Timer Queries.
 TIME_ELAPSED or TIMESTAMP

 */

namespace ngs {

#if GLES_MAJOR_VERSION >= 3

class DEVICE_EXPORT AsyncQuery {
 public:
  AsyncQuery(GLenum target);
  virtual ~AsyncQuery();

  virtual void bind();
  virtual void unbind();

  // Methods that should be called when bound.
  virtual GLuint get_current_bound_name();  // Gets current name query bound to _target.

  // Methods that can be called without being bound.
  virtual GLuint get_name() const;

#if GLES_MAJOR_VERSION>=100
  virtual GLint get_num_bits();
#endif
  // Results;
  bool resultIsReady();
  GLint get_result();

  // Helper methods.
  static void disable_color_and_depth_writes();
  static void enable_color_and_depth_writes();

 private:
  void create_name();
  void remove_name();

  GLuint _name;
  GLenum _target;
};

#endif

}
