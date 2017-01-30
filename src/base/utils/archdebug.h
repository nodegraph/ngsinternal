#if ARCH == ARCH_ANDROID
#include <android/log.h>
#endif

#if ARCH == ARCH_ANDROID
// x is comma separated list of arguments, similar to printf arguments.
#define log_warn(x) \
  __android_log_print(ANDROID_LOG_WARN, "our_cool_app", x);

#endif
