#pragma once
#include <base/objectmodel/component.h>
#include <components/resources/resources_export.h>
#include <base/objectmodel/componentids.h>


namespace ngs {

class IDGenerator;
class TextLimits;
class EntityInstancer;
class ComponentInstancer;

class RESOURCES_EXPORT Resources: public Component  {
 public:

  COMPONENT_ID(Resources, Resources);

  Resources(Entity* entity);
  ~Resources();

  const std::string& get_text_font() const;
  const std::string& get_icon_font() const;

  const std::string& get_quad_vertex_shader() const;
  const std::string& get_quad_fragment_shader() const;

  const std::string& get_text_vertex_shader() const;
  const std::string& get_text_fragment_shader() const;
  unsigned char* get_distance_map() const;
  size_t get_distance_map_size() const;

  TextLimits* get_text_limits() const;

  IDGenerator* get_id_generator() const;

 private:

  // Font related data.
  std::string _text_font; // contains binary data
  std::string _icon_font; // contains binary data

  std::string _quad_vs; // contains text
  std::string _quad_fs;// contains text

  std::string _text_vs; // contains text
  std::string _text_fs;// contains text

  // We own these objets.
  IDGenerator* _id_generator;
  TextLimits* _text_limits;
};

}

