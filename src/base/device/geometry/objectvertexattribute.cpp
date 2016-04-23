#include <base/device/deviceheadersgl.h>
#include <base/device/geometry/objectvertexattribute.h>


namespace ngs {

const ElementSource PosTexVertex::position_source = {3, GL_FLOAT, false, sizeof(PosTexVertex), 0, 0};
const ElementSource PosTexVertex::tex_coord_source = {2, GL_FLOAT, false, sizeof(PosTexVertex), (void*) (3 * sizeof(float)), 0};

}
