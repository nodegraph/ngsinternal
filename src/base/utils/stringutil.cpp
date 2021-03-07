#include "stringutil.h"

#include <iostream>
#include <regex>
#include <unordered_map>
#include <set>
#include <cctype>
#include <algorithm>

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

std::vector<std::string> split_on_capitals(const std::string& source) {
  std::vector<std::string> splits;
  std::string split;
  for (size_t i = 0; i < source.size(); ++i) {
    if (i == 0) {
      split.push_back(source[i]);
    } else {
      if (std::isupper(source[i]) && std::islower(source[i - 1])) {
        // This case is when our current letter is upper case but our previous letter isn't.
        splits.push_back(split);
        split.clear();
      } else if (std::islower(source[i]) && std::isupper(source[i - 1])) {
        // This case is when our current letter is lower case but our previous letter isn't.
        char back = split.back();
        split.pop_back();
        if (!split.empty()) {
          splits.push_back(split);
        }
        split.clear();
        split.push_back(back);
      }
      // Otherwise our current letter and the previous letter are the same case.
      // We always need to push the current letter on the current split in all cases.
      split.push_back(source[i]);
    }
  }
  // Push back our final split.
  splits.push_back(split);
  return splits;
}

std::string to_lower_case(const std::string& source) {
  std::string lower;
  lower.resize(source.size());
  std::transform(source.begin(), source.end(), lower.begin(),[](unsigned char c){return std::tolower(c);});
  return lower;
}

std::string concatenate(const std::vector<std::string>& splits, const std::string& glue) {
  // Concatenate the splits with a space.
  std::string glued;
  for (size_t i=0; i<splits.size(); ++i) {
    if (i == 0) {
      glued = to_lower_case(splits[i]);
    } else {
      glued += glue;
      glued += to_lower_case(splits[i]);
    }
  }
  return glued;
}

}
