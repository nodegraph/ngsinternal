#include "bits.h"
#include <base/memoryallocator/taggednew.h>
#include <cstring>

namespace ngs {

Bits::Bits()
    : num_bytes(0),
      raw(NULL) {
  // Basically this is the cleared state.
}

Bits::Bits(size_t num_bytes_param)
    : num_bytes(0),
      raw(NULL) {
  resize(num_bytes_param);
}

Bits::Bits(size_t num_bytes_param, const char* bits_param)
    : num_bytes(0),
      raw(NULL) {
  resize(num_bytes_param);
  std::memcpy(raw, bits_param, num_bytes);
}

Bits::Bits(const Bits& right)
    : num_bytes(0),
      raw(NULL) {
  resize(right.num_bytes);
  std::memcpy(raw, right.raw, num_bytes);
}

Bits::~Bits() {
  clear();
}

void Bits::clear() {
  delete_array_ff(raw)
  ;
  num_bytes = 0;
}

void Bits::resize(size_t num_bytes_param) {
  clear();
  num_bytes = num_bytes_param;
  raw = new_ff char[num_bytes];
}

void Bits::resize(const std::string& data) {
  clear();
  num_bytes = data.size();
  raw = new_ff char[num_bytes];
  std::memcpy(raw, data.c_str(), num_bytes);
}

// Assignment operator.
Bits& Bits::operator=(const Bits& right) {
  if (*this == right) {
    return *this;
  }

  resize(right.num_bytes);
  std::memcpy(raw, right.raw, num_bytes);
  return *this;
}

// Equality operator.
bool Bits::operator==(const Bits& right) const {
  if (num_bytes != right.num_bytes) {
    return false;
  }

  if (std::memcmp(raw, right.raw, num_bytes) != 0) {
    return false;
  }
  return true;
}

// Inequality operator.
bool Bits::operator!=(const Bits& right) const {
  return !(*this == right);
}

}
