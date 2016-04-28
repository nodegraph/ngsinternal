#include <base/memoryallocator/taggednew.h>
#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/utils/simplesaver.h>
#include <base/utils/archdebug.h>

#include <base/device/program/defaultuniforminfos.h>
#include <base/device/program/attributeinfos.h>
//#include <base/device/geometry/distfieldtext.h>
//#include <base/device/geometry/distfieldtext.cpp>

#include <base/device/geometry/quad.h>
#include <base/device/geometry/elementsource.h>
#include <base/device/pipelinesetups/textpipelinesetup.h>

#include <components/resources/resources.h>

// freetype-gl texture map related
#include <freetype-gl/texture-font.h>
#include <freetype-gl/texture-atlas.h>
#include <freetype-gl/edtaa3func.h>
#include <string.h>
#include <fstream>

namespace {

using namespace ftgl;

unsigned char *
make_distance_map( unsigned char *img,
                   unsigned int width, unsigned int height )
{
    short * xdist = (short *)  malloc( width * height * sizeof(short) );
    short * ydist = (short *)  malloc( width * height * sizeof(short) );
    double * gx   = (double *) calloc( width * height, sizeof(double) );
    double * gy      = (double *) calloc( width * height, sizeof(double) );
    double * data    = (double *) calloc( width * height, sizeof(double) );
    double * outside = (double *) calloc( width * height, sizeof(double) );
    double * inside  = (double *) calloc( width * height, sizeof(double) );
    unsigned int i;

    // Convert img into double (data)
    double img_min = 255, img_max = -255;
    for( i=0; i<width*height; ++i)
    {
        double v = img[i];
        data[i] = v;
        if (v > img_max) img_max = v;
        if (v < img_min) img_min = v;
    }
    // Rescale image levels between 0 and 1
    for( i=0; i<width*height; ++i)
    {
        data[i] = (img[i]-img_min)/img_max;
    }

    // Compute outside = edtaa3(bitmap); % Transform background (0's)
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
    for( i=0; i<width*height; ++i)
        if( outside[i] < 0 )
            outside[i] = 0.0;

    // Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
    memset(gx, 0, sizeof(double)*width*height );
    memset(gy, 0, sizeof(double)*width*height );
    for( i=0; i<width*height; ++i)
        data[i] = 1 - data[i];
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, width, height, xdist, ydist, inside);
    for( i=0; i<width*height; ++i)
        if( inside[i] < 0 )
            inside[i] = 0.0;

    // distmap = outside - inside; % Bipolar distance field
    unsigned char *out = (unsigned char *) malloc( width * height * sizeof(unsigned char) );
    for( i=0; i<width*height; ++i)
    {
        outside[i] -= inside[i];
        outside[i] = 128+outside[i]*16;
        if( outside[i] < 0 ) outside[i] = 0;
        if( outside[i] > 255 ) outside[i] = 255;
        out[i] = 255 - (unsigned char) outside[i];
        //out[i] = (unsigned char) outside[i];
    }

    free( xdist );
    free( ydist );
    free( gx );
    free( gy );
    free( data );
    free( outside );
    free( inside );
    return out;
}
}

namespace ngs {

const glm::vec4 TextPipelineSetup::kClearColor(3.0f/255.0f, 169.0f/255.0f, 244.0f/255.0f, 1.00 );

TextPipelineSetup::TextPipelineSetup(const std::string& vertex_shader, const std::string& fragment_shader,
                                     const std::string& font_data, const unsigned char* distance_map, size_t distance_map_size)
    : PipelineSetup(vertex_shader, fragment_shader),
      _mvp_uniform(NULL),
      _selected_mvp_uniform(NULL),
      _distance_texture_uniform(NULL),
      _quad(NULL),
      _instances_vbo(NULL),
      _num_instances(0),
      _distance_map(distance_map),
      _distance_map_size(distance_map_size){

  _atlas = ftgl::texture_atlas_new(724,724,1);
  _font = ftgl::texture_font_new_from_memory(_atlas, 96, &font_data[0], font_data.size()); // original pt_size=48

  // We create the glyphs on the system side for all the letters we would ever need.
  const char *cache = " !\"#$%&'()*+,-./0123456789:;<=>?"
                         "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                         "`abcdefghijklmnopqrstuvwxyz{|}~";
  texture_font_load_glyphs( _font, cache );
}

TextPipelineSetup::~TextPipelineSetup() {
  delete_ff(_quad);
  delete_ff(_instances_vbo);
}

void TextPipelineSetup::initialize_gl_resources() {
  assert(_mvp_uniform == NULL);
  _mvp_uniform = _default_uniform_infos->get_uniform_info("model_view_project");
  _selected_mvp_uniform = _default_uniform_infos->get_uniform_info("model_view_project_selected");
  _distance_texture_uniform = _default_uniform_infos->get_uniform_info("distance_field");
  _quad = new_ff Quad();
  _instances_vbo = new_ff VertexBuffer();
  _num_instances = 0;
  feed_elements_to_attrs();

  // Create our distance map.
  glBindTexture(GL_TEXTURE_2D, _atlas->id);

  unsigned char *map = NULL;
  // Create our distance map dynamically.
#if BAKE_OUT_DISTANCE_MAP
    map = make_distance_map(_atlas->data, _atlas->width, _atlas->height);
  // Bake out a distance map.
    std::ofstream ofs;
    open_output_file_stream("./distance_map.raw", ofs);
    SimpleSaver ss(ofs);
    ss.save_raw(map, _atlas->width * _atlas->height * sizeof(unsigned char));
    free(map);
    map = NULL;
#endif
  // Load a prebaked distance map.
  {
    memcpy(_atlas->data, _distance_map, _distance_map_size);
  }

  // This assumes the glyphs for all the letters we would
  // ever need have been created on the system side, before
  // the device memory is initialized.
  // Technically we only need to load this texture in the root group.
  // All other groups will use the same texture.
  texture_atlas_upload(_atlas);

}

void TextPipelineSetup::set_mvp(const glm::mat4& m) {
  _default_uniform_infos->set_uniform(_mvp_uniform, &m[0][0]);
}

void TextPipelineSetup::set_selected_mvp(const glm::mat4& m) {
  _default_uniform_infos->set_uniform(_selected_mvp_uniform, &m[0][0]);
}

void TextPipelineSetup::set_distance_texture_unit(int texture_unit) {
  _default_uniform_infos->set_uniform(_distance_texture_uniform, &texture_unit);
}

void TextPipelineSetup::feed_elements_to_attrs() {
  use();
  _quad->bind();

  //debug
//  for (const std::string& s: _attribute_infos->get_attribute_names()) {
//    std::cerr << "attr name: " << s << "\n";
//  }

  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("vertex");
    _quad->feed_positions_to_attr(attr_loc);
  }
  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("tex_coord");
    _quad->feed_tex_coords_to_attr(attr_loc);
  }

  _instances_vbo->bind();
  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("scale");
    _instances_vbo->set_elements_to_feed_to_float_vertex_attribute(CharInstance::scale_source, attr_loc);
  }
  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("translate1");
    _instances_vbo->set_elements_to_feed_to_float_vertex_attribute(CharInstance::translation1_source, attr_loc);
  }
  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("rotation");
    _instances_vbo->set_elements_to_feed_to_float_vertex_attribute(CharInstance::rotation_source, attr_loc);
  }
  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("translate2");
    _instances_vbo->set_elements_to_feed_to_float_vertex_attribute(CharInstance::translation2_source, attr_loc);
  }
  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("coord_s");
    _instances_vbo->set_elements_to_feed_to_float_vertex_attribute(CharInstance::coord_s_source, attr_loc);
  }
  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("coord_t");
    _instances_vbo->set_elements_to_feed_to_float_vertex_attribute(CharInstance::coord_t_source, attr_loc);
  }

  {
    GLuint attr_loc = _attribute_infos->get_attribute_location("state");
    _instances_vbo->set_elements_to_feed_to_float_vertex_attribute(CharInstance::state_source, attr_loc);
  }
}

void TextPipelineSetup::draw() {
  _quad->bind();

  // Since we are drawing in Qt's render thread and need to make sure we restore all gl state.
  // It's better if we don't enable blending for now.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable texture unit zero.
  Device::enable_texture_target(0, GL_TEXTURE_2D);

  // Bind the texture to unit zero.
  glBindTexture( GL_TEXTURE_2D, _atlas->id );

  // Set the texture unit uniform.
  int texture_unit = 0;
  set_distance_texture_unit(texture_unit);

  // Now draw.
  _instances_vbo->bind();
  _instances_vbo->bind_positions();

  _quad->draw_instances(_num_instances);
  _quad->unbind();
}

void TextPipelineSetup::load_char_instances(const std::vector<CharInstance>& instances) {
  _num_instances = instances.size();
  _instances_vbo->bind();
  if (!instances.empty()) {
    _instances_vbo->load(sizeof(CharInstance) * _num_instances, &instances[0]);
  }
}

}
