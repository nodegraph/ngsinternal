#include <base/device/packedbuffers/packedvertexarray.h>


namespace ngs {

#if GLES_MAJOR_VERSION >= 100

PackedVertexArray::PackedVertexArray() {
  _tex_unit[0] = GL_TEXTURE0;
  _tex_unit[1] = GL_TEXTURE1;
  _tex_unit[2] = GL_TEXTURE2;
}

void PackedVertexArray::bind() {
  // Disable any bound vertex buffer objects.
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Disable any bound index buffer objects.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(VertexAttributes), &_vertex[0].x);

  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(VertexAttributes), &_vertex[0].nx);

  glClientActiveTexture(_tex_unit[0]);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttributes), &_vertex[0].s0);

  glClientActiveTexture(_tex_unit[1]);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttributes), &_vertex[0].s1);

  glClientActiveTexture(_tex_unit[2]);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttributes), &_vertex[0].s2);
}

void PackedVertexArray::draw() {
  glDrawRangeElements(GL_TRIANGLES, 0, _vertex.size() - 1, _index.size(),
                      GL_UNSIGNED_SHORT, &_index[0]);
}

#endif

}
