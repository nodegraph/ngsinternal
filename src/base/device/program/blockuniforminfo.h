#pragma once
#include <base/device/device_export.h>
#include <base/device/program/uniforminfo.h>

namespace ngs {

class UniformInfo;

class DEVICE_EXPORT BlockUniformInfo : public UniformInfo {
 public:
  BlockUniformInfo();
  virtual ~BlockUniformInfo();

  // Custom methods.
  void set_block_index(GLint block_index);
  GLint get_block_index() const;

  void set_offset_bytes(GLint offset);
  GLint get_offset_bytes() const;

 private:
  // A block uniform is uniform that lives in a block.
  // It has a block index which identifies that block that it lives in.
  // It also a offset that describes its exact position within the block.

  GLint _block_index;  // -1 means it should actually be a default uniform type.
  GLint _offset_bytes;

};

}
