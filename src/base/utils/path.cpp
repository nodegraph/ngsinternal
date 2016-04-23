#include <base/memoryallocator/taggednew.h>
#include <base/utils/path.h>

#include <base/utils/stringutil.h>
#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <algorithm>
#include <iterator>

namespace ngs {

const std::string Path::kCurrentDir(".");
const std::string Path::kParentDir("..");

Path::Path() {
}

Path::Path(const std::vector<std::string>& elements) {
  for (const std::string& s : elements) {
    _elements.push_back(s);
  }
}

Path::Path(const std::deque<std::string>& elements)
    : _elements(elements) {
}

Path::Path(const std::string& full_path) {
  set(full_path);
}

Path::Path(const char* full_path_pointer) {
  std::string full_path(full_path_pointer);
  set(full_path);
}

Path::Path(const Path& other)
    : _elements(other._elements) {
}

Path::~Path() {
}

void Path::set(const std::string& full_path) {
  std::string delimiter = "/";
  std::vector<std::string> path = split(full_path, delimiter);
  _elements.insert(_elements.end(),path.begin(),path.end());
}

bool Path::is_absolute() const {
  if (_elements.empty()) {
    return true;
  } else if ((_elements.front() == kCurrentDir) || (_elements.front() == kParentDir)) {
    return false;
  }
  return true;
}

void Path::make_relative_to(const Path& base) {
  if (!is_absolute()) {
    assert(false);
  }
  std::deque<std::string> elements;
  size_t num = get_num_prefix_elements(base);
  for (size_t i = base.size(); i-->num;) {
    elements.push_back("..");
  }
  for (size_t i = num; i<_elements.size(); ++i) {
    elements.push_back(_elements[i]);
  }
  // Finally set our new relative path.
  _elements = elements;
}

size_t Path::get_num_prefix_elements(const Path& prefix) const {
  for (size_t i = 0; i < std::min(size(), prefix.size()); ++i) {
    if (_elements[i] != prefix._elements[i]) {
      return i;
    }
  }
  return prefix.size();
}

void Path::save(SimpleSaver& saver) const{
  saver.save_deque(_elements);
}

void Path::load(SimpleLoader& loader) {
  loader.load_deque(_elements);
}

// Operators.
Path& Path::operator=(const Path& other) {
  _elements = other._elements;
  return *this;
}
bool Path::operator==(const Path &other) const {
  if (_elements == other._elements) {
    return true;
  }
  return false;
}
bool Path::operator!=(const Path &other) const {
  return (!(*this == other));
}
const std::string& Path::operator[] (size_t n) const {
  return _elements[n];
}
Path& Path::operator+=(const Path &rhs) {
  _elements.insert(_elements.end(), rhs._elements.begin(), rhs._elements.end());
  return *this;
}

// Deque operations.
void Path::clear() {
  _elements.clear();
}
size_t Path::size() const {
  return _elements.size();
}
bool Path::empty() const {
  return _elements.empty();
}

void Path::push_back(const std::string& back) {
  _elements.push_back(back);
}
void Path::push_front(const std::string& front) {
  _elements.push_front(front);
}
void Path::pop_back() {
  _elements.pop_back();
}
void Path::pop_front(){
  _elements.pop_front();
}
const std::string& Path::front() const{
  return _elements.front();
}
const std::string& Path::back() const{
  return _elements.back();
}

// Custom operations.
std::string Path::get_as_string() const {
  std::string flattened;
  if (is_absolute()) {
    flattened = "/";
  }

  std::deque<std::string>::const_iterator iter;
  for (iter = _elements.begin(); iter != _elements.end(); ++iter) {
    flattened += *iter;
    if (std::next(iter) != _elements.end()) {
      flattened += "/";
    }
  }
  return flattened;
}

void Path::reverse() {
  std::reverse(_elements.begin(), _elements.end());
}

bool Path::begins_with(const Path& path) const {
  if (path.size()>size()) {
    return false;
  }
  for (size_t i=0; i<path.size(); ++i) {
    if (path._elements[i]!=_elements[i]) {
      return false;
    }
  }
  return true;
}

// Iterators.
std::deque<std::string>::iterator Path::begin() {
  return _elements.begin();
}

std::deque<std::string>::iterator Path::end() {
  return _elements.end();
}

std::deque<std::string>::const_iterator Path::begin() const {
  return _elements.begin();
}

std::deque<std::string>::const_iterator Path::end() const {
  return _elements.end();
}

const std::deque<std::string>& Path::get_elements() const {
  return _elements;
}

// Non-member operators.

Path operator+(Path lhs, const Path& rhs)
{
  lhs += rhs;
  return lhs;
}

Path operator+(const std::string& prefix, Path rhs)
{
  std::string delimiter = "/";
  std::vector<std::string> path = split(prefix, delimiter);
  rhs._elements.insert(rhs._elements.begin(),path.begin(),path.end());
  return rhs;
}

Path operator+(Path lhs, const std::string& suffix)
{
  std::string delimiter = "/";
  std::vector<std::string> path = split(suffix, delimiter);
  lhs._elements.insert(lhs._elements.end(),path.begin(),path.end());
  return lhs;
}

}
