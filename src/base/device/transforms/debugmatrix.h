#pragma once

#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>
#include <ostream>

// glm::ivecs
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::ivec2 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::ivec3 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::ivec4 &vec);

// glm::uvecs
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::uvec2 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::uvec3 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::uvec4 &vec);

// glm::vecs
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::vec2 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::vec3 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::vec4 &vec);

// glm::dvecs
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::dvec2 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::dvec3 &vec);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::dvec4 &vec);

// glm::mats
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::mat4 &m);
std::ostream DEVICE_EXPORT &operator<< (std::ostream &out, const glm::dmat4 &m);
