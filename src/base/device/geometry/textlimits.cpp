#include <base/device/geometry/textlimits.h>
#include <base/device/geometry/instancevertexattribute.h>
#include <base/device/geometry/objectvertexattribute.h>

#include <base/memoryallocator/taggednew.h>

#include <freetype-gl/texture-font.h>
#include <freetype-gl/texture-atlas.h>

#include <iostream>

namespace ngs {

TextLimits::TextLimits(const std::string& font_data) {
	// These ftgl objects won't use opengl during construction.
	// The texture_font_new_from_memory method immediately creates a texture_font_t
	// object and sets the atlas member to the _atlas argument.
	// Tracing the code we see that it can call texture_atlas_upload() which uses opengl.
	// However this only happens when texture_font_load_gyphs is called without at -1 argument.

	// Note we only create the font structure so that comp shapes can tessellate text.
	// Otherwise the ftgl objects here do not touch the gpu.
	// The TextPipelineSetup object has a copy of these ftgl object which it actually
	// uses to manipulate gpu state.
	// We create copies of these ftgl structures so that we can keep the tessellation of shapes
	// completely separate from the gpu state.

	_atlas = ftgl::texture_atlas_new(724, 724, 1);
	_font = ftgl::texture_font_new_from_memory(_atlas, 96, &font_data[0],
			font_data.size()); // original pt_size=48

	// We create the glyphs on the system side for all the letters we would ever need.
	const char *cache = " !\"#$%&'()*+,-./0123456789:;<=>?"
			"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
			"`abcdefghijklmnopqrstuvwxyz{|}~";
	ftgl::texture_font_load_glyphs(_font, cache);
}

TextLimits::~TextLimits() {
	delete_ff(_atlas);
	delete_ff(_font);
}

void TextLimits::tessellate_to_vertices(const std::string& text, const glm::vec2 &start_pos, std::vector<PosTexVertex>& text_vertices, glm::vec2& min, glm::vec2& max) {
  // Initialize the pen position.
  ftgl::vec2 pen = { { start_pos.x, start_pos.y } };

  // Loop over the letters in the text.
  text_vertices.clear();
  for (size_t i = 0; i < text.size(); ++i) { //
    ftgl::texture_glyph_t *glyph = ftgl::texture_font_get_glyph(_font, &text[i]);
    if (glyph != NULL) {
      // Get our kerning value.
      float kerning = 0.0f;
      if (i > 0) {
        kerning = ftgl::texture_glyph_get_kerning(glyph, &text[i - 1]);
      }
      // Advance our pen position by the kerning.
      pen.x += kerning;

      // Determine our texture bounds.
      float min_s = glyph->s0;
      float max_s = glyph->s1;
      float min_t = glyph->t1;
      float max_t = glyph->t0;

      // Determine our position bounds.
      float min_x = pen.x + glyph->offset_x;
      float min_y = pen.y - (glyph->height-glyph->offset_y);
      float max_x = min_x + glyph->width;
      float max_y = min_y + glyph->height;

      // Add vertices for the quad representing our letter.
      float dummy = 20.0f; // a dummy depth that is not used by the vertex and frag shaders.
      text_vertices.push_back( { min_x, min_y, dummy, min_s, min_t });
      text_vertices.push_back( { max_x, min_y, dummy, max_s, min_t });
      text_vertices.push_back( { max_x, max_y, dummy, max_s, max_t });
      text_vertices.push_back( { min_x, max_y, dummy, min_s, max_t });
      pen.x += glyph->advance_x;

      // Initialize our bounds with the first letter.
      if (i == 0) {
        min = glm::vec2(min_x, min_y);
        max = glm::vec2(max_x, max_y);
      }

      // Expand our bounds.
      if (min_x < min.x) {
        min.x = min_x;
      }
      if (max_x > max.x) {
        max.x = max_x;
      }
      if (min_y < min.y) {
        min.y = min_y;
      }
      if (max_y > max.y) {
        max.y = max_y;
      }
    }
  }
}

void TextLimits::tessellate_to_instances(const std::string& text, const glm::vec2 &translate1, float rotate, const glm::vec2& translate2, unsigned char state,
                                         std::vector<CharInstance>& instances, glm::vec2& min, glm::vec2& max) {
  // Initialize the pen position.
  ftgl::vec2 pen = { { translate1.x, translate1.y } };

  // Loop over the letters in the text.
  instances.resize(text.size());
  for (size_t i = 0; i < text.size(); ++i) { //
    ftgl::texture_glyph_t *glyph = ftgl::texture_font_get_glyph(_font, &text[i]);
    if (glyph != NULL) {
      // Get our kerning value.
      float kerning = 0.0f;
      if (i > 0) {
        kerning = ftgl::texture_glyph_get_kerning(glyph, &text[i - 1]);
      }
      // Advance our pen position by the kerning.
      pen.x += kerning;

      // Determine our texture bounds.
      float min_s = glyph->s0;
      float max_s = glyph->s1;
      float min_t = glyph->t1;
      float max_t = glyph->t0;

      // Determine our position bounds.
      float min_x = pen.x + glyph->offset_x;
      float min_y = pen.y - (glyph->height-glyph->offset_y);
      float max_x = min_x + glyph->width;
      float max_y = min_y + glyph->height;

      // Add vertices for the quad representing our letter.
      CharInstance& ci = instances[i];
      ci.set_scale(max_x - min_x, max_y - min_y);
      ci.set_translate1(min_x, min_y);
      ci.set_rotate(rotate);
      ci.set_translate2(translate2);
      ci.set_coord_s(min_s, max_s);
      ci.set_coord_t(min_t, max_t);
      ci.set_state(state);
      pen.x += glyph->advance_x;

      // Initialize our bounds with the first letter.
      if (i == 0) {
        min = glm::vec2(min_x, min_y);
        max = glm::vec2(max_x, max_y);
      }

      // Expand our bounds.
      if (min_x < min.x) {
        min.x = min_x;
      }
      if (max_x > max.x) {
        max.x = max_x;
      }
      if (min_y < min.y) {
        min.y = min_y;
      }
      if (max_y > max.y) {
        max.y = max_y;
      }
    }
  }
}

}
