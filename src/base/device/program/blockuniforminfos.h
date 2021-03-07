#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <vector>
#include <string>
#include <unordered_map>

#if GLES_MAJOR_VERSION >= 3

namespace ngs {

class BlockUniformInfo;
class Program;

class DEVICE_EXPORT BlockUniformInfos {
 public:
  BlockUniformInfos(Program& program, GLuint block_index);
  virtual ~BlockUniformInfos();

  // Block Location.
  GLuint get_block_index() const {return _block_index;}
  GLint get_byte_size() const {return _byte_size;}

  // Uniforms.
  bool uniform_exists(const std::string& name) const;
  std::vector<std::string> get_uniform_names() const;
  const BlockUniformInfo* get_uniform_info(const std::string& name) const;
  std::vector<const BlockUniformInfo*> get_uniform_infos() const;

 private:
  // Our uniform block index.
  GLuint _block_index;

  // Lookup by name or index.
  std::unordered_map<std::string,BlockUniformInfo*> _name_to_info; // We own the BlockUniformInfo's.

  // Size.
  GLint _byte_size;

};



}

#endif
