#include <base/utils/fileutil.h>
#include <iostream>
#include <fstream>
#include <cassert>


namespace ngs {

bool file_exists(const std::string& filename) {
  std::ifstream stream(filename);
  return stream.good();
}


std::string load_data(const std::string& filename) {
	std::ifstream file(filename.c_str(), std::ios_base::in | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Warning: could not open the binary file: " << filename << "\n";
    assert(false);
	}

	// Get the file size.
  file.seekg (0, std::ios::end);
  std::streampos size = file.tellg();
  file.seekg (0, std::ios::beg);

	// Allocate a byte array.
	std::string data;
	data.resize(size);

	// Copy the data from the file to the byte array.
	file.read(&data[0], size);
	file.close();

	// Return our data.
	return data;
}



}
