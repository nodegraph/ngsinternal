#pragma once
#include <base/utils/utils_export.h>
#include <vector>

namespace ngs {

class ViewportParams;

UTILS_EXPORT void get_sin_samples(int num_samples, std::vector<float>& samples);
UTILS_EXPORT void get_cos_samples(int num_samples, std::vector<float>& samples);

}
