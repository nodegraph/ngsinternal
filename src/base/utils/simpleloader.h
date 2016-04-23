#pragma once
#include <base/utils/utils_export.h>
#include <base/utils/simpletypes.h>
#include <base/utils/bits.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>


namespace ngs {

//class Bits;

UTILS_EXPORT void open_input_file_stream(const std::string& filename, std::ifstream& ifs);
UTILS_EXPORT void open_input_string_stream(const std::string& contents, std::istringstream& iss);

UTILS_EXPORT Bits* create_bits_from_input_stream(std::istream& is);
UTILS_EXPORT Bits* create_bits_from_string(const std::string& contents);
UTILS_EXPORT Bits* create_bits_from_raw(const char* raw, size_t size);

class UTILS_EXPORT SimpleLoader {
 public:
  SimpleLoader(Bits* bits); // Takes ownership of bits.
  SimpleLoader(size_t bits_length, const char* bits); // borrows reference to bits.
  ~SimpleLoader();

  const Bits* get_bits();

  // Load primitive types. (strings are considered primitive)
  template<class DataType>
  void load(DataType& data);

  // Load raw data.
  void load_raw(void* raw, size_t num_bytes);

  // Load vector of primitive types.
  template<class DataType>
  void load_vector(std::vector<DataType>& data);

  // Load deque of primitive types.
  template<class DataType>
  void load_deque(std::deque<DataType>& data);

  // Rewind from size of primitive type.
  template<class DataType>
  void rewind(const DataType& data);

  // Rewind from byte size.
  void rewind_raw(size_t num_bytes);

  // Rewind from a vector primitive types.
  template<class DataType>
  void rewind_vector(const std::vector<DataType>& data);

  // Low level access.
  void read(void* dest, size_t num_bytes);
  void seekg(size_t offset);

 private:
  void load_file_to_buffer(std::istream& is);

  Bits* _bits;
  char* _pos;
};

// -------------------------------------------------------------------
// General implementation.
// -------------------------------------------------------------------

//template<class DataType>
//void SimpleLoader::load(DataType& data) {
//  CHECK_FUNDAMENTAL_TYPE();
////  CHECK_NOT_SPECIALIZED();
//
//  assert(sizeof(DataType)>=8);
//  // Read the data.
//  read(&data, sizeof(DataType));
//}

template<class DataType>
void SimpleLoader::load_vector(std::vector<DataType>& data) {
  // Read the length.
  size_t length;
  load(length);

  // Read the data.
  data.resize(length);
  for (size_t i=0; i<length; ++i) {
    load(data[i]);
  }
}

template<class DataType>
void SimpleLoader::load_deque(std::deque<DataType>& data) {
  // Read the length.
  size_t length;
  load(length);

  // Read the data.
  data.resize(length);
  for (size_t i=0; i<length; ++i) {
    load(data[i]);
  }
}

template<class DataType>
void SimpleLoader::rewind(const DataType& data) {
  CHECK_FUNDAMENTAL_OR_ENUM_TYPE();
//  CHECK_NOT_SPECIALIZED();
  // Rewind.
  size_t size = sizeof(DataType);
  _pos -= size;
}

template<class DataType>
void SimpleLoader::rewind_vector(const std::vector<DataType>& data) {
  for (size_t i=0; i<data.size(); ++i) {
    rewind(data[i]);
  }
}

// -------------------------------------------------------------------
// Primitive Load Specializations.
// -------------------------------------------------------------------

template<> UTILS_EXPORT
void SimpleLoader::load<long>(long& data);

template<> UTILS_EXPORT
void SimpleLoader::load<unsigned long>(unsigned long& data);

template<> UTILS_EXPORT
void SimpleLoader::load<int>(int& data);

template<> UTILS_EXPORT
void SimpleLoader::load<unsigned int>(unsigned int& data);

template<> UTILS_EXPORT
void SimpleLoader::load<long long>(long long& data);

template<> UTILS_EXPORT
void SimpleLoader::load<unsigned long long>(unsigned long long& data);

template<> UTILS_EXPORT
void SimpleLoader::load<float>(float& data);

template<> UTILS_EXPORT
void SimpleLoader::load<bool>(bool& data);

template<> UTILS_EXPORT
void SimpleLoader::load<std::string>(std::string& data);

template<> UTILS_EXPORT
void SimpleLoader::load<std::wstring>(std::wstring &data);

template<> UTILS_EXPORT
void SimpleLoader::load<Bits>(Bits& data);

template<> UTILS_EXPORT
void SimpleLoader::load<void*>(void* &data);

template<> UTILS_EXPORT
void SimpleLoader::load<short>(short& data);

template<> UTILS_EXPORT
void SimpleLoader::load<unsigned short>(unsigned short& data);

template<> UTILS_EXPORT
void SimpleLoader::load<unsigned char>(unsigned char& data);

// -------------------------------------------------------------------
// Primitive Rewind Specializations.
// -------------------------------------------------------------------

template<> UTILS_EXPORT
void SimpleLoader::rewind<long>(const long& data);

template<> UTILS_EXPORT
void SimpleLoader::rewind<unsigned long>(const unsigned long& data);

template<> UTILS_EXPORT
void SimpleLoader::rewind<std::string>(const std::string& data);

template<> UTILS_EXPORT
void SimpleLoader::rewind<std::wstring>(const std::wstring &data);

template<> UTILS_EXPORT
void SimpleLoader::rewind<Bits>(const Bits& data);

template<> UTILS_EXPORT
void SimpleLoader::rewind<void*>(void* const &  data);

}
