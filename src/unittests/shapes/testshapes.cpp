#include "testshapes.h"
#include <base/shapes/systembuffers/georange.h>
#include <base/shapes/systembuffers/systemgeo.h>
#include <base/shapes/devicebuffers/devicegeo.h>
#include <base/shapes/systembuffers/fontpack.h>
#include <base/memoryallocator/taggednew.h>
#include <cstddef>
#include <cassert>
#include <iostream>
#include <base/shapes/systembuffers/vertexattributes.h>
#include <base/utils/fileutil.h>
#include <base/utils/permit.h>
#include <FTGL/ftgl.h>
#include <components/resources/resources.h>

namespace ngs {

const int TestShapes::_id_a = 0;
const int TestShapes::_id_b = 1;
const int TestShapes::_id_c = 2;

#define default_color 0,0,0,0

const size_t TestShapes::_num_indices_a = 5;
const size_t TestShapes::_num_vertices_a = 3;

const size_t TestShapes::_num_indices_b = 9;
const size_t TestShapes::_num_vertices_b = 7;

const size_t TestShapes::_num_indices_c = 2;
const size_t TestShapes::_num_vertices_c = 4;

const size_t TestShapes::_sum_indices_1 = TestShapes::_num_indices_a;
const size_t TestShapes::_sum_indices_2 = TestShapes::_num_indices_a+TestShapes::_num_indices_b;
const size_t TestShapes::_sum_indices_3 = TestShapes::_num_indices_a+TestShapes::_num_indices_b+TestShapes::_num_indices_c;

const size_t TestShapes::_sum_vertices_1 = TestShapes::_num_vertices_a;
const size_t TestShapes::_sum_vertices_2 = TestShapes::_num_vertices_a+TestShapes::_num_vertices_b;
const size_t TestShapes::_sum_vertices_3 = TestShapes::_num_vertices_a+TestShapes::_num_vertices_b+TestShapes::_num_vertices_c;

TestShapes::TestShapes():
  _system_geometry(NULL),
  _element_a(NULL),
  _element_b(NULL),
  _element_c(NULL){



  set_up();
  test_adding_elements();
  test_updating_elements();
  test_offsetting_elements();
  test_resizing_elements();
  test_destroying_elements();

  std::cerr << "Done testing shapes\n";
}

TestShapes::~TestShapes() {
  delete_ff(_system_geometry);
  delete_ff(_device_geometry);
  delete_ff(_font_pack);
  delete_ff(_resources);
}

void TestShapes::set_up() {
  _resources = new_ff Resources();
//  _resources->load();
//  assert(_resources->finished());

  _font_pack = new_ff FontPack(_resources->get_text_font());
  _system_geometry = new_ff SystemGeo(sizeof(ShapeVertexAttrib), _font_pack);
  _element_a = NULL;
  _element_b = NULL;
  _element_c = NULL;

  _device_geometry = new_ff DeviceGeo();
  _device_geometry->_num_indices_to_overallocate = 0;
  _device_geometry->_num_vertices_to_overallocate = 0;
}

void TestShapes::test_adding_elements() {

  // Check initial system state.
  {
    // Check system memory.
    assert(_system_geometry->_indices[kNodeGraphIndexSet].size()==0);
    assert(_system_geometry->_vertices.size()==0);
    assert(_system_geometry->_ranges.size()==0);
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(0,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(0,0)));
  }

  // Check initial device state.
  {
    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==0);
    assert(_device_geometry->_num_device_vertices==0);
  }

  // Allocate Element A.
  {
    _element_a = new_ff GeoRange(_system_geometry,_num_vertices_a,_num_indices_a);
    _element_a->add_to_index(kNodeGraphIndexSet);

    // Check system memory.
    assert(_system_geometry->_indices[kNodeGraphIndexSet].size()==_sum_indices_1);
    assert(_system_geometry->_vertices.size()==_sum_vertices_1);
    assert(_system_geometry->_ranges.size()==1);
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(0,_sum_indices_1)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(0,_sum_vertices_1)));

  }

  // Update Device.
  {
    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_1);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_1);
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_1,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_1,0)));
  }

  // Allocate Element B.
  {
    _element_b = new_ff GeoRange(_system_geometry,_num_vertices_b,_num_indices_b);
    _element_b->add_to_index(kNodeGraphIndexSet);

    // Check system memory.
    assert(_system_geometry->_indices[kNodeGraphIndexSet].size()==_sum_indices_2);
    assert(_system_geometry->_vertices.size()==_sum_vertices_2);
    assert(_system_geometry->_ranges.size()==2);
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_1,_sum_indices_2)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_1,_sum_vertices_2)));
  }

  // Update Device.
  {
    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_2);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_2);
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_2,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_2,0)));
  }

  // Allocate Element C.
  {
    _element_c = new_ff GeoRange(_system_geometry,_num_vertices_c,_num_indices_c);
    _element_c->add_to_index(kNodeGraphIndexSet);

    // Check system memory.
    assert(_system_geometry->_indices[kNodeGraphIndexSet].size()==_sum_indices_3);
    assert(_system_geometry->_vertices.size()==_sum_vertices_3);
    assert(_system_geometry->_ranges.size()==3);
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_2,_sum_indices_3)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_2,_sum_vertices_3)));
  }

  // Update Device.
  {
    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3);
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3,0)));
  }
}

void TestShapes::test_updating_elements() {
  Permit<SystemGeo> holder(_system_geometry);
  {
    GLuint* indices = _element_a->get_indices();
    ShapeVertexAttrib* vertices = reinterpret_cast<ShapeVertexAttrib*>(_element_a->get_vertices());

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;
    indices[4] = 4;

    // Id=0, color=(0,0,0,0)
    vertices[0].set(_id_a,0,1,2,default_color);
    vertices[1].set(_id_a,3,4,5,default_color);
    vertices[2].set(_id_a,6,7,8,default_color);

    check_element_a(0,glm::vec2(0,0));

    _system_geometry->dirty_local(holder,_element_a);

    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(0,_sum_indices_1)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(0,_sum_vertices_1)));

    _device_geometry->load_changed(_system_geometry);

    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3,0)));
  }
  {
    GLuint* indices = _element_b->get_indices();
    ShapeVertexAttrib* vertices = reinterpret_cast<ShapeVertexAttrib*>(_element_b->get_vertices());

    indices[0] = 5;
    indices[1] = 6;
    indices[2] = 7;
    indices[3] = 8;
    indices[4] = 9;
    indices[5] = 10;
    indices[6] = 11;
    indices[7] = 12;
    indices[8] = 13;

    // Id=1, color=(0,0,0,0)
    vertices[0].set(_id_b,9,10,11,default_color);
    vertices[1].set(_id_b,12,13,14,default_color);
    vertices[2].set(_id_b,15,16,17,default_color);
    vertices[3].set(_id_b,18,19,20,default_color);
    vertices[4].set(_id_b,21,22,23,default_color);
    vertices[5].set(_id_b,24,25,26,default_color);
    vertices[6].set(_id_b,27,28,29,default_color);

    check_element_b(0,glm::vec2(0,0));

    _system_geometry->dirty_local(holder,_element_b);

    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_1,_sum_indices_2)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_1,_sum_vertices_2)));

    _device_geometry->load_changed(_system_geometry);

    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3,0)));

  }
  {
    GLuint* indices = _element_c->get_indices();
    ShapeVertexAttrib* vertices = reinterpret_cast<ShapeVertexAttrib*>(_element_c->get_vertices());

    indices[0] = 14;
    indices[1] = 15;

    // Id=2, color=(0,0,0,0)
    vertices[0].set(_id_c,30,31,32,default_color);
    vertices[1].set(_id_c,33,34,35,default_color);
    vertices[2].set(_id_c,36,37,38,default_color);
    vertices[3].set(_id_c,39,40,41,default_color);

    check_element_c(0,glm::vec2(0,0));


    _system_geometry->dirty_local(holder,_element_c);

    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_2,_sum_indices_3)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_2,_sum_vertices_3)));

    _device_geometry->load_changed(_system_geometry);

    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3,0)));
  }
}

void TestShapes::test_offsetting_elements() {
  glm::vec2 offset(100,100);
  _element_a->translate(offset);
  _element_b->translate(offset);
  _element_c->translate(offset);

  check_element_a(0,offset);
  check_element_b(0,offset);
  check_element_c(0,offset);

  assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(0,_sum_indices_3)));
  assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(0,_sum_vertices_3)));

  _device_geometry->load_changed(_system_geometry);
}

void TestShapes::test_resizing_elements() {
  Permit<SystemGeo> holder(_system_geometry);
  glm::vec2 offset(100,100);

  // B's original size is: 7 indices and 9 vertices

  // Case 1: grow vertices, grow indices.
  {
    _system_geometry->resize(holder,_element_b, 9, 11);

    check_element_a(0,offset);
    check_element_b(0,offset); // here, b is still embedded in a bigger space.
    check_element_c(2,offset); // indices will shift by 2 as 2 extra vertices were added
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_a,_sum_indices_3+2)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_a,_sum_vertices_3+2)));

    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3+2); // exact size due to growth in indices and vertices
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3+2); // exact size due to growth in indices and vertices
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3+2,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3+2,0)));
  }


  // Case 2: grow vertices, shrink indices.
  {
    _system_geometry->resize(holder,_element_b, 10, 10);

    check_element_a(0,offset);
    check_element_b(0,offset); // here, b is still embedded in a bigger space.
    check_element_c(3,offset); // indices will shift by 3 as 3 extra vertices were added
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_a,_sum_indices_3+2-1)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_a,_sum_vertices_3+2+1)));

    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3+2-1);  // exact size due to growth in vertices
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3+2+1); // exact size due to growth in vertices
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3+2-1,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3+2+1,0)));
  }

  // Case 3: shrink vertices, shrink indices.
  {
    _system_geometry->resize(holder,_element_b, 9, 9);

    check_element_a(0,offset);
    check_element_b(0,offset); // here, b is still embedded in a bigger space.
    check_element_c(2,offset); // indices will shift by 2 as 2 extra vertices were added
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_a,_sum_indices_3+2-1-1)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_a,_sum_vertices_3+2+1-1)));

    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3+2-1); // not exact size due to shrinkage in both indices and vertices
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3+2+1); // not exact size due to shrinkage in both indices and vertices
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3+2-1-1,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3+2+1-1,0)));
  }

  // Case 4: shrink vertices, grow indices.
  {
    _system_geometry->resize(holder,_element_b, 8, 10);

    check_element_a(0,offset);
    check_element_b(0,offset); // here, b is still embedded in a bigger space.
    check_element_c(1,offset); // indices will shift by 1 as 1 extra vertices were added
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_a,_sum_indices_3+2-1-1+1)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_a,_sum_vertices_3+2+1-1-1)));

    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3+2-1); // still fits into existing space on device
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3+2+1); // still fits into existing space on device
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3+2-1-1+1,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3+2+1-1-1,0)));
  }

  // Restore everything back to intial state.
  {
    _system_geometry->resize(holder,_element_b, 7, 9);

    check_element_a(0,offset);
    check_element_b(0,offset); // here, b is still embedded in a bigger space.
    check_element_c(0,offset); // indices will shift by 0 as we return to initial size
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_a,_sum_indices_3+2-1-1+1-1)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_a,_sum_vertices_3+2+1-1-1-1)));

    _device_geometry->load_changed(_system_geometry);

    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3+2-1); // still fits into existing space on device
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3+2+1); // still fits into existing space on device
    // Check system stats.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_sum_indices_3+2-1-1+1-1,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_sum_vertices_3+2+1-1-1-1,0)));
  }

  // Decrease the device memory size to the initial state.
  {
    _device_geometry->load_all(_system_geometry);
  }
}

void TestShapes::test_destroying_elements() {
  glm::vec2 offset(100,100);

  {
    delete_ff(_element_b);
    assert(_system_geometry->_indices[kNodeGraphIndexSet].size()==_sum_indices_3 - _num_indices_b);
    assert(_system_geometry->_vertices.size()==_sum_vertices_3 - _num_vertices_b);
    assert(_system_geometry->_ranges.size()==2);

    check_element_a(0,offset);
    check_element_c((GLuint)-_num_vertices_b,offset);

    // Check changed range.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_a,_num_indices_a + _num_indices_c)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_a,_num_vertices_a + _num_vertices_c)));
    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3);

    // Update device.
    _device_geometry->load_changed(_system_geometry);

    // Check changed range.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_a + _num_indices_c,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_a + _num_vertices_c,0)));
    // Notice the gpu memory never decreases, unless you do a update_device_all().
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3);
  }

  {
    delete_ff(_element_a);
    assert(_system_geometry->_indices[kNodeGraphIndexSet].size()==_num_indices_c);
    assert(_system_geometry->_vertices.size()==_num_vertices_c);
    assert(_system_geometry->_ranges.size()==1);

    check_element_c((GLuint)-_num_vertices_a-_num_vertices_b,offset);

    // Check changed range.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(0,_num_indices_c)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(0,_num_vertices_c)));
    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3);

    // Update device.
    _device_geometry->load_changed(_system_geometry);

    // Check changed range.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(_num_indices_c,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(_num_vertices_c,0)));
    // Notice the gpu memory never decreases, unless you do a update_device_all().
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3);
  }

  {
    delete_ff(_element_c);
    assert(_system_geometry->_indices[kNodeGraphIndexSet].size()==0);
    assert(_system_geometry->_vertices.size()==0);
    assert(_system_geometry->_ranges.size()==0);

    // Check changed range.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(0,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(0,0)));
    // Check device stats.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3);

    // Update device.
    _device_geometry->load_changed(_system_geometry);

    // Check changed range.
    assert((_system_geometry->_dirty_index_bounds[kNodeGraphIndexSet] == std::pair<size_t,size_t>(0,0)));
    assert((_system_geometry->_dirty_vertex_bounds == std::pair<size_t,size_t>(0,0)));
    // Notice the gpu memory never decreases, unless you do a update_device_all().
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==_sum_indices_3);
    assert(_device_geometry->_num_device_vertices==_sum_vertices_3);
  }

  // Test updating everything on the device.
  {
    _device_geometry->load_all(_system_geometry);
    // Notice the gpu memory has decreased.
    assert(_device_geometry->_num_device_indices[kNodeGraphIndexSet]==0);
    assert(_device_geometry->_num_device_vertices==0);
  }
}

void TestShapes::check_element_a(GLuint index_offset, const glm::vec2& vertex_offset){
  Permit<SystemGeo> holder(_system_geometry);

  GLuint* indices = _element_a->get_indices();
  ShapeVertexAttrib* vertices = reinterpret_cast<ShapeVertexAttrib*>(_element_a->get_vertices());

  assert(indices[0] == 0+index_offset);
  assert(indices[1] == 1+index_offset);
  assert(indices[2] == 2+index_offset);
  assert(indices[3] == 3+index_offset);
  assert(indices[4] == 4+index_offset);

  assert(vertices[0] == ShapeVertexAttrib(_id_a,vertex_offset.x+0,vertex_offset.x+1,2,default_color));
  assert(vertices[1] == ShapeVertexAttrib(_id_a,vertex_offset.x+3,vertex_offset.x+4,5,default_color));
  assert(vertices[2] == ShapeVertexAttrib(_id_a,vertex_offset.x+6,vertex_offset.x+7,8,default_color));
}

void TestShapes::check_element_b(GLuint index_offset, const glm::vec2& vertex_offset){
  Permit<SystemGeo> holder(_system_geometry);

  GLuint* indices = _element_b->get_indices();
  ShapeVertexAttrib* vertices = reinterpret_cast<ShapeVertexAttrib*>(_element_b->get_vertices());

  assert(indices[0] == 5+index_offset);
  assert(indices[1] == 6+index_offset);
  assert(indices[2] == 7+index_offset);
  assert(indices[3] == 8+index_offset);
  assert(indices[4] == 9+index_offset);
  assert(indices[5] == 10+index_offset);
  assert(indices[6] == 11+index_offset);
  assert(indices[7] == 12+index_offset);
  assert(indices[8] == 13+index_offset);

  assert(vertices[0] == ShapeVertexAttrib(_id_b,vertex_offset.x+9,vertex_offset.x+10,11,default_color));
  assert(vertices[1] == ShapeVertexAttrib(_id_b,vertex_offset.x+12,vertex_offset.x+13,14,default_color));
  assert(vertices[2] == ShapeVertexAttrib(_id_b,vertex_offset.x+15,vertex_offset.x+16,17,default_color));
  assert(vertices[3] == ShapeVertexAttrib(_id_b,vertex_offset.x+18,vertex_offset.x+19,20,default_color));
  assert(vertices[4] == ShapeVertexAttrib(_id_b,vertex_offset.x+21,vertex_offset.x+22,23,default_color));
  assert(vertices[5] == ShapeVertexAttrib(_id_b,vertex_offset.x+24,vertex_offset.x+25,26,default_color));
  assert(vertices[6] == ShapeVertexAttrib(_id_b,vertex_offset.x+27,vertex_offset.x+28,29,default_color));
}

void TestShapes::check_element_c(GLuint index_offset, const glm::vec2& vertex_offset) {
  Permit<SystemGeo> holder(_system_geometry);

  GLuint* indices = _element_c->get_indices();
  ShapeVertexAttrib* vertices = reinterpret_cast<ShapeVertexAttrib*>(_element_c->get_vertices());

  assert(indices[0] == 14+index_offset);
  assert(indices[1] == 15+index_offset);

  assert(vertices[0] == ShapeVertexAttrib(_id_c,vertex_offset.x+30,vertex_offset.x+31,32,default_color));
  assert(vertices[1] == ShapeVertexAttrib(_id_c,vertex_offset.x+33,vertex_offset.x+34,35,default_color));
  assert(vertices[2] == ShapeVertexAttrib(_id_c,vertex_offset.x+36,vertex_offset.x+37,38,default_color));
  assert(vertices[3] == ShapeVertexAttrib(_id_c,vertex_offset.x+39,vertex_offset.x+40,41,default_color));
}

}
