#pragma once
#include <base/utils/utils_export.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <cassert>

#include <base/utils/simpletypes.h>

namespace ngs {

class Bits;

UTILS_EXPORT void open_output_file_stream(const std::string& filename, std::ofstream& ofs);

class UTILS_EXPORT SimpleSaver {
 public:
  SimpleSaver(std::ostream& os); // Borrows ownership of the output stream.
  ~SimpleSaver();

  // Save primitive types. (strings are considered primitive)
  template<class DataType>
  void save(const DataType& data);

  // Save raw data.
  void save_raw(const void*, size_t num_bytes);

  // Save vector of primitives.
  template<class DataType>
  void save_vector(const std::vector<DataType>& data);

  // Save deque of primitives.
  template<class DataType>
  void save_deque(const std::deque<DataType>& data);

private:
  std::ostream& _output;
};

// -------------------------------------------------------------------
// General implementation.
// -------------------------------------------------------------------

//template<class DataType>
//void SimpleSaver::save(const DataType &data) {
//  CHECK_FUNDAMENTAL_TYPE();
////  CHECK_NOT_SPECIALIZED();
//
//  assert(sizeof(DataType)>=8);
//  // Write out the data.
//  _output.write(reinterpret_cast<const char*>(&data), sizeof(DataType));
//}

template<class DataType>
void SimpleSaver::save_vector(const std::vector<DataType> &data) {
  // The length.
  size_t length = data.size();
  save(length);

  // The primitives.
  for (size_t i=0; i<length; ++i) {
    save(data[i]);
  }
}

template<class DataType>
void SimpleSaver::save_deque(const std::deque<DataType> &data) {
  // The length.
  size_t length = data.size();
  save(length);

  // The primitives.
  for (size_t i=0; i<length; ++i) {
    save(data[i]);
  }
}

// -------------------------------------------------------------------
// Primitive Specializations.
// -------------------------------------------------------------------



template<> UTILS_EXPORT
void SimpleSaver::save<long>(const long &data);

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned long>(const unsigned long &data);

template<> UTILS_EXPORT
void SimpleSaver::save<int>(const int &data);

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned int>(const unsigned int &data);

template<> UTILS_EXPORT
void SimpleSaver::save<long long>(const long long &data);

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned long long>(const unsigned long long &data);

//template<> UTILS_EXPORT
//void SimpleSaver::save<short>(const short &data);
//
//template<> UTILS_EXPORT
//void SimpleSaver::save<unsigned short>(const unsigned short &data);

template<> UTILS_EXPORT
void SimpleSaver::save<float>(const float &data);

template<> UTILS_EXPORT
void SimpleSaver::save<bool>(const bool &data);

template<> UTILS_EXPORT
void SimpleSaver::save<std::string>(const std::string &data);

template<> UTILS_EXPORT
void SimpleSaver::save<std::wstring>(const std::wstring &data);

template<> UTILS_EXPORT
void SimpleSaver::save<Bits>(const Bits& data);

template<> UTILS_EXPORT
void SimpleSaver::save<void*>(void* const &data);

template<> UTILS_EXPORT
void SimpleSaver::save<short>(const short &data);

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned char>(const unsigned char &data);

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned short>(const unsigned short &data);

}
