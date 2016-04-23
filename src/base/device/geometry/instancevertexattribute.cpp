#include <base/device/deviceheadersgl.h>
#include <base/device/geometry/instancevertexattribute.h>
#include <base/device/unpackedbuffers/vertexbuffer.h>

namespace ngs {

const ElementSource ShapeInstance::scale_source = { 2, GL_FLOAT, false, sizeof(ShapeInstance), (void*) (0), 1};
const ElementSource ShapeInstance::rotation_source = { 1, GL_FLOAT, false, sizeof(ShapeInstance), (void*) (2 * sizeof(float)), 1};
const ElementSource ShapeInstance::translation_source = { 3, GL_FLOAT, false, sizeof(ShapeInstance), (void*) (3 * sizeof(float)), 1};
const ElementSource ShapeInstance::color_source = { 3, GL_UNSIGNED_BYTE, true, sizeof(ShapeInstance), (void*) (6 * sizeof(float)), 1};
const ElementSource ShapeInstance::state_source = { 1, GL_UNSIGNED_BYTE, false, sizeof(ShapeInstance), (void*) (6 * sizeof(float) + 3 * sizeof(unsigned char)), 1};

void ShapeInstance::set_scale(const glm::vec2& s) {
  scale[0] = s.x;
  scale[1] = s.y;
}

void ShapeInstance::set_scale(float sx, float sy) {
  scale[0] = sx;
  scale[1] = sy;
}

void ShapeInstance::set_rotate(float r) {
  rotate = r;
}

void ShapeInstance::set_translate(const glm::vec2& t, float z) {
  translate[0] = t.x;
  translate[1] = t.y;
  translate[2] = z;
}

void ShapeInstance::set_translate(float tx, float ty, float tz) {
  translate[0] = tx;
  translate[1] = ty;
  translate[2] = tz;
}

void ShapeInstance::set_color(const std::array<unsigned char,4>& c) {
  color[0] = c[0];
  color[1] = c[1];
  color[2] = c[2];
}

void ShapeInstance::set_color(unsigned char r, unsigned char g, unsigned char b) {
  color[0] = r;
  color[1] = g;
  color[2] = b;
}

void ShapeInstance::set_state(unsigned char s) {
  state = s;
}



const ElementSource CharInstance::scale_source = { 2, GL_FLOAT, false, sizeof(CharInstance), (void*) (0), 1 };
const ElementSource CharInstance::translation1_source = { 2, GL_FLOAT, false, sizeof(CharInstance), (void*) (2 * sizeof(float)), 1 };
const ElementSource CharInstance::rotation_source = { 1, GL_FLOAT, false, sizeof(CharInstance), (void*) (4 * sizeof(float)), 1 };
const ElementSource CharInstance::translation2_source = { 2, GL_FLOAT, false, sizeof(CharInstance), (void*) (5 * sizeof(float)), 1 };
const ElementSource CharInstance::coord_s_source = { 2, GL_FLOAT, false, sizeof(CharInstance), (void*) (7 * sizeof(float)), 1 };
const ElementSource CharInstance::coord_t_source = { 2, GL_FLOAT, false, sizeof(CharInstance), (void*) (9 * sizeof(float)), 1 };
const ElementSource CharInstance::state_source = { 1, GL_UNSIGNED_BYTE, false, sizeof(CharInstance), (void*) (11 * sizeof(float)), 1 };


void CharInstance::set_scale(float sx, float sy) {
  scale[0] = sx;
  scale[1] = sy;
}

void CharInstance::set_translate1(float x, float y) {
  translate1[0] = x;
  translate1[1] = y;
}

void CharInstance::set_translate1(const glm::vec2& t1) {
  translate1[0] = t1.x;
  translate1[1] = t1.y;
}

void CharInstance::set_rotate(float r) {
  rotate = r;
}

void CharInstance::set_translate2(float x, float y) {
  translate2[0] = x;
  translate2[1] = y;
}

void CharInstance::set_translate2(const glm::vec2& t2) {
  translate2[0] = t2.x;
  translate2[1] = t2.y;
}

void CharInstance::set_coord_s(float min_s, float max_s) {
  coord_s[0] = min_s;
  coord_s[1] = max_s;
}

void CharInstance::set_coord_t(float min_t, float max_t) {
  coord_t[0] = min_t;
  coord_t[1] = max_t;
}

void CharInstance::set_state(unsigned char s) {
  state = s;
}

}
