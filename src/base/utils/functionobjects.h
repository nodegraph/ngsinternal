#pragma once

#include <base/device/transforms/glmhelper.h>

namespace ngs {

// Pairs of ints.

struct IntPairHash {
  static const int max_second_value_range = 99999;
  size_t operator()(const std::pair<int, int>& p) const {
    return p.first * max_second_value_range + p.second;
  }
};

struct IntPairEquality {
  bool operator()(const std::pair<int, int>& a,
                  const std::pair<int, int>& b) const {
    return (a.first == b.first) && (a.second == b.second);
  }
};

// 2D indices.

struct Index2DHash {
  static const int max_second_value_range = 99999;
  size_t operator()(const glm::ivec2& p) const {
    return p[0] * max_second_value_range + p[1];
  }
};

struct Index2DEquality {
  bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
    return (a == b);
  }
};

// 3d indices.

struct Index3DHash {
  static const int max_dim2 = 99999;
  static const int max_dim3 = 99999;
  size_t operator()(const glm::ivec3& p) const {
    return p[0] * max_dim2 * max_dim3 + p[1] * max_dim3 + p[2];
  }
};

struct Index3DEquality {
  bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
    return (a == b);
  }
};

}
