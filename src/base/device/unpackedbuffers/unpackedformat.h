#include <base/device/device_export.h>
#include <base/device/packedbuffers/elementid.h>
#include <base/device/devicebasictypesgl.h>


namespace ngs {

class DEVICE_EXPORT UnpackedFormat {
public:
	UnpackedFormat();

	// Sets default format and type.
	void set_unpacked_defaults(ElementID id, int num_channels, int num_dimensions, bool normalized_access);
	bool uses_normalized_access() const;

	// Target.
	void set_unpacked_target(GLenum unpacked_target); // You can override default.
	GLenum get_unpacked_target() const;

	// Format.
	void set_unpacked_format(GLenum unpacked_format); // You can override default.
	GLenum get_unpacked_format() const;

	// MultiSampling.
	void set_num_samples(int num);  // You can override default.
	void set_fixed_sample_locations(bool fixed);
	int get_num_samples() const;
	bool get_fixed_sample_locations() const;

private:

	// Binding target.
	GLenum _target;

	// Unpacked internal format
	GLenum _unpacked_format;

	// Whether the pixels are to access as normalized values (0.0 to 1.0) in the shaders or as integers.
	bool _normalized_access;

	// Texture MultiSampling.
	int _num_samples;
	bool _fixed_sample_locations;
};

}
