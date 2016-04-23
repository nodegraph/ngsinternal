#include <base/device/unpackedbuffers/defaultunpacking.h>
#include <base/device/deviceheadersgl.h>

#include <cstring>
#include <iostream>
#include <cassert>

/*
For gles 2.0, we're only supporting the unsigned char type, because in gles 1.0
the required types of the implementation are
GL_UNSIGNED_BYTE per channel = num_channel bytes.
GL_UNSIGNED_SHORT_4_4_4_4 = 2 bytes
GL_UNSIGNED_SHORT_5_5_5_4 = 2 bytes
GL_UNSIGNED_SHORT_5_6_5 = 2 bytes
*/

namespace ngs {

// -----------------------------------------------------------------------------
// Unpacking for unsigned char types.
// -----------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<unsigned char>::get_default_packed_type() {
  return GL_UNSIGNED_BYTE;
}

template<>
GLenum DefaultUnpacking<unsigned char>::get_default_packed_format(int num_channels,
                                                      bool normalized_access) {
  // In GLES 2.0, the access is always normalized.
  switch (num_channels) {
    case 1: {
      return GL_LUMINANCE;
    }
      break;
    case 2: {
      return GL_LUMINANCE_ALPHA;
    }
      break;
    case 3: {
      return GL_RGB;
    }
      break;
    case 4: {
      return GL_RGBA;
    }
      break;
    default: {
      std::cerr
          << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
      return 0;
    }
      break;
  }
}

template<>
GLenum DefaultUnpacking<unsigned char>::get_default_unpacked_target(int num_dimensions) {
  if (num_dimensions == 1) {
    //return GL_TEXTURE_1D;
    assert(false);
  } else if (num_dimensions == 2) {
    return GL_TEXTURE_2D;
    // this could also be a 1d texture array.
  } else if (num_dimensions == 3) {
    //return GL_TEXTURE_2D_ARRAY;
    assert(false);
    // Note this could also be a :
    // GL_TEXTURE_CUBE_MAP or
    // GL_TEXTURE_3D.
    // However these are not the defaults and so must be explicity set.
  }
  return 0;
}

template<>
GLenum DefaultUnpacking<unsigned char>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  // In GLES 2.0 access is always normalized.
  switch (num_channels) {
    case 1: {
      return GL_LUMINANCE; // You will likely not be able to render to this.
    }
      break;
    case 2: {
      return GL_LUMINANCE_ALPHA; // You will likely not be able to render to this.
    }
      break;
    case 3: {
      return GL_RGB; //GL_RGB8_OES; //GL_RGB;
    }
      break;
    case 4: {
      return GL_RGBA; //GL_RGBA8_OES; //GL_RGBA;
    }
      break;
    default: {
      std::cerr
          << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
      return 0;
    }
      break;
  }
}

// -----------------------------------------------------------------------------
// Unpacking for char types.
// -----------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<char>::get_default_packed_type() {
  return GL_BYTE;
}

template<>
GLenum DefaultUnpacking<char>::get_default_packed_format(int num_channels,
                                                      bool normalized_access) {
  // In GLES 2.0, the access is always normalized.
  switch (num_channels) {
    case 1: {
      return GL_LUMINANCE;
    }
      break;
    case 2: {
      return GL_LUMINANCE_ALPHA;
    }
      break;
    case 3: {
      return GL_RGB;
    }
      break;
    case 4: {
      return GL_RGBA;
    }
      break;
    default: {
      std::cerr
          << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
      return 0;
    }
      break;
  }
}

template<>
GLenum DefaultUnpacking<char>::get_default_unpacked_target(int num_dimensions) {
  if (num_dimensions == 1) {
    //return GL_TEXTURE_1D;
    assert(false);
  } else if (num_dimensions == 2) {
    return GL_TEXTURE_2D;
    // this could also be a 1d texture array.
  } else if (num_dimensions == 3) {
    //return GL_TEXTURE_2D_ARRAY;
    assert(false);
    // Note this could also be a :
    // GL_TEXTURE_CUBE_MAP or
    // GL_TEXTURE_3D.
    // However these are not the defaults and so must be explicity set.
  }
  return 0;
}

template<>
GLenum DefaultUnpacking<char>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  // In GLES 2.0 access is always normalized.
  switch (num_channels) {
    case 1: {
      return GL_LUMINANCE;
    }
      break;
    case 2: {
      return GL_LUMINANCE_ALPHA;
    }
      break;
    case 3: {
      return GL_RGB;
    }
      break;
    case 4: {
      return GL_RGBA;
    }
      break;
    default: {
      std::cerr
          << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
      return 0;
    }
      break;
  }
}

// -----------------------------------------------------------------------------
// Unpacking for other types will lead to seg faults through asserts.
// -----------------------------------------------------------------------------

template<class T>
GLenum DefaultUnpacking<T>::get_default_packed_type() {
  assert(false);
  return GL_UNSIGNED_BYTE;
}

template<class T>
GLenum DefaultUnpacking<T>::get_default_packed_format(int num_channels,
                                                      bool normalized_access) {
  assert(false);
  return GL_LUMINANCE;
}

template<class T>
GLenum DefaultUnpacking<T>::get_default_unpacked_target(int num_dimensions) {
  assert(false);
  return GL_TEXTURE_2D;
}

template<class T>
GLenum DefaultUnpacking<T>::get_default_unpacked_format(int num_channels, bool normalized_access) {
  assert(false);
  return GL_LUMINANCE;
}

// Template Instantiations.
template class DefaultUnpacking<float> ;
template class DefaultUnpacking<half> ;
template class DefaultUnpacking<int> ;
template class DefaultUnpacking<unsigned int> ;
template class DefaultUnpacking<char> ;
template class DefaultUnpacking<unsigned char> ;
template class DefaultUnpacking<short> ;
template class DefaultUnpacking<unsigned short> ;

}
