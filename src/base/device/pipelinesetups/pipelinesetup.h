#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>

namespace ngs {

class Pipeline;
class FragDataInfos;
class AttributeInfos;
class DefaultUniformInfos;
class FragDataInfo;

class DeviceMemory;

class DEVICE_EXPORT PipelineSetup {
 public:

  static const glm::vec4 kClearColor;

  PipelineSetup(const std::string& vertex_shader, const std::string& fragment_shader);
  virtual ~PipelineSetup();

  // Setup.
  void initialize_gl();
  void set_clear_color(const glm::vec4& clear_color);
  void set_clear_depth(float clear_depth);

  // Clearing ops do not require the program to be in use.
  void clear_depth();
  void clear_color();

  // Get frag data info.
  const FragDataInfo* get_frag_data_info(const std::string& name) const;

  // -----------------------------------------------
  // The following require the program to be in use.
  // -----------------------------------------------

  // Begin using.
  void use();
  void disable();


  // Draw.
  virtual void draw() {}

protected:

  virtual void dump_matrices(const glm::mat4& model_view, const glm::mat4& projection);
  virtual void initialize_gl_resources() {}

  // Our pipeline.
  Pipeline* _pipeline;
  FragDataInfos* _frag_data_infos; //borrowed
  AttributeInfos* _attribute_infos;//borrowed
  DefaultUniformInfos* _default_uniform_infos;//borrowed

  // GLSL shaders borrowed from the Resources component.
  const std::string& _vertex_shader;
  const std::string& _fragment_shader;

  // Settings.
  glm::vec4 _clear_color;
  float _clear_depth;
};


}
