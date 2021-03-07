#include <base/device/deviceheadersgl.h>
#include <base/device/program/attributeinfos.h>
#include <base/device/program/pipeline.h>
#include <base/device/program/program.h>
#include <base/device/program/defaultuniforminfos.h>
#include <base/device/program/fragdatainfos.h>
#include <base/device/program/shader.h>
#include <base/device/program/blockuniforminfos.h>

#include <base/memoryallocator/taggednew.h>
#include <base/utils/fileutil.h>

//#include <QtCore/QDebug>

namespace ngs {

Pipeline::Pipeline(
    const std::string& vertex_source,
    const std::string& fragment_source,
    const std::string& geometry_source) {

  // Create program.
	_program = new_ff Program();

  const char* temp = vertex_source.c_str();
  Shader vertex_shader(GL_VERTEX_SHADER, 1, &temp);
  _program->attach_shader(vertex_shader);

  const char* temp2 = fragment_source.c_str();
  Shader fragment_shader(GL_FRAGMENT_SHADER, 1, &temp2);
  _program->attach_shader(fragment_shader);

	// Find and attach geometry shader.
#if GLES_MAJOR_VERSION >= 100
		if (!geometry_source.empty()) {
      const char* temp3 = geometry_source.c_str();
      Shader geometry_shader(GL_GEOMETRY_SHADER, 1, &temp3);
      _program->attach_shader(geometry_shader);
		}
#endif

	// Link program.
	_program->link();
	_program->use();

	// Initialize the various info objects.
	_attribute_infos = new_ff AttributeInfos(*_program);
	_default_uniform_infos = new_ff DefaultUniformInfos(*_program);
	_frag_data_infos = new_ff FragDataInfos(*_program, fragment_source);
#if GLES_MAJOR_VERSION >= 3
	GLint num_blocks = _program->get_num_uniform_blocks();
	for (GLint i=0; i<num_blocks; ++i) {
	  _block_uniform_infos.push_back(new_ff BlockUniformInfos(*_program,i));
	}
#endif
}

Pipeline::~Pipeline() {

#if GLES_MAJOR_VERSION >= 3
	for (BlockUniformInfos* bui: _block_uniform_infos) {
	  delete_ff(bui);
	}
	_block_uniform_infos.clear();
#endif

  delete_ff(_attribute_infos);
  delete_ff(_default_uniform_infos);
  delete_ff(_frag_data_infos);
	delete_ff(_program);  // delete_ff program last as others have references to this.
}

void Pipeline::use() {
	_program->use();
}

void Pipeline::disable() {
	_program->disable();
}

}
