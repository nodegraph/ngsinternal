#include <components/resources/resources.h>

#include <components/resources/dataheaders/vera_ttf.h>
#include <components/resources/dataheaders/verabd_ttf.h>
#include <components/resources/dataheaders/dejavusansmono_ttf.h>
#include <components/resources/dataheaders/displayshaders_gles2.h>
#include <components/resources/dataheaders/displayshaders_gles3.h>
#include <components/resources/textures/distance_map.h>

#include <base/utils/idgenerator.h>
#include <base/device/geometry/textlimits.h>

#include <base/utils/fileutil.h>
#include <base/memoryallocator/taggednew.h>
#include <components/entities/componentinstancer.h>
#include <components/entities/entityinstancer.h>

//#include <unistd.h>
#include <limits.h>
#include <iostream>

namespace ngs {

//std::string readTextFile(const std::string& filename) {
//  QFile file(QString::fromStdString(filename));
//  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//    return "ERROR_UNABLE_TO_FIND_FILE";
//  }
//  return file.readAll().toStdString();
//}
//
//std::string readBinaryFile(const std::string& filename) {
//  QFile file(QString::fromStdString(filename));
//  if (!file.open(QIODevice::ReadOnly)) {
//    return "ERROR_UNABLE_TO_FIND_FILE";
//  }
//  return file.readAll().toStdString();
//}


Resources::Resources(Entity* entity)
    : Component(entity, kIID(), kDID()) {

#if GLES_MAJOR_VERSION <= 2
  _quad_vs = std::string(monitor_poly_vert_gles2);
  _quad_fs = std::string(monitor_poly_frag_gles2);
  _text_vs = std::string(text_vert);
  _text_fs = std::string(text_frag_gles2);
#else
  std::string version;
#if (ARCH == ARCH_LINUX) || (ARCH == ARCH_MACOS)
  version = "#version 330\n";
#else
  version = "#version 300 es\n";
#endif
  _quad_vs = version+std::string(monitor_poly_vert_gles3);
  _quad_fs = version+std::string(monitor_poly_frag_gles3);
  _text_vs = version+std::string(text_vert_gles3);
  _text_fs = version+std::string(text_frag_gles3);
#endif

  //_text_font = std::string((const char*) Vera_ttf, Vera_ttf_len);
  _text_font = std::string((const char*) VeraBd_ttf, VeraBd_ttf_len);
  _icon_font = std::string((const char*) DejaVuSansMono_ttf, DejaVuSansMono_ttf_len);

  _id_generator = new_ff IDGenerator();
  _text_limits = new_ff TextLimits(_text_font);
}

Resources::~Resources() {
  delete_ff(_text_limits);
  delete_ff(_id_generator);
}

const std::string& Resources::get_text_font() const {
  return _text_font;
}

const std::string& Resources::get_icon_font() const {
  return _icon_font;
}

const std::string& Resources::get_quad_vertex_shader() const {
  return _quad_vs;
}
const std::string& Resources::get_quad_fragment_shader() const {
  return _quad_fs;
}

const std::string& Resources::get_text_vertex_shader() const {
  return _text_vs;
}
const std::string& Resources::get_text_fragment_shader() const {
  return _text_fs;
}
unsigned char* Resources::get_distance_map() const {
  return distance_map_raw;
}

size_t Resources::get_distance_map_size() const {
  return distance_map_raw_len;
}

TextLimits* Resources::get_text_limits() const {
  return _text_limits;
}

IDGenerator* Resources::get_id_generator() const {
  return _id_generator;
}

}
