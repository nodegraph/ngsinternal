#include "simpleloader.h"
#include <base/memoryallocator/taggednew.h>
#include <base/utils/bits.h>

#include <cstring>
#include <cassert>
#include <sstream>

namespace ngs {

void open_input_file_stream(const std::string& filename, std::ifstream& ifs) {
  ifs.open(filename.c_str(), std::ios::in | std::ios::binary);
}

void open_input_string_stream(const std::string& contents, std::istringstream& iss) {
  iss.clear();
  iss.str(contents);
}

Bits* create_bits_from_input_stream(std::istream& is) {

  // Get the length of the file in bytes.
  std::streamoff begin = is.tellg();
  is.seekg(0, std::ios::end);
  std::streamoff end = is.tellg();
  std::streamoff length = end - begin;
  is.seekg(0, std::ios::beg);

  // Allocate the memory:
  Bits* bits = new_ff Bits();
  bits->resize(length);

  // Read the data all at once:
  is.read(bits->raw, length);

  return bits;
}

Bits* create_bits_from_string(const std::string& contents) {
  return create_bits_from_raw(&contents[0],contents.size());
}

Bits* create_bits_from_raw(const char* raw, size_t size) {
  // Allocate the memory:
  Bits* bits = new_ff Bits();
  bits->resize(size);

  // Copy the bits.
  memcpy(bits->raw, raw, size);

  return bits;
}


SimpleLoader::SimpleLoader(Bits* bits):
    _bits(bits) {
  _pos = _bits->raw;
}

SimpleLoader::SimpleLoader(size_t bits_length, const char* bits)
    : _bits(new_ff Bits(bits_length, bits)) {
  _pos = _bits->raw;
}

SimpleLoader::~SimpleLoader() {
  delete_ff(_bits);
  _pos = NULL;
}

const Bits* SimpleLoader::get_bits() {
  return _bits;
}

void SimpleLoader::load_file_to_buffer(std::istream& is) {

  // Get the length of the file in bytes.
  std::streamoff begin = is.tellg();
  is.seekg(0, std::ios::end);
  std::streamoff end = is.tellg();
  std::streamoff length = end - begin;
  is.seekg(0, std::ios::beg);

  // Allocate the memory:
  _bits->resize(length);
  _pos = _bits->raw;

  // Read the data all at once:
  is.read(_bits->raw, length);
}

void SimpleLoader::read(void* dest, size_t num_bytes) {
  std::memcpy(dest, _pos, num_bytes);
  _pos += num_bytes;
}

void SimpleLoader::seekg(size_t offset_bytes) {
  _pos = _bits->raw + offset_bytes;
}

void SimpleLoader::load_raw(void* raw, size_t num_bytes) {
  read(raw, num_bytes);
}

void SimpleLoader::rewind_raw(size_t num_bytes) {
  _pos -= num_bytes;
}

// -------------------------------------------------------------------
// Primitive Load Specializations.
// -------------------------------------------------------------------

// Longs are always stored as 64 bit long longs by the SimpleSaver.
// On windows they are 4 bytes. On linux they are 8 bytes.
// So we cast to long after reading in the data.
template<> UTILS_EXPORT
void SimpleLoader::load<long>(long& data)
{
  long long data64;
  load(data64);
  data = static_cast<long>(data64);
}

// Unsigned longs are always stored as 64 bit unsigned long longs by the SimpleSaver.
// On windows they are 4 bytes. On linux they are 8 bytes.
// So we cast to long after reading in the data.
template<> UTILS_EXPORT
void SimpleLoader::load<unsigned long>(unsigned long& data)
{
  unsigned long long data64;
  load(data64);
  data = static_cast<unsigned long>(data64);
}

// Ints are also stored as 64 bit long longs by the SimpleSaver.
// Windows and linux define int as the same size, but size_t is sometimes
// defined as unsigned int or unsigned long so for safety we store it as 64 bits.
template<> UTILS_EXPORT
void SimpleLoader::load<int>(int& data)
{
  long long data64;
  load(data64);
  data = static_cast<long>(data64);
}

// Ints are also stored as 64 bit long longs by the SimpleSaver.
// Windows and linux define int as the same size, but size_t is sometimes
// defined as unsigned int or unsigned long so for safety we store it as 64 bits.
template<> UTILS_EXPORT
void SimpleLoader::load<unsigned int>(unsigned int& data)
{
  unsigned long long data64;
  load(data64);
  data = static_cast<unsigned long>(data64);
}

template<> UTILS_EXPORT
void SimpleLoader::load(long long& data) {
  read(&data, sizeof(long long));
}

template<> UTILS_EXPORT
void SimpleLoader::load(unsigned long long& data) {
  read(&data, sizeof(unsigned long long));
}

template<> UTILS_EXPORT
void SimpleLoader::load(float& data) {
  read(&data, sizeof(float));
}

template<> UTILS_EXPORT
void SimpleLoader::load(bool& data) {
  read(&data, sizeof(bool));
}

template<> UTILS_EXPORT
void SimpleLoader::load<std::string>(std::string& data) {
  size_t length;
  load(length);
  if (length == 0) {
    data = "";
  } else {
    data.resize(length);
    read(&data[0], length);
  }
}

template<> UTILS_EXPORT
void SimpleLoader::load<std::wstring>(std::wstring& data) {
  size_t length;
  load(length);
  if (length == 0) {
    data = L"";
  } else {
    data.resize(length);
    read(&data[0], length*sizeof(wchar_t));
  }
}

template<> UTILS_EXPORT
void SimpleLoader::load<Bits>(Bits& data) {
  size_t length;
  load(length);
  if (length == 0) {
    data.clear();
  } else {
    data.resize(length);
    read(data.raw, length*sizeof(char));
  }
}

template<> UTILS_EXPORT
void SimpleLoader::load<void*>(void* &data) {
  std::cerr << "Error: error attempt to load a pointer from a file.\n";
  assert(false);
}

template<> UTILS_EXPORT
void SimpleLoader::load(short& data) {
  read(&data, sizeof(short));
}

template<> UTILS_EXPORT
void SimpleLoader::load(unsigned short& data) {
  read(&data, sizeof(unsigned short));
}

template<> UTILS_EXPORT
void SimpleLoader::load<unsigned char>(unsigned char& data) {
  read(&data, sizeof(unsigned char));
}

// -------------------------------------------------------------------
// Primitive Rewind Specializations.
// -------------------------------------------------------------------

template<> UTILS_EXPORT
void SimpleLoader::rewind<long>(const long& data) {
  // Longs are always stored as 64 bit long longs by the SimpleSaver.
  // On windows they are 4 bytes. On linux they are 8 bytes.
  _pos-=8;
}

template<> UTILS_EXPORT
void SimpleLoader::rewind<unsigned long>(const unsigned long& data) {
  // Unsigned longs are always stored as 64 bit unsigned long longs by the SimpleSaver.
  // On windows they are 4 bytes. On linux they are 8 bytes.
  _pos-=8;
}

template<> UTILS_EXPORT
void SimpleLoader::rewind<std::string>(const std::string& data) {
  _pos-=data.size()*sizeof(char); // contents of the string
  _pos-=8; // the size_t length of the string
}

template<> UTILS_EXPORT
void SimpleLoader::rewind<std::wstring>(const std::wstring &data) {
  _pos-=data.size()*sizeof(wchar_t); // contents of the string
  _pos-=8; // the size_t length of the string
}

template<> UTILS_EXPORT
void SimpleLoader::rewind<Bits>(const Bits& data) {
  _pos-=data.num_bytes;
  _pos-=8; // the size_t length of the data
}

template<> UTILS_EXPORT
void SimpleLoader::rewind<void*>(void* const & data) {
  std::cerr << "Error: error attempt to rewind a file using a pointer.\n";
  assert(false);
}

}

