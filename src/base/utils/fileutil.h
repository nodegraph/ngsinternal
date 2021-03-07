#pragma once
#include <base/utils/utils_export.h>

#include <string>

namespace ngs {

bool UTILS_EXPORT file_exists(const std::string& filename);

// Reads data from a file as a binary glob.
// With binary files there will likely be NULLs in the returned string.
// Be careful with string routines which assume that NULLs are the end of the string.
UTILS_EXPORT std::string load_data(const std::string& filename);

}
