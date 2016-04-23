#pragma once
#include <base/device/device_export.h>
#include <base/device/transforms/glmhelper.h>


#include <string>
#include <vector>

namespace ftgl {
struct texture_font_t;
struct texture_atlas_t;
}


namespace ngs {

struct CharInstance;
struct PosTexVertex;

// This class is mainly used to tessellate the text
// on the cpu side.
class DEVICE_EXPORT TextLimits {
public:
	TextLimits(const std::string& font_data);
	~TextLimits();
  void tessellate_to_vertices(const std::string& text, const glm::vec2 &start_pos, std::vector<PosTexVertex>& text_vertices, glm::vec2& min, glm::vec2& max);
  void tessellate_to_instances(const std::string& text, const glm::vec2 &translate1, float rotate, const glm::vec2& translate2, unsigned char state,
                               std::vector<CharInstance>& instances, glm::vec2& min, glm::vec2& max);
  ftgl::texture_font_t *_font;
	ftgl::texture_atlas_t *_atlas;
};

}
