#pragma once
#include <base/utils/utils_export.h>

#include <string>
#include <vector>

namespace ngs {

std::vector<std::string> remove_empty_strings(const std::vector<std::string>& src);

UTILS_EXPORT std::string escape_char(char c);
UTILS_EXPORT std::string escape_string(const std::string& s);
UTILS_EXPORT std::vector<std::string> escape_strings(const std::vector<std::string> & delimiters);

UTILS_EXPORT std::vector<std::string> split(const std::string & s, const std::vector<std::string> & delimiters);
UTILS_EXPORT std::vector<std::string> split(const std::string & s, const std::string & delimiter);
UTILS_EXPORT std::vector<std::string> split_by_expr(const std::string & s, const std::string expr);

UTILS_EXPORT std::string replace_by_regex(const std::string &src, const std::string& expr, const std::string& replacer);

UTILS_EXPORT std::string join(const std::vector<std::string> & parts, const std::string & delimiter);

}

