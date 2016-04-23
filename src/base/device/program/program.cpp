#include <base/device/program/program.h>
#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/program/attributeinfo.h>
#include <base/memoryallocator/taggednew.h>


#include <base/device/program/shader.h>
#include <base/device/program/blockuniforminfo.h>
#include <base/device/program/defaultuniforminfo.h>
#include <base/device/program/uniforminfo.h>

#include <iostream>
#include <vector>
#include <cassert>

namespace ngs {

Program::Program() {
  create_name();
}

Program::~Program() {
  remove_name();
}

GLuint Program::get_name() {
  return _name;
}

void Program::attach_shader(const Shader& shader) {
  gpu(glAttachShader(_name, shader.get_name()));
}

void Program::detach_shader(const Shader& shader) {
  gpu(glDetachShader(_name, shader.get_name()));
}

void Program::link() {
  gpu(glLinkProgram(_name));
  if (!is_linked()) {
    std::cerr << "Error: linking program.\n";
    std::cerr << get_info_log() << "\n";
    std::string info = get_info_log();
    assert(false);
  } else {
    //std::cerr<<"Success: linking program.\n";
    //std::cerr<<get_info_log()<<"\n";
  }
}

bool Program::is_linked() {
  int state;
  gpu(glGetProgramiv(_name, GL_LINK_STATUS, &state));
  if (state == 1) {
    return true;
  }
  return false;
}

void Program::use() {
  gpu(glUseProgram(_name));
}

void Program::disable() {
  gpu(glUseProgram(0));
}

std::string Program::get_info_log() {
  std::string log;
  int length = 0;
  gpu(glGetProgramiv(_name, GL_INFO_LOG_LENGTH, &length));
  if (length > 0) {
    int written = 0;
    log.resize(length);
    gpu(glGetProgramInfoLog(_name, length, &written, &log[0]));
  }
  return log;
}


void Program::create_name() {
  gpu(_name=glCreateProgram());
  if (_name == 0) {
    std::cerr
        << "Error insufficient memory is likely. unable to create a program object name.\n";
    assert(false);
  }
}
void Program::remove_name() {
  if (_name) {
    gpu(glDeleteProgram(_name));
    _name = 0;
  }
}

// --------------------------------------------------------------------------------------------------------------
// Transform Feedback.
// --------------------------------------------------------------------------------------------------------------

#if GLES_MAJOR_VERSION >= 3
void Program::set_transform_feedback_varyings(const std::vector<std::string>& names) {
  size_t num = names.size();
  std::vector<const char*> name_pointers;
  name_pointers.resize(num);

  for (size_t i = 0; i < names.size(); i++) {
    name_pointers[i] = &names[i][0];
  }

  glTransformFeedbackVaryings(_name, num, &name_pointers[0],
                              GL_INTERLEAVED_ATTRIBS);  //GL_SEPARATE_ATTRIBS requires multiples feedback vbos.
}
#endif

// --------------------------------------------------------------------------------------------------------------
// Vertex shader limits.
// --------------------------------------------------------------------------------------------------------------

#if GLES_MAJOR_VERSION >= 3
GLint Program::get_max_vertex_uniforms_in_default_block() {
  GLint max;
  gpu(glGetProgramiv(_name,GL_MAX_VERTEX_UNIFORM_COMPONENTS,&max));
  return max;
}

GLint Program::get_max_vertex_uniforms() {
  GLint max;
  gpu(glGetProgramiv(_name,GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,&max));
  return max;
}
#endif

// --------------------------------------------------------------------------------------------------------------
// Frag Data.
// --------------------------------------------------------------------------------------------------------------

#if GLES_MAJOR_VERSION >= 3
GLint Program::get_frag_data_location(const std::string& name) {
  // Note we prolly need to update the nvidia drivers.
  // glGetFragDataLocation will crash in multi threaded gl rendering
  // but glGetFragDataLocationEXT works just fine.
  gpu(GLint loc=glGetFragDataLocation(_name, name.c_str()));
  return loc;
}
#endif

#if GLES_MAJOR_VERSION >= 100
GLint Program::get_frag_data_index(const std::string& name) {
  gpu(GLint loc=glGetFragDataIndex(_name, name.c_str()));
  return loc;
}
#endif

// --------------------------------------------------------------------------------------------------------------
// Attributes.
// --------------------------------------------------------------------------------------------------------------

GLint Program::get_num_attributes() {
  GLint num;
  gpu(glGetProgramiv(_name,GL_ACTIVE_ATTRIBUTES, &num));
  return num;
}

GLint Program::get_max_attribute_location() {
  GLint max;
  gpu(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&max));
  return max;
}

AttributeInfo* Program::create_attribute_info(GLuint index) {
//  std::string name;
//  int buffer_size = get_max_uniform_name_length();
//  name.resize(buffer_size);

  static const int buffer_size = 2048;
  static char buffer[buffer_size];

  int size = 0;
  GLenum type;

  // Get name size and type.
  int written = 0;
  gpu(glGetActiveAttrib(_name, index, buffer_size, &written, &size, &type, buffer));

  // The number of written chars must be less than buffer_size -1.
  // Opengl won't count the NULL terminator.
  // So the max written value is buffer_size-1;
  // If we receive the max written value, then the name might actually be longer.
  // So we would need larger buffer_size constant in this case.
  assert(written<buffer_size-1);
  std::string name(buffer,written);

//  std::cerr<<"Written: " << written << "\n";
//  std::cerr<<"Attribute: "<<name<<"\n";

  // Get the attribute index location.
  gpu(GLint location=glGetAttribLocation(_name, name.c_str()));

  // Fill in the structure;
  AttributeInfo* attr = new_ff AttributeInfo();
  attr->name = name;
  attr->type = type;
  attr->size = size;
  attr->location = location;
  return attr;
}

std::vector<AttributeInfo*> Program::create_all_attribute_infos() {
  std::vector<AttributeInfo*> infos;
  int num = get_num_attributes();
  infos.resize(num);
  for (int i = 0; i < num; i++) {
    infos[i]=create_attribute_info(i);
  }
  return infos;
}

void Program::set_attribute_location(const AttributeInfo* info) {
  gpu(glBindAttribLocation(_name, info->location, info->name.c_str()););
}






















// --------------------------------------------------------------------------------------------------------------
// All uniforms.
// --------------------------------------------------------------------------------------------------------------

GLint Program::get_num_uniforms() {
  GLint num;
  gpu(glGetProgramiv(_name,GL_ACTIVE_UNIFORMS,&num));
  return num;
}
GLint Program::get_max_uniform_name_length() {
  GLint max;
  gpu(glGetProgramiv(_name, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max));
  return max;
}
std::string Program::get_uniform_name(GLuint uniform_index) {
#if GLES_MAJOR_VERSION >= 100
  std::string name;
  int max_length = get_max_uniform_name_length();
  name.resize(max_length);
  int written = 0;
  gpu(glGetActiveUniformName(_name, uniform_index, max_length, &written, &name[0]));
  name.resize(written);
  return name;
#else
  int max_length = get_max_uniform_name_length();
  std::string name;
  name.resize(max_length);
  int size=-1;
  GLenum type = GL_ZERO;
  int written = 0;
  glGetActiveUniform(_name, uniform_index, max_length, &written, &size, &type, &name[0] );
  name.resize(written);
  return name;
#endif
}

void Program::get_uniform_names(std::vector<std::string>& names) {
  names.clear();
  // Get the number of uniforms.
  int num_uniforms = -1;
  gpu(glGetProgramiv(_name, GL_ACTIVE_UNIFORMS, &num_uniforms));
  // Get the max name length.
  int max_length = get_max_uniform_name_length();
  // Loop over each uniform, collecting names.
  for(int i=0; i<num_uniforms; ++i)  {
      std::string name;
      name.resize(max_length);
      int size=-1;
      GLenum type = GL_ZERO;
      int written = 0;
      glGetActiveUniform(_name, GLuint(i), max_length, &written, &size, &type, &name[0] );
      name.resize(written);
      names.push_back(name);
  }
}

std::vector<GLuint> Program::get_uniform_indices() {
  std::vector<GLuint> indices;
  GLint num = get_num_uniforms();
  indices.resize(num);
  for (GLint i = 0; i < num; i++) {
    indices[i] = i;
  }
  return indices;
}

// This is for uniforms in named blocks.
#if GLES_MAJOR_VERSION >= 3
std::vector<GLuint> Program::get_uniform_indices(std::vector<std::string>& uniform_names) {
  GLsizei num = static_cast<GLsizei>(uniform_names.size());
  std::vector<const char*> temp;
  temp.resize(num);
  for (int i = 0; i < num; i++) {
    temp[i] = uniform_names[i].c_str();
  }

  std::vector<GLuint> indices;
  indices.resize(num);
  gpu(glGetUniformIndices(_name,num,&temp[0],&indices[0]));
  return indices;
}
#endif

// --------------------------------------------------------------------------------------------------------------
// Default uniforms.
// --------------------------------------------------------------------------------------------------------------

std::vector<GLuint> Program::get_default_uniform_indices() {
  std::vector<GLuint> all_indices = get_uniform_indices();
  GLsizei num = static_cast<GLsizei>(all_indices.size());

  std::vector<GLint> block_indices;
  block_indices.resize(num);

  std::vector<GLuint> indices;
  if (num == 0) {
    return indices;
  }

  // Get the block indices for all uniforms.
  // Default uniforms hava a block index of -1.
#if GLES_MAJOR_VERSION >= 3
  gpu(glGetActiveUniformsiv(_name,num,&all_indices[0],GL_UNIFORM_BLOCK_INDEX,&block_indices[0]));
#else
  for (size_t i = 0; i < all_indices.size(); i++) {
    char uniform_name[1024];
    GLsizei length;
    GLint size;
    GLenum type;
    glGetActiveUniform(_name, all_indices[i], 1024, &length, &size, &type,
                       uniform_name);
    block_indices[i] = -1;
//    if (type == GL_UNIFORM_BLOCK_INDEX) {
//      block_indices[i] = 1;  // Note this should actually be the block index of the uniform.
//    } else {
//      block_indices[i] = -1;
//    }
  }
#endif


  for (size_t i = 0; i < block_indices.size(); i++) {
    if (block_indices[i] == -1) {
      indices.push_back(static_cast<GLuint>(i));
    }
  }
  return indices;
}

// This is for uniforms in the default unnamed block.
// returns -1 on error.
GLint Program::get_default_uniform_location(const char* name) {
  gpu(GLint location=glGetUniformLocation(_name,name););
  return location;
}

DefaultUniformInfo* Program::create_default_uniform_info(GLuint uniform_index) {
  std::string name;
  GLsizei buffer_size = get_max_uniform_name_length();
  name.resize(buffer_size);
  GLsizei written;
  GLint size;
  GLenum data_type;

  gpu(glGetActiveUniform(_name, uniform_index, buffer_size, &written, &size, &data_type, &name[0]));
  name.resize(written);

  GLint location = 0;
  if (size == 1) {
    location = get_default_uniform_location(name.c_str());
  } else {
    std::string start_name = name + "[0]";
    location = get_default_uniform_location(start_name.c_str());
  }

  // fill in the uniform_type
  DefaultUniformInfo* info = new_ff DefaultUniformInfo();
  info->set_name(name);
  info->set_type(data_type);
  info->set_num_elements(size);
  info->set_index(uniform_index);
  info->set_location(location);
  return info;
}

std::vector<DefaultUniformInfo*> Program::create_default_uniform_infos() {
  std::vector<GLuint> indices = get_default_uniform_indices();
  std::vector<DefaultUniformInfo*> infos;
  infos.reserve(indices.size());
  for (size_t i = 0; i < indices.size(); i++) {
    DefaultUniformInfo* info = create_default_uniform_info(indices[i]);
    infos.push_back(info);
  }
  return infos;
}

// --------------------------------------------------------------------------------------------------------------
// Uniform Blocks.
// --------------------------------------------------------------------------------------------------------------

#if GLES_MAJOR_VERSION >= 3
GLint Program::get_num_uniform_blocks() {
  GLint num;
  gpu(glGetProgramiv(_name,GL_ACTIVE_UNIFORM_BLOCKS,&num));
  return num;
}

GLuint Program::get_uniform_block_index(const char* block_name) {
  gpu(GLuint index=glGetUniformBlockIndex(_name,block_name););
  return index;
}

std::string Program::get_uniform_block_name(GLuint block_index) {
  std::string name;
  GLsizei buffer_size = get_max_uniform_name_length();
  name.resize(buffer_size);
  GLsizei written = 0;
  gpu(glGetActiveUniformBlockName(_name,block_index, buffer_size, &written, &name[0]));
  name.resize(written);
  return name;
}

GLint Program::get_uniform_block_size(GLuint block_index) {
  GLint size;
  gpu(glGetActiveUniformBlockiv(_name,block_index,GL_UNIFORM_BLOCK_DATA_SIZE,&size));
  return size;
}
#endif

// --------------------------------------------------------------------------------------------------------------
// Block Uniforms.
// --------------------------------------------------------------------------------------------------------------

#if GLES_MAJOR_VERSION >= 3
std::vector<GLint> Program::get_block_uniform_indices(GLuint block_index) {
  GLint num_indices;
  gpu(glGetActiveUniformBlockiv(_name,block_index,GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_indices));

  std::vector<GLint> indices;
  indices.resize(num_indices);
  gpu(glGetActiveUniformBlockiv(_name,block_index,GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &indices[0]));
  return indices;
}

std::vector<GLuint> Program::get_block_uniform_indices() {
  std::vector<GLuint> all_indices = get_uniform_indices();
  GLsizei num = static_cast<GLsizei>(all_indices.size());

  std::vector<GLint> block_indices;
  block_indices.resize(num);

  std::vector<GLuint> indices;
  if (num == 0) {
    return indices;
  }

  // Get the block indices for all uniforms.
  // Default uniforms hava a block index of -1.
  if (glGetActiveUniformsiv) {
    gpu(glGetActiveUniformsiv(_name,num,&all_indices[0],GL_UNIFORM_BLOCK_INDEX,&block_indices[0]));
  } else {
    for (size_t i = 0; i < all_indices.size(); i++) {
      char uniform_name[1024];
      GLsizei length;
      GLint size;
      GLenum type;
      glGetActiveUniform(_name, all_indices[i], 1024, &length, &size, &type,
                         uniform_name);
      if (type == GL_UNIFORM_BLOCK_INDEX) {
        block_indices[i] = 1;  // Note this should actually be the block index of the uniform.
      } else {
        block_indices[i] = -1;
      }
    }
  }

  indices.clear();
  for (size_t i = 0; i < block_indices.size(); i++) {
    if (block_indices[i] != -1) {
      indices.push_back(static_cast<GLuint>(i));
    }
  }
  return indices;
}

GLint Program::get_block_uniform_offset(GLuint uniform_index) {
  GLint offset;
  gpu(glGetActiveUniformsiv(_name,1,&uniform_index,GL_UNIFORM_OFFSET,&offset));
  return offset;
}

std::vector<GLint> Program::get_block_uniform_offsets(const std::vector<GLuint>& uniform_indices) {
  size_t num = uniform_indices.size();
  std::vector<GLint> offsets;
  offsets.resize(num);
  // Note other info can be retrieved with this command also.
  gpu(glGetActiveUniformsiv(_name,static_cast<GLsizei>(num),&uniform_indices[0],GL_UNIFORM_OFFSET,&offsets[0]));
  return offsets;
}

BlockUniformInfo* Program::create_block_uniform_info(GLuint uniform_index) {
  std::string name;
  GLsizei buffer_size = get_max_uniform_name_length();
  name.resize(buffer_size);
  GLsizei written;
  GLint size;
  GLenum type;
  gpu(glGetActiveUniform(_name, uniform_index, buffer_size, &written, &size, &type, &name[0]));
  name.resize(written);

  GLint offset = get_block_uniform_offset(uniform_index);
  GLint block_index = get_uniform_block_index_for_block_uniform(uniform_index);

  // fill in the uniform_type
  BlockUniformInfo* info = new_ff BlockUniformInfo();
  info->set_name(name);
  info->set_type(type);
  info->set_num_elements(size);
  info->set_index(uniform_index);
  info->set_block_index(block_index);
  info->set_offset_bytes(offset);
  return info;
}

GLint Program::get_uniform_block_index_for_block_uniform(GLuint uniform_index) {
  GLint block_index;
  gpu(glGetActiveUniformsiv(_name,1,&uniform_index,GL_UNIFORM_BLOCK_INDEX,&block_index));
  return block_index;
}

#endif



#if GLES_MAJOR_VERSION >= 3
std::vector<BlockUniformInfo*> Program::create_block_uniform_infos(const char* name) {
  GLuint block_index = get_uniform_block_index(name);
  return create_block_uniform_infos(block_index);
}

std::vector<BlockUniformInfo*> Program::create_block_uniform_infos(GLuint block_index) {
  std::vector<GLint> indices = get_block_uniform_indices(block_index);
  size_t num = indices.size();
  std::vector<BlockUniformInfo*> infos;
  infos.reserve(num);
  for (size_t i = 0; i < num; i++) {
    BlockUniformInfo* info = create_block_uniform_info(indices[i]);
    infos.push_back(info);
  }
  return infos;
}

std::vector<BlockUniformInfo*> Program::create_block_uniform_infos() {
  std::vector<GLuint> indices = get_block_uniform_indices();
  std::vector<BlockUniformInfo*> infos;
  infos.reserve(indices.size());
  for (size_t i = 0; i < indices.size(); i++) {
    BlockUniformInfo* info = create_block_uniform_info(indices[i]);
    infos.push_back(info);
  }
  return infos;
}
#endif



// --------------------------------------------------------------------------------------------------------------
// Uniform Block Bindings.
// --------------------------------------------------------------------------------------------------------------

#if GLES_MAJOR_VERSION >= 3
GLint Program::get_max_uniform_block_binding() {
  GLint max;
  gpu(glGetProgramiv(_name,GL_MAX_UNIFORM_BUFFER_BINDINGS,&max));
  return max;
}
void Program::set_uniform_block_binding(GLuint block_index,
                                        GLuint uniform_block_binding) {
  gpu(glUniformBlockBinding(_name,block_index,uniform_block_binding));
}
#endif

// --------------------------------------------------------------------------------------------------------------
// Internals.
// --------------------------------------------------------------------------------------------------------------


}
