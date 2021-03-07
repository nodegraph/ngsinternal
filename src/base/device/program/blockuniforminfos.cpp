#include <base/memoryallocator/taggednew.h>
#include <base/device/program/program.h>
#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/program/blockuniforminfo.h>
#include <base/device/program/blockuniforminfos.h>

#if GLES_MAJOR_VERSION >= 3

namespace ngs {

BlockUniformInfos::BlockUniformInfos(Program& program, GLuint block_index):
  _block_index(block_index){

  _byte_size = program.get_uniform_block_size(_block_index);

  // Create the vector of all the uniform types.
  std::vector<BlockUniformInfo*> infos = program.create_block_uniform_infos(_block_index);

  // Fill the name lookup table.
  for (size_t i = 0; i < infos.size(); i++) {
    _name_to_info[infos[i]->get_name()] = infos[i];
  }
}

BlockUniformInfos::~BlockUniformInfos() {
  for (auto &kv: _name_to_info) {
    delete_ff(kv.second);
  }
  _name_to_info.clear();
}

// -------------------------------------------------------------------------------------------
// PackedUniform specific methods.
// -------------------------------------------------------------------------------------------

bool BlockUniformInfos::uniform_exists(const std::string& name) const {
  return _name_to_info.count(name);
}

std::vector<std::string> BlockUniformInfos::get_uniform_names() const {
  std::vector<std::string> names;
  names.resize(_name_to_info.size());
  for (auto &kv: _name_to_info) {
    names.push_back(kv.first);
  }
  return names;
}

const BlockUniformInfo* BlockUniformInfos::get_uniform_info(const std::string& name) const {
  if (!uniform_exists(name)) {
    return NULL;
  }
  return _name_to_info.at(name);
}

std::vector<const BlockUniformInfo*> BlockUniformInfos::get_uniform_infos() const{
  std::vector<const BlockUniformInfo*> infos;
  infos.resize(_name_to_info.size());
  for (auto &kv: _name_to_info) {
    infos.push_back(kv.second);
  }
  return infos;
}



}

#endif
