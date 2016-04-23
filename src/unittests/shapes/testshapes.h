#pragma once
#include <base/device/transforms/glmhelper.h>


namespace ngs {

class SystemGeo;
class DeviceGeo;
struct GeoRange;
struct BaseVertexAttrib;

class FontPack;
class Resources;

class TestShapes {
 public:
  TestShapes();
  ~TestShapes();

  void set_up();
  void test_adding_elements();
  void test_updating_elements();
  void test_offsetting_elements();
  void test_resizing_elements();
  void test_destroying_elements();

 private:

  void check_element_a(unsigned int index_offset, const glm::vec2& vertex_offset);
  void check_element_b(unsigned int index_offset, const glm::vec2& vertex_offset);
  void check_element_c(unsigned int index_offset, const glm::vec2& vertex_offset);

  Resources *_resources;
  FontPack *_font_pack;

  SystemGeo* _system_geometry;
  DeviceGeo* _device_geometry;

  GeoRange* _element_a;
  GeoRange* _element_b;
  GeoRange* _element_c;

  static const int _id_a;
  static const int _id_b;
  static const int _id_c;

  static const size_t _num_indices_a;
  static const size_t _num_vertices_a;

  static const size_t _num_indices_b;
  static const size_t _num_vertices_b;

  static const size_t _num_indices_c;
  static const size_t _num_vertices_c;

  static const size_t _sum_indices_1;
  static const size_t _sum_indices_2;
  static const size_t _sum_indices_3;

  static const size_t _sum_vertices_1;
  static const size_t _sum_vertices_2;
  static const size_t _sum_vertices_3;


};

}
