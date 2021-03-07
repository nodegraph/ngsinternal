#pragma once
#include <base/device/device_export.h>
#include <base/device/pipelinesetups/pipelinesetup.h>

#include <vector>

namespace ftgl {
  struct texture_font_t;
  struct texture_atlas_t;
}

namespace ngs {

class DefaultUniformInfo;
class Resources;

//template<class A>
//class DistFieldText;

class Quad;
//struct PosTexVertex;
//struct ShapeInstance;

class DEVICE_EXPORT TextPipelineSetup: public PipelineSetup {
 public:

  static const glm::vec4 kClearColor;

  TextPipelineSetup(const std::string& vertex_shader, const std::string& fragment_shader,
                    const std::string& font_data, const unsigned char* distance_map, size_t distance_map_size);
  ~TextPipelineSetup();

  // Uniforms.
  void set_mvp(const glm::mat4& m);
  void set_selected_mvp(const glm::mat4& m);

  // Draw.
  virtual void draw();

  virtual void load_char_instances(const std::vector<CharInstance>& instances);

 protected:
  virtual void initialize_gl_resources();

 private:
  // Configure our pipeline's attribute locations.
  virtual void feed_elements_to_attrs();

  // Uniforms.
  void set_distance_texture_unit(int texture_unit);

  // Our uniform infos. These are borrowed references.
  const DefaultUniformInfo* _mvp_uniform;
  const DefaultUniformInfo* _selected_mvp_uniform;
  const DefaultUniformInfo* _distance_texture_uniform;

  // Our text geometry.
  Quad* _quad;

  // Our instances.
  VertexBuffer* _instances_vbo;
  size_t _num_instances;

  // Our text font and it's texture atlas.
  ftgl::texture_font_t *_font;
  ftgl::texture_atlas_t *_atlas;

  const unsigned char* _distance_map;
  size_t _distance_map_size;

};


}
