#include <base/device/pipelinesetups/quadpipelinesetup.h>
#include <base/memoryallocator/taggednew.h>

#include <base/device/program/defaultuniforminfos.h>
#include <base/device/program/attributeinfos.h>
#include <base/device/geometry/quad.h>
#include <base/device/geometry/triangle.h>
#include <base/device/geometry/circle.h>

//#include <base/shapes/systembuffers/vertexattributes.h>

#include <components/resources/resources.h>

#if ARCH == ARCH_ANDROID
#include <android/log.h>
#endif

namespace ngs {

const glm::vec4 QuadPipelineSetup::kClearColor(3.0f/255.0f, 169.0f/255.0f, 244.0f/255.0f, 1.00 );

QuadPipelineSetup::QuadPipelineSetup(const std::string& vertex_shader, const std::string& fragment_shader)
    : PipelineSetup(vertex_shader, fragment_shader),
      _mvp_uniform(NULL),
      _selected_mvp_uniform(NULL),
      _quad(NULL),
      _tri(NULL),
      _circle(NULL),
      _quads(NULL),
      _num_quads(0),
      _tris(NULL),
      _num_tris(0),
      _circles(NULL),
      _num_circles(0){
}

QuadPipelineSetup::~QuadPipelineSetup() {
  delete_ff(_quad);
  delete_ff(_tri);
  delete_ff(_circle);
  delete_ff(_quads);
  delete_ff(_tris);
  delete_ff(_circles);
}

void QuadPipelineSetup::initialize_gl_resources() {
  assert(_mvp_uniform == NULL);

  _mvp_uniform = _default_uniform_infos->get_uniform_info("model_view_project");
  _selected_mvp_uniform = _default_uniform_infos->get_uniform_info("model_view_project_selected");
  _quad = new_ff Quad();
  _tri = new_ff Triangle();
  _circle = new_ff Circle();
  _quads = new_ff VertexBuffer();
  _num_quads = 0;
  _tris = new_ff VertexBuffer();
  _num_tris = 0;
  _circles = new_ff VertexBuffer();
  _num_circles = 0;

  feed_elements_to_attrs();
}

void QuadPipelineSetup::set_mvp(const glm::mat4& m) {
  _default_uniform_infos->set_uniform(_mvp_uniform, &m[0][0]);
}

void QuadPipelineSetup::set_selected_mvp(const glm::mat4& m) {
  _default_uniform_infos->set_uniform(_selected_mvp_uniform, &m[0][0]);
}

void QuadPipelineSetup::feed_elements_to_attrs() {
  GLuint attr_loc = -1;
  use();
  {
    _quad->bind();
    {
      attr_loc = _attribute_infos->get_attribute_location("vertex");
      _quad->feed_positions_to_attr(attr_loc);
    }
    _quads->bind();
    {
      attr_loc = _attribute_infos->get_attribute_location("scale");
      _quads->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::scale_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("rotate");
      _quads->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::rotation_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("translate");
      _quads->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::translation_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("color");
      _quads->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::color_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("state");
      _quads->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::state_source, attr_loc);
    }
  }
  {
  _tri->bind();
    {
      attr_loc = _attribute_infos->get_attribute_location("vertex");
      _tri->feed_positions_to_attr(attr_loc);
    }
    _tris->bind();
    {
      attr_loc = _attribute_infos->get_attribute_location("scale");
      _tris->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::scale_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("rotate");
      _tris->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::rotation_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("translate");
      _tris->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::translation_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("color");
      _tris->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::color_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("state");
      _tris->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::state_source, attr_loc);
    }
  }
  {
    _circle->bind();
    {
      attr_loc = _attribute_infos->get_attribute_location("vertex");
      _circle->feed_positions_to_attr(attr_loc);
    }
    _circles->bind();
    {
      attr_loc = _attribute_infos->get_attribute_location("scale");
      _circles->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::scale_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("rotate");
      _circles->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::rotation_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("translate");
      _circles->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::translation_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("color");
      _circles->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::color_source, attr_loc);
      attr_loc = _attribute_infos->get_attribute_location("state");
      _circles->set_elements_to_feed_to_float_vertex_attribute(ShapeInstance::state_source, attr_loc);
    }
  }
}

void QuadPipelineSetup::draw() {
  _quad->bind();
  _quads->bind();
  _quads->bind_positions();
  _quad->draw_instances(_num_quads);

  _tri->bind();
  _tris->bind();
  _tris->bind_positions();
  _tri->draw_instances(_num_tris);

  _circle->bind();
  _circles->bind();
  _circles->bind_positions();
  _circle->draw_instances(_num_circles);

  _quad->unbind();
  _quads->unbind();

  _tri->unbind();
  _tris->unbind();

  _circle->unbind();
  _circles->unbind();
}

void QuadPipelineSetup::load_quad_instances(const std::vector<ShapeInstance>& instances) {
  //std::cerr << "the number of quad instances is: " << instances.size() << "\n";
  _num_quads = instances.size();
  if (_num_quads) {
    _quads->bind();
    _quads->load(sizeof(ShapeInstance)*_num_quads, &instances[0]);
  }
}

void QuadPipelineSetup::load_tri_instances(const std::vector<ShapeInstance>& instances) {
  //std::cerr << "the number of tri instances is: " << instances.size() << "\n";
  _num_tris = instances.size();
  if (_num_tris) {
    _tris->bind();
    _tris->load(sizeof(ShapeInstance)*_num_tris, &instances[0]);
  }
}

void QuadPipelineSetup::load_circle_instances(const std::vector<ShapeInstance>& instances) {
  //std::cerr << "the number of circle instances is: " << instances.size() << "\n";
  _num_circles = instances.size();
  if (_num_circles) {
    _circles->bind();
    _circles->load(sizeof(ShapeInstance)*_num_circles, &instances[0]);
  }
}

}
