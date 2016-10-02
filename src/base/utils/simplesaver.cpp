#include <base/utils/simplesaver.h>
#include <base/memoryallocator/taggednew.h>
#include <string>
#include <base/utils/bits.h>

namespace ngs {

// Notes:
// We write out all int and long types as 64 bits.
// This makes the data format compatible across linux and windows as well as 32 bit and 64 platforms.
// 64 bit architectures:
// -- linux: (unsigned) long is 64 bits
// -- windows: (unsigned) long is 32 bits
// -- so we always save out (unsigned) long as 64 bits so that it will be compatible between linux and windows on 64 bit.
// -- note on windows64, on file read the 64 bits may get truncated to 32 bits.
// 32 bit architectures:
// -- size_t is typedef'd to unsigned int on 32 bit windows and linux.
// -- we always save out ints as 64 bit so that it will be compatible with 64 bit architectures.
// -- note on file read the 64 bits may get truncated to 32 bits.


void open_output_file_stream(const std::string& filename, std::ofstream& ofs) {
  ofs.open(filename.c_str(), std::ios::out | std::ios::binary);
  if (!ofs.is_open()) {
    std::cerr << "Error: unable to open the binary file for writing: "
              << filename << "\n";
    assert(false);
  }
}

SimpleSaver::SimpleSaver(std::ostream& os):
  _output(os){
}

SimpleSaver::~SimpleSaver() {
}

void SimpleSaver::save_raw(const void* raw, size_t num_bytes) {
  _output.write(reinterpret_cast<const char*>(raw), num_bytes);
}


template<> UTILS_EXPORT
void SimpleSaver::save<EntityDID>(const EntityDID &data) {
  size_t num = static_cast<size_t>(data);
  save(num);
}

template<> UTILS_EXPORT
void SimpleSaver::save<ComponentDID>(const ComponentDID &data) {
  size_t num = static_cast<size_t>(data);
  save(num);
}

// We always store long as 64 bits.
// On 64bit arch, long is 32 bits on windows, and 64 bits on linux.
template<> UTILS_EXPORT
void SimpleSaver::save<long>(const long &data) {
  long long data64 = static_cast<long long>(data);
  save(data64);
}

// We always store unsigned long as 64 bits.
// On 64bit arch, unsigned long is 32 bits on windows, and 64 bits on linux.
template<> UTILS_EXPORT
void SimpleSaver::save<unsigned long>(const unsigned long &data) {
  unsigned long long data64 = static_cast<unsigned long long>(data);
  save(data64);
}

// We always store int as 64 bits as well.
// Windows and linux define int as the same size, but size_t is sometimes
// defined as unsigned int or unsigned long so for safety we store it as 64 bits.
template<> UTILS_EXPORT
void SimpleSaver::save<int>(const int &data) {
  long long data64 = static_cast<long long>(data);
  save(data64);
}

// We always store int as 64 bits as well.
// Windows and linux define int as the same size, but size_t is sometimes
// defined as unsigned int or unsigned long so for safety we store it as 64 bits.
template<> UTILS_EXPORT
void SimpleSaver::save<unsigned int>(const unsigned int &data) {
  unsigned long long data64 = static_cast<unsigned long long>(data);
  save(data64);
}

template<> UTILS_EXPORT
void SimpleSaver::save<long long>(const long long &data) {
  _output.write(reinterpret_cast<const char*>(&data), sizeof(long long));
}

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned long long>(const unsigned long long &data) {
  _output.write(reinterpret_cast<const char*>(&data), sizeof(unsigned long long));
}

template<> UTILS_EXPORT
void SimpleSaver::save<float>(const float &data) {
  _output.write(reinterpret_cast<const char*>(&data), sizeof(float));
}

template<> UTILS_EXPORT
void SimpleSaver::save<bool>(const bool &data) {
  _output.write(reinterpret_cast<const char*>(&data), sizeof(bool));
}

template<> UTILS_EXPORT
void SimpleSaver::save<std::string>(const std::string &data) {
  size_t length = data.length();
  save(length);
  _output.write(data.c_str(), length*sizeof(char));
}

template<> UTILS_EXPORT
void SimpleSaver::save<std::wstring>(const std::wstring &data) {
  size_t length = data.length();
  save(length);
  _output.write(reinterpret_cast<const char*>(data.c_str()), length*sizeof(wchar_t));
}

template<> UTILS_EXPORT
void SimpleSaver::save<Bits>(const Bits& data) {
  size_t length = data.num_bytes;
  save(length);
  _output.write((const char*) data.raw, length);
}

template<> UTILS_EXPORT
void SimpleSaver::save<void*>(void* const &data) {
  std::cerr << "Error: error attempt to save a pointer to a file.\n";
}

template<> UTILS_EXPORT
void SimpleSaver::save<short>(const short& data) {
  _output.write(reinterpret_cast<const char*>(&data), sizeof(short));
}

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned short>(const unsigned short& data) {
  _output.write(reinterpret_cast<const char*>(&data), sizeof(unsigned short));
}

template<> UTILS_EXPORT
void SimpleSaver::save<unsigned char>(const unsigned char& data) {
  _output.write(reinterpret_cast<const char*>(&data), sizeof(unsigned char));
}


}
