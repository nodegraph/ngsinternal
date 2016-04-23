#include <components/resources/dataheaders/es2_shaders.h>

namespace ngs {

const char * es2_poly_frag = "#version 100\n"
    "precision mediump float;\n"
    "uniform vec4 tint;\n"
    "varying vec4 out_color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = tint * vec4(out_color.rgba);\n"
    "}\n";

const char* es2_poly_vert = "#version 100\n"
    "precision mediump float;\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "attribute float comp_shape_id;\n"
    "attribute vec3 vertex;\n"
    "attribute vec4 color;\n"
    "varying vec4 out_color;\n"
    "void main()\n"
    "{\n"
    "    out_color = color;\n"
    "    vec4 warped = vec4(vertex,1.0);\n"
    "    warped.x += comp_shape_id*1.0;\n"
    "    gl_Position = projection*(modelview*warped);\n"
    "}\n";

}
