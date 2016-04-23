#pragma once

#include <base/device/packedbuffers/elementid.h>
#include <string>

#include <base/device/transforms/glmhelper.h>

namespace ngs {

/*
 This class tests the gpu pipeline class using a very simple setup.
 The vertex shader simply does model view project on the vertices.
 The fragment shader samples an input texture, tints it, and sends it to the output texture.
 These tests are templated on the texture element type.
 Both input and output textures are templated on the same type in these tests.
 Although in the real world they likely won't.

 Note we have three fragment shaders we test with.
 normalized_texture_value.f
 int_texture_value.f
 uint_texture_value.f

 The normalized version uses a sampler which returns a normalized value.
 The int version uses a sampler which returns a integer value.
 The uint version uses a sampler which returns a unsigned interger value.
 Note the glsl shader is able to use these samplers because an appropriate unpacked_format
 was used when creating the texture.
 */

class Texture;
class PackedTexture;

class TestTexture2 {
 public:
	TestTexture2(ElementID element_id, bool normalized_access);
  virtual ~TestTexture2();

 private:
  void create_packed_texture();
  void create_texture();
  void create_read_back_packed_texture();
  void check_packed_chunk_equality_operator();

  // Determine whether we'll be testing with textures which yield normalized values or not.
  bool _normalized_access;
  ElementID _element_id;

  PackedTexture* _packed_texture;
  Texture* _texture;
  PackedTexture* _read_back_packed_texture;
};

}
