#include "stringutil.h"

#include <iostream>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <set>

namespace {

// From the ECMAScript web page for c++ expressions. (http://www.cplusplus.com/reference/regex/ECMAScript/)
// Any character can be escaped except those which form any of the special character sequences above.
// Needed for: ^ $ \ . * + ? ( ) [ ] { } |
const std::set<char> special_chars = {
    '^', '$', '\\', '.', '*', '+', '?', '(', ')', '[', ']', '{', '}', '|'
};

}

namespace ngs {

std::vector<std::string> remove_empty_strings(const std::vector<std::string>& src) {
  std::vector<std::string> results;
  for (size_t i = 0; i < src.size(); i++) {
    if (src[i].empty()) {
      continue;
    }
    results.push_back(src[i]);
  }
  return results;
}

std::string escape_char(char c) {
  std::string result;
  if (special_chars.count(c)) {
    result += '\\';
  }
  result += c;
  return result;
}

std::string escape_string(const std::string & s) {
  std::string result;
  for(const char& c:s) {
    result += escape_char(c);
  }
  return result;
}

std::vector<std::string> escape_strings(const std::vector<std::string> & delimiters) {
  std::vector<std::string> results;
  for(const std::string& s:delimiters) {
    results.push_back(escape_string(s));
  }
  return results;
}

std::string join(const std::vector<std::string> & parts, const std::string & delimiter) {
  std::string result;
  for (const std::string& part:parts) {
    if(!result.empty()) {
      result+=delimiter;
    }
    result+=part;
  }
  return result;
}

std::string replace_by_regex(const std::string &src, const std::string& expr, const std::string& replacer) {
  std::regex rgx(expr);
  return std::regex_replace(src, rgx, replacer);
}

std::vector<std::string> split(const std::string & s, const std::vector<std::string> & delimiters) {
  std::string joined = join(escape_strings(delimiters), "|");
  return split_by_expr(s, joined);
}

std::vector<std::string> split(const std::string & s, const std::string & delimiter) {
  std::vector<std::string> delimiters = { delimiter };
  return split(s, delimiters);
}

std::vector<std::string> split_by_expr(const std::string & s, const std::string expr) {
  std::regex rgx(expr);
  std::sregex_token_iterator first_string(s.begin(), s.end(), rgx, -1);
  std::sregex_token_iterator last_string;
  std::vector<std::string> results(first_string, last_string);
  return remove_empty_strings(results);
}

}
