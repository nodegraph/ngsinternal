#pragma once
#include <base/device/device_export.h>
#include <base/device/unpackedbuffers/buffer.h>

namespace ngs {

class PackedUniformBuffer;

#if GLES_MAJOR_VERSION >= 3
class DEVICE_EXPORT UniformBuffer : public Buffer {
 public:
  UniformBuffer();
  virtual ~UniformBuffer();

  // Uniform Block Binding.
  virtual void bind_to_uniform_block_binding(GLuint uniform_block_binding_index, GLintptr offset, GLsizeiptr size);
  virtual void bind_to_uniform_block_binding( GLuint uniform_block_binding_index);

  // Loading.
  void load_all(PackedUniformBuffer* packed_uniforms);
  void load_changed(PackedUniformBuffer* packed_uniforms);

  // Dirtiness Tracking.
  bool is_dirty();
  size_t get_min_dirty_byte() {return _min_dirty_byte;}
  size_t get_max_dirty_byte() {return _max_dirty_byte;}
  void clean_dirty_range();
  void augment_dirty_range(size_t min, size_t max);


 private:
  // Keep track of which parts of this packed buffer is different
  // from its corresponding uniform buffer on the gpu.
  size_t _min_dirty_byte;
  size_t _max_dirty_byte;

};
#endif


}
