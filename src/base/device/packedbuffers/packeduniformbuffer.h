#pragma once

#include <base/device/device_export.h>
#include <base/device/program/blockuniforminfo.h>
#include <base/device/program/uniforminfo.h>
#include <vector>
#include <set>

namespace ngs {

class Program;
class ChunkFormat;
class UniformBuffer;

// This class is used to hold uniforms on the cpu system side.
// This is eventually supposed to get uploaded to uniform buffer objects on the gpu side.
#if GLES_MAJOR_VERSION >= 3
class DEVICE_EXPORT PackedUniformBuffer {
 public:
  PackedUniformBuffer(size_t byte_size);
  virtual ~PackedUniformBuffer();

//  virtual bool operator==(const PackedUniforms& other) const;

  virtual void resize(size_t num_bytes);
  virtual size_t get_byte_size() const;
  virtual const char* get_raw() const;

  // Uniform Values.
  // These simply use memcpy to copy the value in or out.
  bool set_uniform(const BlockUniformInfo* info, const void* value);
  bool set_part_of_uniform(const BlockUniformInfo* info, size_t offset_from_uniform_start, const char* raw, size_t num_raw_bytes);
  bool get_uniform(const BlockUniformInfo* info, void* value);
  bool get_part_of_uniform(const BlockUniformInfo* info, size_t offset_from_uniform_start, char* raw, size_t num_raw_bytes);

  // Associated Uniform Buffers.
  bool associate_uniform_buffers(UniformBuffer* buffer);

 private:

  void dirty_uniform_buffers(size_t min, size_t max);

  // This is where all the uniforms for this uniform block live.
  // Note that there is one UniformBuffer for each uniform block.
  std::vector<char> _raw;

  // Our associated UniformBufferd on the gpu side.
  std::set<UniformBuffer*> _uniform_buffers;

};
#endif

}
