#pragma once
#include <base/device/device_export.h>


#include <base/device/packedbuffers/chunkgeometry.h>
#include <base/device/unpackedbuffers/unpackedformat.h>
#include <string>

// Possible texture target types.
// GL_TEXTURE_1D
// GL_TEXTURE_2D
// GL_TEXTURE_3D
// GL_TEXTURE_RECTANGLE
// GL_TEXTURE_BUFFER
// GL_TEXTURE_CUBE_MAP
// GL_TEXTURE_1D_ARRAY
// GL_TEXTURE_2D_ARRAY
// GL_TEXTURE_2D_MULTISAMPLE
// GL_TEXTURE_2D_MULTISAMPLE_ARRAY

// Possible filter type values.
// GL_NEAREST
// GL_LINEAR
// GL_NEAREST_MIPMAP_NEAREST
// GL_LINEAR_MIPMAP_NEAREST
// GL_NEAREST_MIPMAP_LINEAR
// GL_LINEAR_MIPMAP_LINEAR

// // Possible values for depth comparison modes. (reference_value OPERATOR texture_value)
// GL_NEVER (always fails)
// GL_ALWAYS (always succeeds)
// GL_LESS
// GL_LEQUAL
// GL_EQUAL
// GL_NOT_EQUAL 
// GL_GEQUAL 
// GL_GREATER

// Possible edge sampling modes.
// GL_REPEAT: the texture coordinate wraps around the texture. So a texture coordinate of -0.2 becomes the equivalent of 0.8.
// GL_MIRRORED_REPEAT: the texture coordinate wraps around like a mirror. -0.2 becomes 0.2, -1.2 becomes 0.8, etc.
// GL_CLAMP_TO_EDGE: the texture coordinate is clamped to the [0, 1] range.
// GL_CLAMP_TO_BORDER: the texture coordinate is clamped to the [0, 1] range, but the edge texels are blended with a constant border color.

// -----------------------------------------------------------------------------------------
// Possible Internal formats.
// -----------------------------------------------------------------------------------------

// notes.  the difference between "arb" and "ext" GL_ tokens is that ext is only supported
//         by a few manufactureres.  "ext" gradually moves to "arb" which then moves to a 
//         a name without a suffix.

// Half float formats
// GL_RGBA16F
// GL_RGB16F
// GL_LUMINANCE_ALPHA16F_ARB
// GL_LUMINANCE16F_ARB
// GL_ALPHA16F_ARB
// GL_INTENSITY16F_ARB

// Float formats
// GL_RGBA32F
// GL_RGB32F
// GL_LUMINANCE_ALPHA32F_ARB
// GL_LUMINANCE32F_ARB
// GL_ALPHA32F_ARB
// GL_INTENSITY32F_ARB

// 32 bit unsigned int formats
// GL_RGBA32UI
// GL_RGB32UI
// GL_LUMINANCE_ALPHA32UI_EXT
// GL_LUMINANCE32UI_EXT
// GL_ALPHA32UI_EXT
// GL_INTENSITY32UI_EXT

// 16 bit unsigned int formats
// GL_RGBA16UI
// GL_RGB16UI
// GL_LUMINANCE_ALPHA16UI_EXT
// GL_LUMINANCE16UI_EXT
// GL_ALPHA16UI_EXT
// GL_INTENSITY16UI_EXT

// 8 bit unsigned int formats
// GL_RGBA8UI
// GL_RGB8UI
// GL_LUMINANCE_ALPHA8UI_EXT
// GL_LUMINANCE8UI_EXT
// GL_ALPHA8UI_EXT
// GL_INTENSITY8UI_EXT

// 32 bit int formats
// GL_RGBA32I
// GL_RGB32I
// GL_LUMINANCE_ALPHA32I_EXT
// GL_LUMINANCE32I_EXT
// GL_ALPHA32I_EXT
// GL_INTENSITY32I_EXT

// 16 bit int formats
// GL_RGBA16I
// GL_RGB16I
// GL_LUMINANCE_ALPHA16I_EXT
// GL_LUMINANCE16I_EXT
// GL_ALPHA16I_EXT
// GL_INTENSITY16I_EXT

// 8 bit int formats
// GL_RGBA8I
// GL_RGB8I
// GL_LUMINANCE_ALPHA8I_EXT
// GL_LUMINANCE8I_EXT
// GL_ALPHA8I_EXT
// GL_INTENSITY8I_EXT

// Note: 
// To load a texture without any data from the cpu memory, just call load_null.
// load_null will use default packed_target, packed_format, and packed_type.      
// In some cases like creating depth/stencil buffers for FBO's you'll need to
// explicitly set the packed_target, packed_format, and packed_type.
// In this case you should create a null packed texture and load that into
// the texture. The load_packed_texture will pick up the packed types from the
// null packed texture.

namespace ngs {

class PackedTexture;
class TextureBuffer;
class PackedFormat;

class DEVICE_EXPORT Texture {
public:
	Texture(unsigned int tex_unit, ElementID id, bool normalized_access); // creates a texture object but doesn't load it.
	Texture(unsigned int tex_unit, PackedTexture& chunk,
			bool normalized_access);
	Texture(unsigned int tex_unit, ElementID id, int width, int height,
			int num_channels, bool normalized_access);
	virtual ~Texture();

	// ------------------------------------------------------------------------------
	// Immutable States: - cannot be adjusted after texture creation.
	//                   - if adjusted you should reload the texture again.
	// ------------------------------------------------------------------------------

	// Name
	GLuint get_name() const;

	// Chunk Geometry.
	ChunkGeometry& get_chunk_geometry();
	const ChunkGeometry& get_chunk_geometry() const;

	// Unpacked Format.
	UnpackedFormat& get_unpacked_format();

	// ------------------------------------------------------------------------------
	// Usage: These only work if the texture is currently bound.
	// ------------------------------------------------------------------------------

	// Bind.
	void bind(unsigned int tex_unit);
	void unbind(unsigned int tex_unit);

	// Reallocate gpu memory.
	void reallocate(int width, int height, int depth, int num_channels);

	// Reallocate from a packed chunk..
	void reallocate(PackedTexture& chunk);

	// Loading.
	void load_null();
	void load_packed_texture(PackedTexture& chunk, PackedFormat& format);
	void load_subsection(GLint mipmap_level, GLint x_offset, GLint y_offset,
			GLint z_offset, PackedTexture& chunk,
			PackedFormat& packed_format) const;

	// Dump to a packed texture.
	PackedTexture* create_packed_texture() const;

	// ------------------------------------------------------------------------------
	// Mutable States: - these can be adjusted after texture creation.
	//                 - adjustments will take effect on the next texture bind.
	// ------------------------------------------------------------------------------

	// Texture properties.
	void set_defaults();

	// Mip maps.
	void set_mipmap_base_level(int level);
	void set_mipmap_max_level(int level);
	void set_min_lod(float lod);
	void set_max_lod(float lod);

	// Filters.
	void set_mag_filter(GLenum filter);
	void set_min_filter(GLenum filter);

	// Filter defaults.
	void set_default_nearest_filters();
	void set_default_linear_filters();
	void set_default_mipmap_filters();

	// Anisotropic filtering.
	void set_anisotropic_filtering(float amount);

	// Comparison mode.
	void enable_depth_comparison();
	void disable_depth_comparison();
	void set_depth_comparison_operator(GLenum mode);

	// Edge value sampling mode.
	void set_texture_wrap_s_mode(GLenum mode);
	void set_texture_wrap_t_mode(GLenum mode);
	void set_texture_wrap_r_mode(GLenum mode);

private:
	bool name_is_valid(GLuint name);
	void create_name();
	void remove_name();

	// Updates any new_ settings to opengl.
	void update_gl();

	// ------------------------------------------------------------------------------
	// Immutable State: cannot be adjusted after construction.
	// ------------------------------------------------------------------------------

	// Name of object.
	GLuint _name;

	// Chunk Geometry.
	ChunkGeometry _chunk_geometry;

	// Unpacked format.
	UnpackedFormat _unpacked_format;

	// Legacy border setting.
	static const GLint _legacy_border = 0;

	// Mip map level of texture.
	GLint _mip_map_level;

	// ------------------------------------------------------------------------------
	// Mutable State: these can all be adjusted at runtime.
	// ------------------------------------------------------------------------------

	// mip maps
	GLint _mipmap_base_level;
	GLint _mipmap_max_level;
	GLfloat _mipmap_min_lod;
	GLfloat _mipmap_max_lod;

	// filters
	GLenum _mag_filter;
	GLenum _min_filter;

	// anisotropic filtering
	float _anisotropic_amount;

	// depth comparison mode
	bool _use_depth_comparison;
	GLenum _depth_comparison_operator;

	// edge value sampling
	GLenum _texture_wrap_s_mode;
	GLenum _texture_wrap_t_mode;
	GLenum _texture_wrap_r_mode;

	// If dirty is true, we haven't updated the opengl state yet.
	bool _dirty;

};
// Static helper creator methods. 
#if GLES_MAJOR_VERSION >= 3
DEVICE_EXPORT Texture* create_depth_stencil_texture(unsigned int tex_unit, int width,
		int height);
#endif

// Create a packed texture with background zero, and 1 in a bar like region.
DEVICE_EXPORT PackedTexture* create_bar_texture(ElementID element_id, int width, int height,
		int depth, int num_channels);

// Create a texture with background zero, and 1 in a bar like region.
DEVICE_EXPORT Texture* create_bar_texture(ElementID element_id, int width, int height,
		int depth, int num_channels, bool normalized_access);

DEVICE_EXPORT PackedTexture* create_bar_packed_texture(ElementID element_id, int width, int height,
    int depth, int num_channels);

// Checks the pixels in a bar like region of the texture.
DEVICE_EXPORT void check_bar_packed_texture(PackedTexture* packed_texture, float bar_value,
		float background_value);

// Checks the pixels in a bar like region of the texture.
DEVICE_EXPORT void check_bar_texture(Texture* texture, float bar_value,
		float background_value);

}

