#pragma once
#include <base/utils/utils_export.h>
#include <vector>
#include <stddef.h>

namespace ngs {

class UTILS_EXPORT BlockArray {
 public:
  BlockArray(size_t block_size_bytes);

  // Converters.
  long get_block_size_bytes(long num_blocks) const;

  // Block getters.
  unsigned char& operator[](const size_t index);

  // Block setters.
  void set_block(size_t dest_index, const unsigned char* src_data);
  void set_block(size_t dest_index, size_t src_index);

  // Sizing.
  size_t size() const;
  size_t get_size_bytes() const;
  void resize(size_t size);
  void erase(size_t start_index, size_t end_index); // note end_index is one past the last actual element to be deleted.

  // Raw.
  std::vector<unsigned char>& get_raw();

 private:
  const size_t _block_size_bytes;
  std::vector<unsigned char> _raw;
};

}
