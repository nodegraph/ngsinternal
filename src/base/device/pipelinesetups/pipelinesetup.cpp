#include <base/memoryallocator/taggednew.h>

#include <base/device/devicedebug.h>
#include <base/device/deviceheadersgl.h>
#include <base/glewhelper/glewhelper.h>
#include <base/device/pipelinesetups/pipelinesetup.h>
#include <base/device/program/fragdatainfos.h>
#include <base/device/program/pipeline.h>
#include <base/utils/archdebug.h>

#include <iostream>

namespace ngs {

const glm::vec4 PipelineSetup::kClearColor(3.0f/255.0f, 169.0f/255.0f, 244.0f/255.0f, 1.00 );

PipelineSetup::PipelineSetup(const std::string& vertex_shader, const std::string& fragment_shader)
    : _pipeline(NULL),
      _frag_data_infos(NULL),
      _attribute_infos(NULL),
      _default_uniform_infos(NULL),
      _vertex_shader(vertex_shader),
      _fragment_shader(fragment_shader),
      _clear_color(kClearColor),
      _clear_depth(1.0){
}

PipelineSetup::~PipelineSetup() {
  delete_ff(_pipeline);
}

// ------------------------------------------------------------------------------------------
// Device state and device memory.
// ------------------------------------------------------------------------------------------

void PipelineSetup::initialize_gl() {
  assert(_pipeline == NULL);
  _pipeline = new_ff Pipeline(_vertex_shader, _fragment_shader);
  _frag_data_infos = _pipeline->get_frag_data_infos();
  _attribute_infos = _pipeline->get_attribute_infos();
  _default_uniform_infos = _pipeline->get_default_uniform_infos();

  // We disable culling for now, to make sure we don't change the gl state too much.
  // This is because we have no logic to restore gl state, and we are rendering in Qt's render thread.

  // We cull all back facing polygons. This is mainly to make sure all our node graph vertices have the right winding order.
  // Back facing polygons are identified by counter clockwise winding order.
//  gpu(glEnable(GL_CULL_FACE));
//  gpu(glCullFace(GL_BACK));
//  gpu(glFrontFace(GL_CCW));

  // Update the pipeline uniforms.
  _pipeline->use();

  // Cache the uniform infos.
  initialize_gl_resources();

  // Disable all current texture targets.
  Device::disable_all_texture_targets();
}

void PipelineSetup::set_clear_color(const glm::vec4& clear_color) {
  _clear_color = clear_color;
}

void PipelineSetup::set_clear_depth(float clear_depth) {
  _clear_depth = clear_depth;
}

void PipelineSetup::clear_depth() {
  // Clear the screan.
  gpu(glClearDepthf(1.0));
  gpu(glEnable(GL_DEPTH_TEST));
  gpu(glClear(GL_DEPTH_BUFFER_BIT ));
}

void PipelineSetup::clear_color() {
  gpu(glClearColor( _clear_color.x, _clear_color.y, _clear_color.z, 1.00 ));
  gpu(glClear( GL_COLOR_BUFFER_BIT ));
}

const FragDataInfo* PipelineSetup::get_frag_data_info(const std::string& name) const{
  return _frag_data_infos->get_frag_data_info(name);
}

void PipelineSetup::use() {
  _pipeline->use();
}

void PipelineSetup::disable() {
  _pipeline->disable();
}

void PipelineSetup::dump_matrices(const glm::mat4& model_view, const glm::mat4& projection) {
    std::cerr << "model_view: ";

    for (size_t row=0; row<4; ++row) {
      for (size_t col=0; col<4; ++col) {
        std::cerr << model_view[row][col] << " ";
      }
      std::cerr << "\n";
    }

    std::cerr << "projection: ";

    for (size_t row=0; row<4; ++row) {
      for (size_t col=0; col<4; ++col) {
        std::cerr << projection[row][col] << " ";
      }
      std::cerr << "\n";
    }
}

}
