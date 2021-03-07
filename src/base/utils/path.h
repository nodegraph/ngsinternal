#pragma once
#include <base/utils/utils_export.h>

#include <deque>
#include <string>
#include <vector>
#include <functional>

namespace ngs {

class SimpleSaver;
class SimpleLoader;

// This represents a path in the entity hierarchy.
// Relative paths must must have a first element that is "." or "..".
// Absolute paths don't have ".", ".." or "/" as the first element.

// Absolute paths can be made into relative paths.
// However relative paths cannot be made into relative paths,
// as it requires an entity hierarchy in which to do so.
// Once the entity that the rel path refers to is known
// it is trivial to get the absolute path.

class UTILS_EXPORT Path {
 public:
  // We assume this path never exists in the entity hierarchy.
  static const std::deque<std::string> kInvalidPathElements;
  static const std::string kCurrentDir;
  static const std::string kParentDir;

  // Note splitting strings can be very time consuming in inner loops.
  static std::deque<std::string> split_string(const std::string& path);
  static Path string_to_path(const std::string& path);

  // Note we don't have a constructor that automatically splits strings to
  // help emphasize the slowness of splitting strings.
  Path();
  Path(const std::deque<std::string>& elements);
  Path(const Path& other);
  ~Path();

  void invalidate() {_elements = kInvalidPathElements;}

  // Serialization.
  void save(SimpleSaver& saver) const;
  void load(SimpleLoader& loader);

  // Initializers.
  bool is_absolute() const;
  void make_relative_to(const Path& base);
  size_t get_num_prefix_elements(const Path& prefix) const;

  // Operators.
  Path& operator=(const Path& other);
  bool operator==(const Path &other) const;
  bool operator!=(const Path &other) const;
  const std::string& operator[] (size_t n) const;
  Path& operator+=(const Path &rhs);

  // Deque operations.
  void clear();
  size_t size() const;
  bool empty() const;
  void push_back(const std::string& back);
  void push_front(const std::string& front);
  void pop_back();
  void pop_front();
  const std::string& front() const;
  const std::string& back() const;

  // Custom operations.
  std::string get_as_string() const;
  void reverse();
  bool begins_with(const Path& path) const;

  // Iterators.
  std::deque<std::string>::iterator begin();
  std::deque<std::string>::iterator end();
  std::deque<std::string>::const_iterator begin() const;
  std::deque<std::string>::const_iterator end() const;

  const std::deque<std::string>& get_elements() const;
 private:
  std::deque<std::string> _elements;

  friend UTILS_EXPORT Path operator+(Path lhs, const Path& rhs);
  friend UTILS_EXPORT Path operator+(const std::string& prefix, Path rhs);
  friend UTILS_EXPORT Path operator+(Path lhs, const std::string& suffix);
};

UTILS_EXPORT Path operator+(Path lhs, const Path& rhs);
UTILS_EXPORT Path operator+(const std::string& prefix, Path rhs);
UTILS_EXPORT Path operator+(Path lhs, const std::string& suffix);

struct UTILS_EXPORT PathHasher
{
  size_t operator()(const Path& k) const
  {
    std::string flat = k.get_as_string();
    return std::hash<std::string>()(flat);
  }
};

}
