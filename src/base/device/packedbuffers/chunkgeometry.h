#pragma once
#include <base/device/device_export.h>
#include <base/device/packedbuffers/elementid.h>


namespace ngs {

class DEVICE_EXPORT ChunkGeometry {
public:
	ChunkGeometry(ElementID id);

	// Calculated size and dimensions.
	int get_num_elements() const;
	int get_num_bytes() const;
	int get_width_stride_bytes() const;
	int get_height_stride_bytes() const;
	int get_byte_offset(int column, int row = 0, int plane = 0) const;

	// Calculated element info.
	bool element_is_signed() const;
	int get_element_size_bytes() const;
	static int get_element_size_bytes(ElementID elementId);

	int get_pixel_size_bytes() {
		return _num_channels * get_element_size_bytes();
	}
	int get_last_row_offset_bytes(int depth=0) {
		return (depth*get_height_stride_bytes())+(_height-1)*get_width_stride_bytes();
	}

	// -------------------------------------------------------------
	// Operators.
	// -------------------------------------------------------------
	bool operator!=(const ChunkGeometry& right) const;
	bool operator==(const ChunkGeometry& right) const;

	// -------------------------------------------------------------
	// Simple Getters.
	// -------------------------------------------------------------
	int get_x() const {
		return _x;
	}
	int get_y() const {
		return _y;
	}
	int get_width() const {
		return _width;
	}
	int get_height() const {
		return _height;
	}
	int get_depth() const {
		return _depth;
	}
	int get_num_channels() const {
		return _num_channels;
	}
	int get_dim_interpretation() const {
		return _dim_interpretation;
	}
	ElementID get_element_id() const {
		return _element_id;
	}
	int get_alignment_bytes() const {
		return _alignment_bytes;
	}

	// -------------------------------------------------------------
	// Simple Setters.
	// -------------------------------------------------------------
	void set_x(int x) {
		_x = x;
	}
	void set_y(int y) {
		_y = y;
	}
	void set_width(int width) {
		_width = width;
	}
	void set_height(int height) {
		_height = height;
	}
	void set_depth(int depth) {
		_depth = depth;
	}
	void set_num_channels(int num_channels) {
		_num_channels = num_channels;
	}
	void set_dim_interpretation(int dim_interpretation) {
		_dim_interpretation = dim_interpretation;
	}
	void set_element_id(ElementID id) {
		_element_id = id;
	}
	void set_alignment_bytes(int bytes) {
		_alignment_bytes = bytes;
	}

private:

	// Dimensions.
	int _x;
	int _y;
	int _width;
	int _height;
	int _depth;
	int _num_channels;
	// Dimension Interpretation. - (width*height*1) can be viewed as 3d or 2d data.
	int _dim_interpretation;
	// Element Type.
	ElementID _element_id;
	// Alignment
	int _alignment_bytes;

	// multi-sampling
	//	int _num_samples;
	//	bool _fixed_sample_locations;
};

}

