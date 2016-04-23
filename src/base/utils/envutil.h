#pragma once
#include <base/utils/utils_export.h>
#include <string>

namespace ngs {

UTILS_EXPORT std::string get_env(std::string const & var);

// Windows universal platform doesn't provide getenv.
// However some of our external libs do, so we define a stub dummy for it.
#if ARCH == ARCH_WINRT
extern "C" {
  UTILS_EXPORT char * getenv(const char *varname);
}
#endif

}
