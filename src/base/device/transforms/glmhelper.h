#pragma once

// We use glm with swizzling.
// GLM: rotate function taking degrees as a parameter is deprecated.
// #define GLM_FORCE_RADIANS before including GLM headers to remove this message.
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/vec1.hpp>

#include <base/device/transforms/debugmatrix.h>

