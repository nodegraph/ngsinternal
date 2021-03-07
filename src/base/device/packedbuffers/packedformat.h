#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>
#include <base/device/packedbuffers/elementid.h>

namespace ngs {


class DEVICE_EXPORT PackedFormat {
public:
	PackedFormat();

	// Set packed defaults.
	void set_packed_defaults(ElementID id, int num_channels, bool normalized_access);

	// Override defaults.
	void set_packed_format(GLenum packed_format); // will override any previous settings like defaults.
	void set_packed_type(GLenum packed_type); // will override any previous settings like defaults.

	// Packed setting getters.
	bool uses_normalized_access();
	GLenum get_packed_format();
	GLenum get_packed_type();

private:

	// Packed attributes for OpenGL.
	GLenum _packed_format;
	GLenum _packed_type;
	bool _normalized_access;
};

}
