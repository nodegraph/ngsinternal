#include <base/utils/envutil.h>
#include <stdlib.h>

namespace ngs {

std::string get_env(std::string const & var) {
  char * value = NULL; //getenv(var.c_str());
  if (!value) {
    return "";
  }
  return value;
}

#if ARCH == ARCH_WINRT
char * getenv(const char *varname) {
  return "";
}
#endif

}
