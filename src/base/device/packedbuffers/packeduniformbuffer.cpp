#include <base/device/packedbuffers/packeduniformbuffer.h>
#include <base/device/unpackedbuffers/uniformbuffer.h>
#include <base/device/program/program.h>
#include <base/memoryallocator/taggednew.h>

#include <cstring>
#include <iostream>
#include <cassert>

namespace ngs {
#if GLES_MAJOR_VERSION >= 3
PackedUniformBuffer::PackedUniformBuffer(size_t byte_size){
  _raw.resize(byte_size);
}

PackedUniformBuffer::~PackedUniformBuffer() {
}

void PackedUniformBuffer::resize(size_t num_bytes) {
  _raw.resize(num_bytes);
}

size_t PackedUniformBuffer::get_byte_size() const {
  return _raw.size();
}

const char* PackedUniformBuffer::get_raw() const {
  return &_raw[0];
}

//bool PackedUniforms::operator==(const PackedUniforms& right) const {
//  if (_raw.size() != right._raw.size()) {
//    return false;
//  }
//
//  // Check the data.
//  for (int i = 0; i < get_size_bytes(); i++) {
//    if (_raw[i] != right._raw[i]) {
//      return false;
//    }
//  }
//
//  // Check the uniform types.
//  if (_types.size() != right._types.size()) {
//    return false;
//  }
//  for (size_t i = 0; i < _types.size(); i++) {
//    if (!(*_types[i] == (*right._types[i]))) {
//      return false;
//    }
//  }
//
//  // Check the names.
//  if (_names != right._names) {
//    return false;
//  }
//
//  return true;
//}

void PackedUniformBuffer::dirty_uniform_buffers(size_t min, size_t max) {
  for (UniformBuffer* buffer: _uniform_buffers) {
    buffer->augment_dirty_range(min,max);
  }
}


bool PackedUniformBuffer::set_uniform(const BlockUniformInfo* info, const void* value) {

  // Determine the range to write to.
  int num_bytes = info->get_total_size_bytes();
  size_t begin = info->get_offset_bytes();
  size_t end = begin + num_bytes;

  // Augment the dirty range.
  dirty_uniform_buffers(begin, end);

  // Copy the data.
  std::cerr << "setting packed uniform: " << info->get_name() << ": bytes: " << num_bytes << "\n";
  std::memcpy(&_raw[begin], value, num_bytes);
  return true;
}

bool PackedUniformBuffer::set_part_of_uniform(const BlockUniformInfo* info, size_t offset_from_uniform_start, const char* raw, size_t num_raw_bytes) {

  // Determine the range to write to.
  size_t begin = info->get_offset_bytes() + offset_from_uniform_start;
  size_t end = begin + num_raw_bytes;

  // Augment the dirty range.
  dirty_uniform_buffers(begin, end);

  std::cerr << "setting part of uniform: " << info->get_name() << ": bytes: " << num_raw_bytes << "\n";
  std::memcpy(&_raw[begin], raw, num_raw_bytes);
  return true;
}

bool PackedUniformBuffer::get_uniform(const BlockUniformInfo* info, void* value) {
  char* src = &_raw[0] + info->get_offset_bytes();
  int num_byutes = info->get_total_size_bytes();
  std::memcpy(value, src, num_byutes);
  return true;
}

bool PackedUniformBuffer::get_part_of_uniform(const BlockUniformInfo* info, size_t offset_from_uniform_start, char* raw, size_t num_raw_bytes) {
  char* src = &_raw[0] + info->get_offset_bytes();
  src+=offset_from_uniform_start;
  std::memcpy(raw, src, num_raw_bytes);
  return true;
}

bool PackedUniformBuffer::associate_uniform_buffers(UniformBuffer* buffer) {
  if (_uniform_buffers.count(buffer)) {
    return false;
  }
  _uniform_buffers.insert(buffer);
  buffer->augment_dirty_range(0,_raw.size());
  return true;
}

#endif
}

