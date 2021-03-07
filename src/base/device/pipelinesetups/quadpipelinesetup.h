#pragma once
#include <base/device/device_export.h>
#include <base/device/pipelinesetups/pipelinesetup.h>
#include <base/device/geometry/instancevertexattribute.h>

#include <vector>

namespace ngs {

class DefaultUniformInfo;
class Resources;

class Quad;
class Triangle;
class Circle;

struct PosTexVertex;
struct ShapeInstance;

class DEVICE_EXPORT QuadPipelineSetup: public PipelineSetup {
 public:

  static const glm::vec4 kClearColor;

  QuadPipelineSetup(const std::string& vertex_shader, const std::string& fragment_shader);
  ~QuadPipelineSetup();

  // Uniforms.
  void set_mvp(const glm::mat4& m);
  void set_selected_mvp(const glm::mat4& m);

  // Draw.
  virtual bool has_instances() const;
  virtual void draw();
  virtual void draw_garbage(); // used on macos to deal with artifacts when drawing an empty node graph

  // Quad pipeline specific methods.
  virtual void load_quad_instances(const std::vector<ShapeInstance>& instances);
  virtual void load_tri_instances(const std::vector<ShapeInstance>& instances);
  virtual void load_circle_instances(const std::vector<ShapeInstance>& instances);

 protected:
  virtual void initialize_gl_resources();

 private:
  // Load garbage quads. This is used to deal with macos artifacts with empty node graphs.
  virtual void load_garbage_quad_instances();

  // Configure our pipeline's attribute locations.
  virtual void feed_elements_to_attrs();

  // Our uniform infos.
  const DefaultUniformInfo* _mvp_uniform;
  const DefaultUniformInfo* _selected_mvp_uniform;

  // Our geometry.
  Quad* _quad;
  Triangle* _tri;
  Circle* _circle;

  // Our instances.
  VertexBuffer* _quads;
  size_t _num_quads;
  VertexBuffer* _tris;
  size_t _num_tris;
  VertexBuffer* _circles;
  size_t _num_circles;
};


}
