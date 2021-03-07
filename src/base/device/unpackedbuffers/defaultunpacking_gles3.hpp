#include <base/device/unpackedbuffers/defaultunpacking.h>
#include <base/device/deviceheadersgl.h>

#include <cstring>
#include <iostream>
#include <cassert>

/*

 Well after careful reading of the OpenGL 4.2 specs,
 I found that the snorm textures are not part of the required framebuffer formats.
 Only the color formats for "textures and renderbuffers" are required.
 This doesn't include the snorm textures which are a part of the "texture-only" color formats.


 Quote from section 4.4.4 of the OpenGL 4.2 specs.

 Required Framebuffer Formats
 Implementations must support framebuffer objects with up to MAX_COLOR_-
 ATTACHMENTS color attachments, a depth attachment, and a stencil attachment.
 Each color attachment may be in any of the required color formats for textures
 and renderbuffers described in sections 3.9.3 and 4.4.2.

 Quote from section 3.9.3 of the OpenGL 4.2 specs.

 Texture and renderbuffer color formats (see section 4.4.2).
 � RGBA32F, RGBA32I, RGBA32UI, RGBA16, RGBA16F, RGBA16I,
 RGBA16UI, RGBA8, RGBA8I, RGBA8UI, SRGB8_ALPHA8, RGB10_A2,
 RGB10_A2UI, RGB5_A1, and RGBA4.
 OpenGL 4.2 (Core Profile) - August 8, 2011
 3.9. TEXTURING 213
 � R11F_G11F_B10F and RGB565.
 � RG32F, RG32I, RG32UI, RG16, RG16F, RG16I, RG16UI, RG8, RG8I,
 and RG8UI.
 � R32F, R32I, R32UI, R16F, R16I, R16UI, R16, R8, R8I, and R8UI.

 Texture-only color formats:
 � RGBA16_SNORM and RGBA8_SNORM.
 � RGB32F, RGB32I, and RGB32UI.
 � RGB16_SNORM, RGB16F, RGB16I, RGB16UI, and RGB16.
 � RGB8_SNORM, RGB8, RGB8I, RGB8UI, and SRGB8.
 � RGB9_E5.
 � RG16_SNORM, RG8_SNORM, COMPRESSED_RG_RGTC2 and
 COMPRESSED_SIGNED_RG_RGTC2.
 � R16_SNORM, R8_SNORM, COMPRESSED_RED_RGTC1 and
 COMPRESSED_SIGNED_RED_RGTC1.

 Hope this helps someone else too!

 */

namespace ngs {

template<class T>
GLenum DefaultUnpacking<T>::get_default_unpacked_target(int num_dimensions) {
  if (num_dimensions == 1) {
#if GLES_MAJOR_VERSION >= 100
    return GL_TEXTURE_1D;
#else
    assert(false);
#endif
  } else if (num_dimensions == 2) {
    return GL_TEXTURE_2D;
    // this could also be a 1d texture array.
  } else if (num_dimensions == 3) {
#if GLES_MAJOR_VERSION >= 3
    return GL_TEXTURE_2D_ARRAY;
#else
    assert(false);
#endif
    // Note this could also be a :
    // GL_TEXTURE_CUBE_MAP or
    // GL_TEXTURE_3D.
    // However these are not the defaults and so must be explicity set.
  }
  return 0;
}

/*
 If a component of a pixel is less than 4 bytes then we need to be careful of the component order.
 On little endian machines the least significant byte is stored first.
 On big endian machines the most significant byte is stored first.


 For example for a 4 bytes rgba pixel the following is how the memory is laid out.
 The pixel value as unsigned integer is equivalent to: (A << 24) | (R << 16) | (G << 8) | B

 Low memory address ----->  High memory address
 Little endian:   BGRA
 Big endian:      ARGB

 Note: Alpha is the most significant component.

 For floats and integer pixels we don't need to worry about this.
 */

// This is a general implementation for pixel which have component elements that
// are less than 4 bytes.  Below you'll find specializations pixel elements that are 4 bytes.
// This template covers the following cases.
// short,ushort,char,uchar.
template<class T>
GLenum DefaultUnpacking<T>::get_default_packed_format(int num_channels,
                                                      bool normalized_access) {
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_RED;
      }
        break;
      case 2: {
        return GL_RG;
      }
        break;
      case 3: {
#if GLES_MAJOR_VERSION >= 100
        return GL_BGR;
#else
        return GL_RGB;
#endif
      }
        break;
      case 4: {
#if GLES_MAJOR_VERSION >= 100
        return GL_BGRA;
#else
        return GL_RGBA;
#endif
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
    switch (num_channels) {
      case 1: {
        return GL_RED_INTEGER;
      }
        break;
      case 2: {
        return GL_RG_INTEGER;
      }
        break;
      case 3: {
#if GLES_MAJOR_VERSION >= 100
        return GL_BGR_INTEGER;
#else
        return GL_RGB_INTEGER;
#endif
      }
        break;
      case 4: {
#if GLES_MAJOR_VERSION >= 100
        return GL_BGRA_INTEGER;
#else
        return GL_RGBA_INTEGER;
#endif
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
}

// ------------------------------------------------------------------------------------------
// Template Specializations
// ------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Float - doesn't use the _normalize_data member.
// You should expliciting convert to integer if you don't wan't to see normalized values in glsl.
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<float>::get_default_packed_format(
    int num_channels, bool normalized_access) {
  // The normalized access parameter is ignored.
  // Floats are always unnormalized.
  switch (num_channels) {
    case 1: {
      return GL_RED;
    }
      break;
    case 2: {
      return GL_RG;
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
GLenum DefaultUnpacking<float>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  // The normalized access parameter is ignored.
  // Floats are always unnormalized.
  switch (num_channels) {
    case 1: {
      return GL_R32F;
    }
      break;
    case 2: {
      return GL_RG32F;
    }
      break;
    case 3: {
      return GL_RGB32F;
    }
      break;
    case 4: {
      return GL_RGBA32F;
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
GLenum DefaultUnpacking<float>::get_default_packed_type() {
  return GL_FLOAT;
}

// --------------------------------------------------------------------------------
// Half - doesn't use the _normalize_data member.
// You should expliciting convert to integer if you don't wan't to see normalized values in glsl.
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<half>::get_default_packed_format(
    int num_channels, bool normalized_access) {
  // The normalized access parameter is ignored.
  // Floats are always unnormalized.
  switch (num_channels) {
    case 1: {
      return GL_RED;
    }
      break;
    case 2: {
      return GL_RG;
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
GLenum DefaultUnpacking<half>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  // The normalized access parameter is ignored.
  // Floats are always unnormalized.
  switch (num_channels) {
    case 1: {
      return GL_R16F;
    }
      break;
    case 2: {
      return GL_RG16F;
    }
      break;
    case 3: {
      return GL_RGB16F;
    }
      break;
    case 4: {
      return GL_RGBA16F;
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
GLenum DefaultUnpacking<half>::get_default_packed_type() {
  return GL_HALF_FLOAT;
}

// --------------------------------------------------------------------------------
// Int - doesn't use the _normalize_data member.
// You should expliciting convert to float if you don't wan't to see unnormalized values in glsl.
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<int>::get_default_packed_format(
    int num_channels, bool normalized_access) {
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_RED_INTEGER;
      }
        break;
      case 2: {
        return GL_RG_INTEGER;
      }
        break;
      case 3: {
        return GL_RGB_INTEGER;
      }
        break;
      case 4: {
        return GL_RGBA_INTEGER;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
    switch (num_channels) {
      case 1: {
        return GL_RED_INTEGER;
      }
        break;
      case 2: {
        return GL_RG_INTEGER;
      }
        break;
      case 3: {
        return GL_RGB_INTEGER;
      }
        break;
      case 4: {
        return GL_RGBA_INTEGER;
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
}

template<>
GLenum DefaultUnpacking<int>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_R32I;
      }
        break;
      case 2: {
        return GL_RG32I;
      }
        break;
      case 3: {
        return GL_RGB32I;
      }
        break;
      case 4: {
        return GL_RGBA32I;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
    switch (num_channels) {
      case 1: {
        return GL_R32I;
      }
        break;
      case 2: {
        return GL_RG32I;
      }
        break;
      case 3: {
        return GL_RGB32I;
      }
        break;
      case 4: {
        return GL_RGBA32I;
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
}

template<>
GLenum DefaultUnpacking<int>::get_default_packed_type() {
  return GL_INT;
}

// --------------------------------------------------------------------------------
// Unsigned int - doesn't use the _normalize_data member.
// You should explicitly convert to float if you don't wan't to see unnormalized values in glsl.
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<unsigned int>::get_default_packed_format(
    int num_channels, bool normalized_access) {
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_RED_INTEGER;
      }
        break;
      case 2: {
        return GL_RG_INTEGER;
      }
        break;
      case 3: {
        return GL_RGB_INTEGER;
      }
        break;
      case 4: {
        return GL_RGBA_INTEGER;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
    switch (num_channels) {
      case 1: {
        return GL_RED_INTEGER;
      }
        break;
      case 2: {
        return GL_RG_INTEGER;
      }
        break;
      case 3: {
        return GL_RGB_INTEGER;
      }
        break;
      case 4: {
        return GL_RGBA_INTEGER;
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
}

template<>
GLenum DefaultUnpacking<unsigned int>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_R32UI;

      }
        break;
      case 2: {
        return GL_RG32UI;
      }
        break;
      case 3: {
        return GL_RGB32UI;
      }
        break;
      case 4: {
        return GL_RGBA32UI;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
    switch (num_channels) {
      case 1: {
        return GL_R32UI;

      }
        break;
      case 2: {
        return GL_RG32UI;
      }
        break;
      case 3: {
        return GL_RGB32UI;
      }
        break;
      case 4: {
        return GL_RGBA32UI;
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
}

template<>
GLenum DefaultUnpacking<unsigned int>::get_default_packed_type() {
  return GL_UNSIGNED_INT;
}

// --------------------------------------------------------------------------------
// short
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<short>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
#if GLES_MAJOR_VERSION >= 100
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_R16_SNORM;
      }
        break;
      case 2: {
        return GL_RG16_SNORM;
      }
        break;
      case 3: {
        return GL_RGB16_SNORM;
      }
        break;
      case 4: {
        return GL_RGBA16_SNORM;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
#endif
    switch (num_channels) {
      case 1: {
        return GL_R16I;
      }
        break;
      case 2: {
        return GL_RG16I;
      }
        break;
      case 3: {
        return GL_RGB16I;
      }
        break;
      case 4: {
        return GL_RGBA16I;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
#if GLES_MAJOR_VERSION >= 100
  }
#endif
}

template<>
GLenum DefaultUnpacking<short>::get_default_packed_type() {
  return GL_SHORT;
}

// --------------------------------------------------------------------------------
// unsigned short
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<unsigned short>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
#if GLES_MAJOR_VERSION >= 100
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_R16;
      }
        break;
      case 2: {
        return GL_RG16;
      }
        break;
      case 3: {
        return GL_RGB16;
      }
        break;
      case 4: {
        return GL_RGBA16;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
#endif
    switch (num_channels) {
      case 1: {
        return GL_R16UI;
      }
        break;
      case 2: {
        return GL_RG16UI;
      }
        break;
      case 3: {
        return GL_RGB16UI;
      }
        break;
      case 4: {
        return GL_RGBA16UI;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
#if GLES_MAJOR_VERSION >= 100
  }
#endif
}

template<>
GLenum DefaultUnpacking<unsigned short>::get_default_packed_type() {
  return GL_UNSIGNED_SHORT;
}

// --------------------------------------------------------------------------------
// char
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<char>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_R8_SNORM;
      }
        break;
      case 2: {
        return GL_RG8_SNORM;
      }
        break;
      case 3: {
        return GL_RGB8_SNORM;
      }
        break;
      case 4: {
        return GL_RGBA8_SNORM;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
    switch (num_channels) {
      case 1: {
        return GL_R8I;
      }
        break;
      case 2: {
        return GL_RG8I;
      }
        break;
      case 3: {
        return GL_RGB8I;
      }
        break;
      case 4: {
        return GL_RGBA8I;
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
}

template<>
GLenum DefaultUnpacking<char>::get_default_packed_type() {
  return GL_BYTE;
}

// --------------------------------------------------------------------------------
// unsigned char
// --------------------------------------------------------------------------------

template<>
GLenum DefaultUnpacking<unsigned char>::get_default_unpacked_format(
    int num_channels, bool normalized_access) {
  if (normalized_access) {
    switch (num_channels) {
      case 1: {
        return GL_R8;
      }
        break;
      case 2: {
        return GL_RG8;
      }
        break;
      case 3: {
        return GL_RGB8;
      }
        break;
      case 4: {
        return GL_RGBA8;
      }
        break;
      default: {
        std::cerr
            << "Error: DefaultUnpacking seems to have more than 4 channels.\n";
        return 0;
      }
        break;
    }
  } else {
    switch (num_channels) {
      case 1: {
        return GL_R8UI;
      }
        break;
      case 2: {
        return GL_RG8UI;
      }
        break;
      case 3: {
        return GL_RGB8UI;
      }
        break;
      case 4: {
        return GL_RGBA8UI;
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
}

template<>
GLenum DefaultUnpacking<unsigned char>::get_default_packed_type() {
  return GL_UNSIGNED_BYTE;
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
