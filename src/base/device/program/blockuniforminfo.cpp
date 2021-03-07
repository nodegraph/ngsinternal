#include <base/device/program/blockuniforminfo.h>
#include <base/device/program/uniforminfo.h>

namespace ngs {

BlockUniformInfo::BlockUniformInfo()
    : UniformInfo(),
      _block_index(0),
      _offset_bytes(0) {
}

BlockUniformInfo::~BlockUniformInfo() {
}

void BlockUniformInfo::set_block_index(GLint block_index) {
  _block_index = block_index;
}

GLint BlockUniformInfo::get_block_index() const {
  return _block_index;
}

void BlockUniformInfo::set_offset_bytes(GLint offset) {
  _offset_bytes = offset;
}

GLint BlockUniformInfo::get_offset_bytes() const {
  return _offset_bytes;
}

}
