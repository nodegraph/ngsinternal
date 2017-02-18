#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/memoryallocator/taggednew.h>

#include <base/device/unpackedbuffers/texture.h>
#include <base/device/unpackedbuffers/defaultunpacking.h>
#include <base/device/unpackedbuffers/framebuffer.h>
#include <base/device/unpackedbuffers/renderbuffer.h>

#include <base/device/packedbuffers/packedformat.h>
#include <base/device/packedbuffers/packedtexture.h>

#include <iostream>
#include <cstring>

// -----------------------------------------------------------------------------------------
// Packed formats.
// -----------------------------------------------------------------------------------------
// GL_RGBA
// GL_RGB
// GL_RED
// GL_GREEN
// GL_BLUE
// GL_ALPHA
// GL_LUMINANCE_ALPHA
// GL_LUMINANCE
// GL_INTENSITY

// -----------------------------------------------------------------------------------------
// Packed types.
// -----------------------------------------------------------------------------------------
// GL_FLOAT
// GL_UNSIGNED_INT
// GL_INT
// GL_UNSIGNED_SHORT
// GL_SHORT
// GL_UNSIGNED_BYTE
// GL_BYTE

// -----------------------------------------------------------------------------------------
// Cube Face Targets in increasing order.
// -----------------------------------------------------------------------------------------
// GL_TEXTURE_CUBE_MAP_POSITIVE_X
// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z

namespace ngs {

// Creates a default 4 channel float texture on texture unit zero.
Texture::Texture(unsigned int tex_unit, ElementID element_id,
		bool normalized_access) :
		_chunk_geometry(element_id), // creates a default 512x512, 4 channel float chunk size
		_dirty(true) {
	// Set default unpacked formats.
	_unpacked_format.set_unpacked_defaults(element_id, _chunk_geometry.get_num_channels(),
			_chunk_geometry.get_dim_interpretation(), normalized_access);

	// Packed Texture.
	PackedTexture packed_texture(element_id);

	// Packed Format.
	PackedFormat packed_format;
	packed_format.set_packed_defaults(_chunk_geometry.get_element_id(), _chunk_geometry.get_num_channels(),
			normalized_access);

	// Create and load the texture.
	create_name();
	set_defaults();
	bind(tex_unit);
	load_packed_texture(packed_texture, packed_format);
}

Texture::Texture(unsigned int tex_unit, PackedTexture& packed_texture,
		bool normalized_access) :
		_chunk_geometry(packed_texture.get_chunk_geometry()), // copy the chunk slice
		_dirty(true) {
  gpu();

	// Set default unpacked formats.
	_unpacked_format.set_unpacked_defaults(_chunk_geometry.get_element_id(), _chunk_geometry.get_num_channels(),
			_chunk_geometry.get_dim_interpretation(), normalized_access);
	gpu();
	// Packed Format.
	PackedFormat packed_format;
	packed_format.set_packed_defaults(_chunk_geometry.get_element_id(), _chunk_geometry.get_num_channels(),
			normalized_access);
	gpu();
	// Create and load the texture.
	create_name();
	set_defaults();
	bind(tex_unit);
	load_packed_texture(packed_texture, packed_format);
}

Texture::Texture(unsigned int tex_unit, ElementID id, int width, int height,
		int num_channels, bool normalized_access) :
		_chunk_geometry(id), _dirty(true) {

	// Set default unpacked formats.
	_unpacked_format.set_unpacked_defaults(_chunk_geometry.get_element_id(), _chunk_geometry.get_num_channels(),
			_chunk_geometry.get_dim_interpretation(), normalized_access);

	_chunk_geometry.set_width(width);
	_chunk_geometry.set_height(height);
	_chunk_geometry.set_num_channels(num_channels);

	// Create and load the texture.
	create_name();
	set_defaults();
	bind(tex_unit);
	load_null();
}

Texture::~Texture() {
	remove_name();
}

// ------------------------------------------------------------------------------
// Immutable States: - cannot be adjusted after texture creation.
//                   - if adjusted you should reload the texture again.
// ------------------------------------------------------------------------------

// Name

GLuint Texture::get_name() const {
	return _name;
}

// Chunk Geometry.
ChunkGeometry& Texture::get_chunk_geometry() {
	return _chunk_geometry;
}

const ChunkGeometry& Texture::get_chunk_geometry() const {
	return _chunk_geometry;
}

// Unpacked Format.
UnpackedFormat& Texture::get_unpacked_format() {
	return _unpacked_format;
}

// ------------------------------------------------------------------------------
// Usage: 
// ------------------------------------------------------------------------------

// Reallocation.

void Texture::reallocate(int width, int height, int depth, int num_channels) {
	_chunk_geometry.set_width(width);
	_chunk_geometry.set_height(height);
	_chunk_geometry.set_depth(depth);
	_chunk_geometry.set_num_channels(num_channels);

	load_null();
}

void Texture::reallocate(PackedTexture& chunk) {
	_chunk_geometry=chunk.get_chunk_geometry();
	_dirty = true;

	// Packed Format.
	PackedFormat packed_format;
	packed_format.set_packed_defaults(_chunk_geometry.get_element_id(), _chunk_geometry.get_num_channels(),
			_unpacked_format.uses_normalized_access());

	// Load texture.
	set_defaults();
	load_packed_texture(chunk, packed_format);
}

// Loading.

void Texture::load_null() {
  // Create an empty packed texture of the same chunk geometry dimensions.
	// We just need the null pointer inside of it.
	PackedTexture null_chunk(_chunk_geometry);

	// Packed Format.
	PackedFormat packed_format;
	packed_format.set_packed_defaults(_chunk_geometry.get_element_id(), _chunk_geometry.get_num_channels(),
			_unpacked_format.uses_normalized_access());

	// Load packed chunk assumes the geometries match.
	load_packed_texture(null_chunk, packed_format);
}

// Assumes the geometry of the chunk matches the Texture's geometry.
// It doesn't check.
void Texture::load_packed_texture(PackedTexture& chunk,
		PackedFormat& packing_format) {

	GLenum target = _unpacked_format.get_unpacked_target();
	GLenum unpacked_format = _unpacked_format.get_unpacked_format();
	int num_samples = _unpacked_format.get_num_samples();
	bool fixed_sample_locations = _unpacked_format.get_fixed_sample_locations();

	GLenum packed_format = packing_format.get_packed_format();
	GLenum packed_type = packing_format.get_packed_type();

	// Copy the geometry from the chunk.
	_chunk_geometry = chunk.get_chunk_geometry();

	switch (target) {

#if GLES_MAJOR_VERSION >= 1
	case GL_TEXTURE_2D: {
		gpu(
				glTexImage2D(target, _mipmap_base_level, unpacked_format,
						_chunk_geometry.get_width(), _chunk_geometry.get_height(), _legacy_border,
						packed_format, packed_type, chunk.get_pixel(0, 0))
				;)
		;
	}
		break;
  case GL_TEXTURE_CUBE_MAP: {
    int i = 0;
    for (GLenum t = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        t <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; t++) {
      gpu(
          glTexImage2D(t, _mipmap_base_level, unpacked_format,
              _chunk_geometry.get_width(), _chunk_geometry.get_height(), _legacy_border,
              packed_format, packed_type,
              chunk.get_pixel(0, 0, i++))
          ;)
      ;
    }
  }
    break;

#endif

#if GLES_MAJOR_VERSION >= 3
	case GL_TEXTURE_3D: {
		gpu(
				glTexImage3D(target, _mipmap_base_level, unpacked_format, _chunk_geometry.get_width(), _chunk_geometry.get_height(), _chunk_geometry.get_depth(), _legacy_border, packed_format, packed_type, chunk.get_pixel(0,0)););
	}
		break;
  case GL_TEXTURE_2D_ARRAY: {
    int slices = _chunk_geometry.get_depth();
    gpu(
        glTexImage3D(target,_mipmap_base_level,unpacked_format, _chunk_geometry.get_width(),_chunk_geometry.get_height(),slices,_legacy_border, packed_format,packed_type,chunk.get_pixel(0,0,0)););
  }
    break;
#endif

#if GLES_MAJOR_VERSION >= 100
  case GL_TEXTURE_1D: {
    gpu(
        glTexImage1D(target, _mipmap_base_level, unpacked_format,
            _chunk_geometry.get_width(), _legacy_border, packed_format, packed_type,
            chunk.get_pixel(0, 0))
        ;)
    ;
  }
    break;
	case GL_TEXTURE_RECTANGLE: {
		gpu(
				glTexImage2D(target, _mipmap_base_level, unpacked_format,
						_chunk_geometry.get_width(), _chunk_geometry.get_height(), _legacy_border,
						packed_format, packed_type, chunk.get_pixel(0, 0))
				;)
		;
	}
		break;
  case GL_TEXTURE_BUFFER: {
    std::cerr
        << "Error: a texture buffer based texture cannot be created from a packed chunk. use an unpack_chunk instead.\n";
    assert(false);
  }
    break;
  case GL_TEXTURE_1D_ARRAY: {
    int slices = _chunk_geometry.get_height();
    gpu(
        glTexImage2D(target, _mipmap_base_level, unpacked_format,
            _chunk_geometry.get_width(), slices, _legacy_border, packed_format,
            packed_type, chunk.get_pixel(0, 0, 0))
        ;)
    ;
  }
    break;
  case GL_TEXTURE_2D_MULTISAMPLE: {
    std::cerr
        << "Warning attempt to fill a multi-sampled Texture with client data.\n";
    std::cerr
        << "This is not permitted by the opengl spec.  Will default to creating from null.\n";
    glTexImage2DMultisample(target, num_samples, unpacked_format,
        _chunk_geometry.get_width(), _chunk_geometry.get_height(), fixed_sample_locations);
  }
    break;
  case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: {
    std::cerr
        << "Warning attempt to fill a multi-sampled_array Texture with client data.\n";
    std::cerr
        << "This is not permitted by the opengl spec.  Will default to creating from null.\n";
    int slices = _chunk_geometry.get_depth();
    glTexImage3DMultisample(target, num_samples, unpacked_format,
        _chunk_geometry.get_width(), _chunk_geometry.get_height(), slices, fixed_sample_locations);
  }
    break;
#endif






	default: {
		std::cerr
				<< "Error: texture object encountered an unhandled texture target type: "
				<< target << "\n";
	}
		break;
	}
}

void Texture::load_subsection(GLint mipmap_level, GLint x_offset,
		GLint y_offset, GLint z_offset, PackedTexture& packed_texture,
		PackedFormat& packed_format) const {
	GLenum target = _unpacked_format.get_unpacked_target();
	switch (target) {
#if GLES_MAJOR_VERSION >= 1
	  case GL_TEXTURE_2D: {
	    gpu(
	        glTexSubImage2D(target, mipmap_level, x_offset, y_offset,
	            packed_texture.get_chunk_geometry().get_width(), packed_texture.get_chunk_geometry().get_height(),
	            packed_format.get_packed_format(),
	            packed_format.get_packed_type(), packed_texture.get_pixel(0, 0))
	        ;)
	    ;
	  }
	    break;
	  case GL_TEXTURE_CUBE_MAP: {
	    int i = 0;
	    for (GLenum t = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	        t <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; t++) {
	      gpu(
	          glTexSubImage2D(t, mipmap_level, x_offset, y_offset,
	              packed_texture.get_chunk_geometry().get_width(), packed_texture.get_chunk_geometry().get_height(),
	              packed_format.get_packed_format(),
	              packed_format.get_packed_type(),
	              packed_texture.get_pixel(0, 0, i++))
	          ;)
	      ;
	    }
	  }
	    break;
#endif

#if GLES_MAJOR_VERSION >= 3
    case GL_TEXTURE_2D_ARRAY: {
      int slices = packed_texture.get_chunk_geometry().get_depth();
      gpu(
          glTexSubImage3D(target, mipmap_level, x_offset, y_offset, z_offset, packed_texture.get_chunk_geometry().get_width(), packed_texture.get_chunk_geometry().get_height(), slices, packed_format.get_packed_format(), packed_format.get_packed_type(), packed_texture.get_pixel(0,0,0)););
    }
      break;
	  case GL_TEXTURE_3D: {
	    gpu(
	        glTexSubImage3D(target, mipmap_level, x_offset, y_offset, z_offset, packed_texture.get_chunk_geometry().get_width(), packed_texture.get_chunk_geometry().get_height(),packed_texture.get_chunk_geometry().get_depth(), packed_format.get_packed_format(), packed_format.get_packed_type(), packed_texture.get_pixel(0,0)););
	  }
	    break;
#endif

#if GLES_MAJOR_VERSION >= 100
	  case GL_TEXTURE_1D: {
	    gpu(
	        glTexSubImage1D(target, mipmap_level, x_offset,
	            packed_texture.get_chunk_geometry().get_width(), packed_format.get_packed_format(),
	            packed_format.get_packed_type(), packed_texture.get_pixel(0, 0))
	        ;)
	    ;
	  }
	    break;
	  case GL_TEXTURE_RECTANGLE: {
	    gpu(
	        glTexSubImage2D(target, mipmap_level, x_offset, y_offset,
	            packed_texture.get_chunk_geometry().get_width(), packed_texture.get_chunk_geometry().get_height(),
	            packed_format.get_packed_format(),
	            packed_format.get_packed_type(), packed_texture.get_pixel(0, 0))
	        ;)
	    ;
	  }
	    break;
	  case GL_TEXTURE_BUFFER: {
	    std::cerr
	        << "Error: attempt to load a subsection of a texture object from an unpacked buffer.\n";
	    std::cerr
	        << "       instead you should load directly into a subsection of the unpacked buffer object.\n";
	  }
	    break;

	  case GL_TEXTURE_1D_ARRAY: {
	    int slices = packed_texture.get_chunk_geometry().get_height();
	    gpu(
	        glTexSubImage2D(target, mipmap_level, x_offset, y_offset,
	            packed_texture.get_chunk_geometry().get_width(), slices,
	            packed_format.get_packed_format(),
	            packed_format.get_packed_type(),
	            packed_texture.get_pixel(0, 0, 0))
	        ;)
	    ;
	  }
	    break;
	  case GL_TEXTURE_2D_MULTISAMPLE: {
	    std::cerr
	        << "Warning attempt to fill a subsection of a multi-sampled UnpackedTexture with client data.\n";
	    std::cerr << "This is not permitted by the opengl spec.\n";
	  }
	    break;
	  case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: {
	    std::cerr
	        << "Warning attempt to fill a subsection of a multi-sampled_array UnpackedTexture with client data.\n";
	    std::cerr
	        << "This is not permitted by the opengl spec.  Will default to creating from null.\n";
	  }
	    break;
#endif




	default: {
		std::cerr
				<< "Error: texture object encountered an unhandled texture target type: "
				<< target << "\n";
	}
		break;
	}
}
#if GLES_MAJOR_VERSION < 100
// Note the texture must be bound before this is called.
PackedTexture* Texture::create_packed_texture() const {

  PackedTexture* packed_texture = new_ff PackedTexture(_chunk_geometry);
  void* data = packed_texture->get_pixel(0, 0, 0);

  // Chunk Geometry.
  ElementID element_id = _chunk_geometry.get_element_id();
  int num_channels = _chunk_geometry.get_num_channels();

  // Unpacked Format.
  GLenum target = _unpacked_format.get_unpacked_target();
  bool normalized_access = _unpacked_format.uses_normalized_access();

  // Packed Format.
  PackedFormat packing_format;
  packing_format.set_packed_defaults(element_id, num_channels, normalized_access);

  GLenum packed_format = packing_format.get_packed_format();
  GLenum packed_type = packing_format.get_packed_type();

  switch (target) {
    // We only support texture 2D at the moment.
    case GL_TEXTURE_2D: {
      FrameBuffer* fbo = new_ff FrameBuffer();
      fbo->bind();
      Texture* temp = const_cast<Texture*>(this);
      fbo->attach_texture(GL_COLOR_ATTACHMENT0, *temp, 0);

//#if GLES_MAJOR_VERSION >= 3
//      RenderBuffer* rbo = new_ff RenderBuffer(GL_DEPTH32F_STENCIL8, _chunk_geometry->get_width(), _chunk_geometry->get_height());
//#elif GLES_MAJOR_VERSION >=2
//      RenderBuffer* rbo = new_ff RenderBuffer(GL_DEPTH_COMPONENT16, _chunk_geometry.get_width(), _chunk_geometry.get_height());
//#endif
//
//      // Attach depth buffer.
//      fbo->attach_render_buffer(GL_DEPTH_ATTACHMENT, *rbo);

      // Check fbo completeness. - needs depth attachment to be complete.
      assert(fbo->is_complete());

      // Get the current viewport size.
      int viewport[4];
      gpu(glGetIntegerv(GL_VIEWPORT, viewport));
      // Set the viewport to be the size of the texture.
      int width = get_chunk_geometry().get_width();
      int height = get_chunk_geometry().get_height();
      gpu(glViewport(0, 0, width, height));

#if GLES_MAJOR_VERSION <= 2
      {
        // The data can only be read to certain formats.
        // We assume the following. It will assert otherwise.
        // Usually it's. GL_RGBA with GL_UNSIGNED_BYTE.
        GLint ext_format;
        GLint ext_type;
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &ext_format);
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &ext_type);

        if (ext_format != GL_RGBA) {
          assert(false);
        }
        if (ext_type != GL_UNSIGNED_BYTE) {
          assert(false);
        }

        // Read the pixels.
        gpu(glReadPixels(0, 0, width, height, packed_format, packed_type, data));
      }
#else
      // Read the pixels.
      gpu(glReadPixels(0, 0, width, height, packed_format, packed_type, data));
#endif
      // Restore the viewport's size.
      gpu(glViewport(viewport[0], viewport[1], viewport[2], viewport[3]));
      // Bind the default frame buffer back.
      fbo->unbind();
      delete_ff(fbo);
    }
      break;
    default: {
    }
      break;
  }
  return packed_texture;
}
#endif


#if GLES_MAJOR_VERSION >= 100
PackedTexture* Texture::create_packed_texture() const {

	PackedTexture* packed_texture = new_ff PackedTexture(_chunk_geometry);
	void* data = packed_texture->get_pixel(0, 0, 0);

	// Chunk Geometry.
	ElementID element_id = _chunk_geometry.get_element_id();
	int num_channels = _chunk_geometry.get_num_channels();

	// Unpacked Format.
	GLenum target = _unpacked_format.get_unpacked_target();
	bool normalized_access = _unpacked_format.uses_normalized_access();

	// Packed Format.
	PackedFormat packing_format;
	packing_format.set_packed_defaults(element_id, num_channels,
			normalized_access);

	GLenum packed_format = packing_format.get_packed_format();
	GLenum packed_type = packing_format.get_packed_type();

	switch (target) {

    case GL_TEXTURE_1D: {
      gpu(glGetTexImage(target, _mipmap_base_level, packed_format, packed_type, data));
    }
      break;
    case GL_TEXTURE_2D: {
      gpu(glGetTexImage(target, _mipmap_base_level, packed_format, packed_type, data));
    }
    break;
    case GL_TEXTURE_3D: {
      gpu(glGetTexImage(target, _mipmap_base_level, packed_format, packed_type, data));
    }
    break;
    case GL_TEXTURE_RECTANGLE: {
      gpu(glGetTexImage(target, _mipmap_base_level, packed_format, packed_type, data));
    }
    break;
    case GL_TEXTURE_BUFFER: {
      std::cerr << "Error: attempt to download from a texture object base on a buffer object.\n";
      std::cerr << "       instead you should download directly from the buffer object.\n";
    }
    break;
    case GL_TEXTURE_CUBE_MAP: {
      int i = 0;
      for (GLenum t = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
          t <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; t++) {
        gpu(glGetTexImage(target, _mipmap_base_level, packed_format, packed_type, packed_texture->get_pixel(0, 0, i++)));
      }
    }
    break;
    case GL_TEXTURE_1D_ARRAY: {
      gpu(glGetTexImage(target, _mipmap_base_level, packed_format, packed_type, data));
    }
    break;
    case GL_TEXTURE_2D_ARRAY: {
      gpu(glGetTexImage(target, _mipmap_base_level, packed_format, packed_type, data));
    }
    break;
    case GL_TEXTURE_2D_MULTISAMPLE: {
      std::cerr << "Warning attempt to download a multi-sampled Texture.\n";
      std::cerr << "This is not permitted by the opengl spec.\n";
    }
    break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: {
      std::cerr << "Warning attempt to download a multi-sampled_array Texture.\n";
      std::cerr << "This is not permitted by the opengl spec..\n";
    }
    break;
    default: {
      std::cerr << "Error: texture object encountered an unhandled texture target type: " << target << "\n";
    }
    break;
  }

	return packed_texture;
}
#endif

// Binding

void Texture::bind(unsigned int tex_unit) {
	GLenum target = _unpacked_format.get_unpacked_target();

	// Bind.
	Device::disable_texture_targets(tex_unit);
	Device::enable_texture_target(tex_unit, target);
	gpu(glBindTexture(target, _name));
	update_gl();

	gpu(glPixelStorei(GL_PACK_ALIGNMENT,_chunk_geometry.get_alignment_bytes()));
#if GLES_MAJOR_VERSION >=3
	gpu(glPixelStorei(GL_PACK_ROW_LENGTH,0));
#endif

	// Note we currently we unpack with the same alignment.
	// From the gl spec we should be able to use different values, but we're seeing one half or short element misalignments during read back.
	gpu(glPixelStorei(GL_UNPACK_ALIGNMENT,_chunk_geometry.get_alignment_bytes()));
#if GLES_MAJOR_VERSION>=3
	gpu(glPixelStorei(GL_UNPACK_ROW_LENGTH,0));
#endif
}

void Texture::unbind(unsigned int tex_unit) {

	GLenum target = _unpacked_format.get_unpacked_target();

	Device::disable_texture_targets(tex_unit);
	gpu(glBindTexture(target, 0));
}

// ------------------------------------------------------------------------------
// Private: 
// ------------------------------------------------------------------------------

bool Texture::name_is_valid(GLuint name) {
	GLboolean success;
	gpu(success = glIsTexture(name));
	if (success) {
		return true;
	}
	return false;
}

void Texture::create_name() {
	gpu(glGenTextures(1, &_name));
	if (_name == 0) {
		std::cerr
				<< "Error: memory error is likely. unable to create a new_ texture name.\n";
		assert(false);
	}
}

void Texture::remove_name() {
	if (_name != 0) {
		gpu(glDeleteTextures(1, &_name));
	}
	_name = 0;
}

// The texture must already be bound before this is called.
void Texture::update_gl() {
	GLenum target = _unpacked_format.get_unpacked_target();
	// If we're dirty, we need to let opengl know about our changes.
	if (_dirty) {
		// Mipmapping.
#if GLES_MAJOR_VERSION >= 3
		gpu(glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, _mipmap_base_level));
		gpu(glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, _mipmap_max_level));
		gpu(glTexParameterf(target, GL_TEXTURE_MIN_LOD, _mipmap_min_lod));
		gpu(glTexParameterf(target, GL_TEXTURE_MAX_LOD, _mipmap_max_lod));
#endif

		// Generate all the mipmaps.
		if (_mipmap_base_level != _mipmap_max_level) {
			glGenerateMipmap(target);
		}

		// Filters.
		gpu(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, _min_filter));
		gpu(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, _mag_filter));

		// Anisotropic filtering.
#if GLES_MAJOR_VERSION >= 100
		gpu(glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, _anisotropic_amount));
#endif
		// Depth comparision.
#if GLES_MAJOR_VERSION >= 3
		if (_use_depth_comparison) {
			gpu(
					glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
			gpu(
					glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, _depth_comparison_operator));
		} else {
			gpu(glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, 0));
		}
#endif

		// Edge value sampling.
		gpu(glTexParameteri(target, GL_TEXTURE_WRAP_S, _texture_wrap_s_mode));
		gpu(glTexParameteri(target, GL_TEXTURE_WRAP_T, _texture_wrap_t_mode));
#if GLES_MAJOR_VERSION >= 3
		gpu(glTexParameteri(target, GL_TEXTURE_WRAP_R, _texture_wrap_r_mode));
#endif
		_dirty = false;
	}
}

// ------------------------------------------------------------------------------
// Mutable States: - these can be adjusted after texture creation.
//                 - adjustments will take effect on the next texture bind.
// ------------------------------------------------------------------------------

void Texture::set_defaults() {
	// only one mipmap level
	_mipmap_base_level = 0;
	_mipmap_max_level = 0;
	_mipmap_min_lod = -1000;  //gl default
	_mipmap_max_lod = 1000;  //gl default

	// nearest filtering
	set_default_nearest_filters();

	// no anisotropic filtering
	_anisotropic_amount = 1.0;

	// no depth comparison
	_use_depth_comparison = false;

	// edge value sampling repeats
	_texture_wrap_s_mode = GL_REPEAT;
	_texture_wrap_t_mode = GL_REPEAT;
	_texture_wrap_r_mode = GL_REPEAT;
}

// mip maps

void Texture::set_mipmap_base_level(GLint level) {
	_mipmap_base_level = level;
	_dirty = true;
}

void Texture::set_mipmap_max_level(GLint level) {
	_mipmap_max_level = level;
	_dirty = true;
}

void Texture::set_min_lod(GLfloat lod) {
	_mipmap_min_lod = lod;
	_dirty = true;
}

void Texture::set_max_lod(GLfloat lod) {
	_mipmap_max_lod = lod;
	_dirty = true;
}

// filters

void Texture::set_min_filter(GLenum filter) {
	_min_filter = filter;
	_dirty = true;
}

void Texture::set_mag_filter(GLenum filter) {
	_mag_filter = filter;
	_dirty = true;
}

void Texture::set_default_nearest_filters() {
	_min_filter = GL_NEAREST;
	_mag_filter = GL_NEAREST;
	_dirty = true;
}

void Texture::set_default_linear_filters() {
	_min_filter = GL_LINEAR;
	_mag_filter = GL_LINEAR;
	_dirty = true;
}

void Texture::set_default_mipmap_filters() {
	_min_filter = GL_LINEAR_MIPMAP_NEAREST;
	_mag_filter = GL_LINEAR;
	_dirty = true;
}

// anisotropic filtering
void Texture::set_anisotropic_filtering(GLfloat amount) {
	_anisotropic_amount = amount;
	_dirty = true;
}

// depth comparison
void Texture::enable_depth_comparison() {
	_use_depth_comparison = true;
	_dirty = true;
}

void Texture::disable_depth_comparison() {
	_use_depth_comparison = false;
	_dirty = true;
}

void Texture::set_depth_comparison_operator(GLenum mode) {
	_depth_comparison_operator = mode;
	_dirty = true;
}

// edge sampling mode

void Texture::set_texture_wrap_s_mode(GLenum mode) {
	_texture_wrap_s_mode = mode;
	_dirty = true;
}

void Texture::set_texture_wrap_t_mode(GLenum mode) {
	_texture_wrap_t_mode = mode;
	_dirty = true;
}

void Texture::set_texture_wrap_r_mode(GLenum mode) {
	_texture_wrap_r_mode = mode;
	_dirty = true;
}

}

namespace ngs {

// ---------------------------------------------------------------------------------------------
// Static creator methods.
// ---------------------------------------------------------------------------------------------

/*
 the GL_FLOAT_32_UNSIGNED_INT_24_8_REV packed format uses one 32 bit for floating point depth,
 and one 32 bit region to store the depth.  However only the first 8 bits are used, the remaining 
 24 bits are unused.
 */
#if GLES_MAJOR_VERSION >= 3
Texture* create_depth_stencil_texture(unsigned int tex_unit, int width,
		int height) {
	// Null Packed Texture.
	PackedTexture packed_texture(FloatElement);
	PackedFormat format;
	format.set_packed_format(GL_DEPTH_STENCIL);
	format.set_packed_type(GL_FLOAT_32_UNSIGNED_INT_24_8_REV);

	// Unpacked Texture.
	Texture* texture = new_ff Texture(tex_unit, FloatElement, true);
	texture->get_chunk_geometry().set_width(width);
	texture->get_chunk_geometry().set_height(height);
	texture->get_chunk_geometry().set_num_channels(2);
	texture->get_unpacked_format().set_unpacked_format(GL_DEPTH32F_STENCIL8);

	texture->bind(tex_unit);
	texture->load_packed_texture(packed_texture, format);

	return texture;
}
#endif

namespace {

template<class T>
void fill_row(char* raw, int width, int num_channels) {
	T* element = reinterpret_cast<T*>(raw);
	for (int j = 0; j < width * num_channels; j++) {
		*element = 1;
		++element;
	}
}

}

// Create a black texture with a white horizontal bar in the middle.
PackedTexture* create_bar_packed_texture(ElementID element_id, int width, int height,
		int depth, int num_channels) {

	// Create a packed texture with a horizontal bar in it.
	PackedTexture* packed_texture = new_ff PackedTexture(element_id);
	packed_texture->get_chunk_geometry().set_element_id(element_id);
	packed_texture->reallocate(width, height, depth, num_channels);

	// Memset the elements to zero.
	{
		char* raw = packed_texture->get_pixel();
		memset(raw, 0, packed_texture->get_chunk_geometry().get_num_bytes());
	}

	// Fill in the horizontal bar.
	size_t element_size = packed_texture->get_chunk_geometry().get_element_size_bytes();
	size_t row_size = element_size * num_channels * width;
	for (int i = height / 2; i < height; i++) {
		char* raw = packed_texture->get_pixel(0, i, 0);

		switch (element_id) {
		case FloatElement:
			fill_row<float>(raw, width, num_channels);
			break;
		case HalfElement:
			fill_row<half>(raw, width, num_channels);
			break;
		case IntElement:
			fill_row<int>(raw, width, num_channels);
			break;
		case UIntElement:
			fill_row<unsigned int>(raw, width, num_channels);
			break;
		case ShortElement:
			fill_row<short>(raw, width, num_channels);
			break;
		case UShortElement:
			fill_row<unsigned short>(raw, width, num_channels);
			break;
		case CharElement:
			fill_row<char>(raw, width, num_channels);
			break;
		case UCharElement:
			fill_row<unsigned char>(raw, width, num_channels);
			break;
		default:
			assert(false && "unknown element type was encountered");
		}
	}
	return packed_texture;
}

// Create a black texture with a white horizontal bar in the middle.
Texture* create_bar_texture(ElementID element_id, int width, int height,
		int depth, int num_channels, bool normalized_access) {

	// Create a temporary packed texture with a horizontal bar in it.
	PackedTexture* packed_texture = create_bar_packed_texture(element_id, width, height, depth, num_channels);

	// Double check the packed_texture contents.
	check_bar_packed_texture(packed_texture, 1, 0);

	// Create our render target.
	Texture* texture = new_ff Texture(0, *packed_texture, normalized_access);
	texture->unbind(0);

	// Delete the temporary packed texture.
	delete_ff(packed_texture);

	return texture;
}

namespace {

template<class T>
void check_row(char* raw, int width, int num_channels, T value) {
	T* element = reinterpret_cast<T*>(raw);
	for (int j = 0; j < width * num_channels; j++) {
		if (*element != value) {
			std::cerr << "Error: unexpected contents in input texture: " << *element << " but expected: " << value << "\n";
			assert(false);
		}
		++element;
	}
}

}

void check_bar_packed_texture(PackedTexture* packed_texture, float bar_value, float background_value) {

	int width = packed_texture->get_chunk_geometry().get_width();
	int height = packed_texture->get_chunk_geometry().get_height();
	int num_channels = packed_texture->get_chunk_geometry().get_num_channels();
	size_t element_size = packed_texture->get_chunk_geometry().get_element_size_bytes();

	// Check the horizontal bar.
	size_t row_size = element_size * num_channels * width;
	for (int i = height / 2; i < height; i++) {
		char* raw = packed_texture->get_pixel(0, i, 0);
		switch (packed_texture->get_chunk_geometry().get_element_id()) {
		case FloatElement:
			check_row<float>(raw, width, static_cast<float>(num_channels),
					static_cast<float>(bar_value));
			break;
		case HalfElement:
			check_row<half>(raw, width, static_cast<half>(num_channels),
					static_cast<half>(bar_value));
			break;
		case IntElement:
			check_row<int>(raw, width, static_cast<int>(num_channels),
					static_cast<int>(bar_value));
			break;
		case UIntElement:
			check_row<unsigned int>(raw, width,
					static_cast<unsigned int>(num_channels),
					static_cast<unsigned int>(bar_value));
			break;
		case ShortElement:
			check_row<short>(raw, width, static_cast<short>(num_channels),
					static_cast<short>(bar_value));
			break;
		case UShortElement:
			check_row<unsigned short>(raw, width,
					static_cast<unsigned short>(num_channels),
					static_cast<unsigned short>(bar_value));
			break;
		case CharElement:
			check_row<char>(raw, width, static_cast<char>(num_channels),
					static_cast<char>(bar_value));
			break;
		case UCharElement:
			check_row<unsigned char>(raw, width,
					static_cast<unsigned char>(num_channels),
					static_cast<unsigned char>(bar_value));
			break;
		default:
			assert(false && "unknown element type encountered");
		}
	}

	// Check the bottom background.
	for (int i = 0; i < height / 2; i++) {
		char* raw = packed_texture->get_pixel(0, i, 0);

		switch (packed_texture->get_chunk_geometry().get_element_id()) {
		case FloatElement:
			check_row<float>(raw, width, static_cast<float>(num_channels),
					static_cast<float>(background_value));
			break;
		case HalfElement:
			check_row<half>(raw, width, static_cast<half>(num_channels),
					static_cast<half>(background_value));
			break;
		case IntElement:
			check_row<int>(raw, width, static_cast<int>(num_channels),
					static_cast<int>(background_value));
			break;
		case UIntElement:
			check_row<unsigned int>(raw, width,
					static_cast<unsigned int>(num_channels),
					static_cast<unsigned int>(background_value));
			break;
		case ShortElement:
			check_row<short>(raw, width, static_cast<short>(num_channels),
					static_cast<short>(background_value));
			break;
		case UShortElement:
			check_row<unsigned short>(raw, width,
					static_cast<unsigned short>(num_channels),
					static_cast<unsigned short>(background_value));
			break;
		case CharElement:
			check_row<char>(raw, width, static_cast<char>(num_channels),
					static_cast<char>(background_value));
			break;
		case UCharElement:
			check_row<unsigned char>(raw, width,
					static_cast<unsigned char>(num_channels),
					static_cast<unsigned char>(background_value));
			break;
		default:
			assert(false && "unknown element type encountered");
		}
	}
}

// Assumes all components of the texture are either bar_value or background_value.
void check_bar_texture(Texture* texture, float bar_value,
		float background_value) {

	// Download the texture to a packed texture.
	texture->bind(0);
	PackedTexture* packed_texture = texture->create_packed_texture();
	texture->unbind(0);

	// Check the packed texture.
	check_bar_packed_texture(packed_texture, bar_value, background_value);

	// Cleanup packed texture.
	delete_ff(packed_texture)
	;
}

}

