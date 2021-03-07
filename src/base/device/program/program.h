#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <string>
#include <vector>

namespace ngs {

class Shader;
class AttributeInfo;
class UniformInfo;
class BlockUniformInfo;
class DefaultUniformInfo;

class DEVICE_EXPORT Program {
 public:
  Program();
  virtual ~Program();

  // Our gl name.
  GLuint get_name();

  // Shader Attachment.
  void attach_shader(const Shader& shader);
  void detach_shader(const Shader& shader);

  // Linking.
  void link();
  bool is_linked();

  // Usage.
  void use();
  void disable();

  // Error Info.
  std::string get_info_log();

  // Setup transform feedback varyings. Must be called before linking.
#if GLES_MAJOR_VERSION >= 3
  void set_transform_feedback_varyings(const std::vector<std::string>& names);
#endif

  // Vertex shader limits.
#if GLES_MAJOR_VERSION >= 3
  // The maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a vertex shader.
  // The value must be at least 1024
  GLint get_max_vertex_uniforms_in_default_block();
  // The number of words for vertex shader uniform variables in all uniform blocks (including default). The value must be at least 1.
  GLint get_max_vertex_uniforms();
#endif

  // Frag Data.
#if GLES_MAJOR_VERSION >= 3
  GLint get_frag_data_location(const std::string& name);
#endif
#if GLES_MAJOR_VERSION >= 100
  GLint get_frag_data_index(const std::string& name);
#endif

  // Attributes.
  GLint get_num_attributes();
  GLint get_max_attribute_location();
  AttributeInfo* create_attribute_info(GLuint index);
  std::vector<AttributeInfo*> create_all_attribute_infos();
  void set_attribute_location(const AttributeInfo* info);

  // All Uniforms. (default and block)
  GLint get_num_uniforms();
  GLint get_max_uniform_name_length();
  std::string get_uniform_name(GLuint uniform_index);
  void get_uniform_names(std::vector<std::string>& names);
  std::vector<GLuint> get_uniform_indices();
#if GLES_MAJOR_VERSION >= 3
  std::vector<GLuint> get_uniform_indices(std::vector<std::string>& uniform_names);
#endif

  // Default Uniforms.
  std::vector<GLuint> get_default_uniform_indices();
  GLint get_default_uniform_location(const char* name);
  DefaultUniformInfo* create_default_uniform_info(GLuint uniform_index);
  std::vector<DefaultUniformInfo*> create_default_uniform_infos();


#if GLES_MAJOR_VERSION >= 3

  // Uniform Blocks. -- uniform blocks have an index, a byte size and a name.
  GLint get_num_uniform_blocks();
  GLuint get_uniform_block_index(const char* block_name);
  std::string get_uniform_block_name(GLuint block_index);
  GLint get_uniform_block_size(GLuint block_index);

  // Uniform Blocks contain uniforms inside them.
  // We call these the Block Uniforms.
  // Block Uniforms have an index, a byte offset and a name.
  std::vector<GLint> get_block_uniform_indices(GLuint block_index);
  std::vector<GLuint> get_block_uniform_indices();
  GLint get_block_uniform_offset(GLuint uniform_index);
  std::vector<GLint> get_block_uniform_offsets(const std::vector<GLuint>& uniform_indices);
  BlockUniformInfo* create_block_uniform_info(GLuint uniform_index);
  GLint get_uniform_block_index_for_block_uniform(GLuint uniform_index);

  // Our BlockUniformInfo packages up information about block uniforms.
  std::vector<BlockUniformInfo*> create_block_uniform_infos(const char* name);
  std::vector<BlockUniformInfo*> create_block_uniform_infos(GLuint block_index);
  std::vector<BlockUniformInfo*> create_block_uniform_infos();

  // Uniform Block Bindings.
  GLint get_max_uniform_block_binding();
  void set_uniform_block_binding(GLuint block_index, GLuint uniform_block_binding);

#endif


 private:
  void create_name();
  void remove_name();

  GLuint _name;
};

}
