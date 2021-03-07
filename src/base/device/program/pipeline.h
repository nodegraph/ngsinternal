#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <string>
#include <vector>

namespace ngs {

class Program;
class AttributeInfo;
class AttributeInfos;
class BlockUniformTracker;
class DefaultUniformInfos;
class FragDataInfos;
class FragDataInfo;
class DefaultUniformInfo;
class BlockUniformInfo;
class BlockUniformInfos;

class DEVICE_EXPORT Pipeline {
 public:
  Pipeline(const std::string& vertex_source, const std::string& fragment_source,
           const std::string& geometry_source = std::string());
  virtual ~Pipeline();

  void use();
  void disable();

  Program* get_program() {return _program;}

  FragDataInfos* get_frag_data_infos() {return _frag_data_infos;}
  AttributeInfos* get_attribute_infos() {return _attribute_infos;}
  DefaultUniformInfos* get_default_uniform_infos() {return _default_uniform_infos;}

#if GLES_MAJOR_VERSION >= 3
  const std::vector<BlockUniformInfos*>& get_block_uniform_infos() {return _block_uniform_infos;}
#endif

 private:
  // Default Uniforms can be set using the default uniform infos class.
  // Block Uniforms cannot be set using the associated info classes.
  // They must be uploaded to unpacked block uniforms on the gpu.

  Program* _program;
  AttributeInfos* _attribute_infos;
  DefaultUniformInfos* _default_uniform_infos;
  FragDataInfos* _frag_data_infos;

#if GLES_MAJOR_VERSION >= 3
  std::vector<BlockUniformInfos*> _block_uniform_infos;
#endif
};

}
