#pragma once
#include <base/utils/utils_export.h>
#include <string>

namespace ngs {

class UTILS_EXPORT Bits {
 public:
  Bits();
  Bits(size_t num_bytes_param);
  Bits(size_t num_bytes_param, const char* bits_param);
  Bits(const Bits& right);
  virtual ~Bits();
  void clear();
  void resize(size_t num_bytes_param);

  // This is mostly used for unit testing.
  void resize(const std::string& data);

  // Assignment operator.
  Bits& operator=(const Bits& right);
  // Equality operator.
  bool operator==(const Bits& right) const;
  // Inequality operator.
  bool operator!=(const Bits& right) const;

  char* raw;
  size_t num_bytes;
};

}
