#include "blockarray.h"
#include <cstring>

namespace ngs {

BlockArray::BlockArray(size_t block_size_bytes):
  _block_size_bytes(block_size_bytes){
}

// Block info.
long BlockArray::get_block_size_bytes(long num_blocks) const {
  return num_blocks*_block_size_bytes;
}

// Block getters.
unsigned char& BlockArray::operator[](const size_t index) {
  return _raw[index*_block_size_bytes];
}

// Block setters.
void BlockArray::set_block(size_t dest_index, const unsigned char* src_data) {
  unsigned char* dest=&operator[](dest_index);
  std::memcpy(dest,src_data,_block_size_bytes);
}

void BlockArray::set_block(size_t dest_index, size_t src_index) {
  unsigned char* dest=&operator[](dest_index);
  unsigned char* src=&operator[](src_index);
  std::memcpy(dest,src,_block_size_bytes);
}

// Resize.
size_t BlockArray::size() const {
  return _raw.size()/_block_size_bytes;
}

size_t BlockArray::get_size_bytes() const {
  return _raw.size();
}

void BlockArray::resize(size_t size) {
  _raw.resize(get_block_size_bytes(size));
}

void BlockArray::erase(size_t start_index, size_t end_index) {
  std::vector<unsigned char>::iterator remove_start_vertex = _raw.begin() + get_block_size_bytes(start_index);
  std::vector<unsigned char>::iterator remove_end_vertex = _raw.begin() + get_block_size_bytes(end_index);
  _raw.erase(remove_start_vertex, remove_end_vertex);
}

std::vector<unsigned char>& BlockArray::get_raw() {
  return _raw;
}

}
