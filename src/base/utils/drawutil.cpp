#include "drawutil.h"

#include <iostream>

//#define _USE_MATH_DEFINES
#include <cmath>
#include <boost/math/constants/constants.hpp>

namespace ngs {

void get_sin_samples(int num_samples, std::vector<float>& samples) {
  const float pi = boost::math::constants::pi<float>();
  const float scale = 2.0f * pi / static_cast<float>(num_samples);
  std::cerr.precision(15);

  samples.resize(num_samples);
  for (int i = 0; i < num_samples; i++) {
    float x = static_cast<float>(i + 1) * scale;
    samples[i] = sin(x);
    //std::cerr<<samples[i]<<"\n";
  }
}

void get_cos_samples(int num_samples, std::vector<float>& samples) {
  const float pi = boost::math::constants::pi<float>();
  const float scale = 2.0f * pi / static_cast<float>(num_samples);
  std::cerr.precision(15);

  samples.resize(num_samples);
  for (int i = 0; i < num_samples; i++) {
    float x = static_cast<float>(i + 1) * scale;
    samples[i] = cos(x);
    //std::cerr<<samples[i]<<"\n";
  }
}

}
