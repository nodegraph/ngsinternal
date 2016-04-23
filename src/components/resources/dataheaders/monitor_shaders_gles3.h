const char* text_vert = "#version 300 es\n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "uniform mat4 model_view_project;\n"
    " uniform mat4 model_view_project_selected;\n"
    " // vertex attributes\n"
    "in vec3 vertex;\n"
    "in vec2 tex_coord;\n"
    " // instance attributes\n"
    "in vec2 scale;\n"
    "in vec2 translate1;\n"
    "in float rotation;\n"
    "in vec2 translate2;\n"
    "in vec2 coord_s;\n"
    "in vec2 coord_t;\n"
    "in uint state;\n"
    " // vertex outputs\n"
    "out vec2 frag_tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main()\n"
    "{\n"
    "    if (tex_coord == vec2(0.0f,0.0f)) {"
    "        frag_tex_coord.x = coord_s.x;"
    "        frag_tex_coord.y = coord_t.x;"
    "    } else if (tex_coord == vec2(1.0f,0.0f)) {"
    "        frag_tex_coord.x = coord_s.y;"
    "        frag_tex_coord.y = coord_t.x;"
    "    } else if (tex_coord == vec2(1.0f,1.0f)) {"
    "        frag_tex_coord.x = coord_s.y;"
    "        frag_tex_coord.y = coord_t.y;"
    "    } else if (tex_coord == vec2(0.0f,1.0f)) {"
    "        frag_tex_coord.x = coord_s.x;"
    "        frag_tex_coord.y = coord_t.y;"
    "    }"
    "    vec2 warped = vec2 (vertex.x, vertex.y);"
    "    warped = warped*scale + translate1;"
    "    float rx = cos(rotation)*warped.x - sin(rotation)*warped.y;\n"
    "    float ry = sin(rotation)*warped.x + cos(rotation)*warped.y;\n"
    "    warped = vec2(rx,ry) + translate2;"
    "    frag_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
    "    float depth = 10.0;\n"
    "    if (state > uint(0)) {"
    "        gl_Position = model_view_project_selected*vec4(warped.x, warped.y, 20.0, 1.0);\n"
    "    } else {"
    "        gl_Position = model_view_project*vec4(warped.x, warped.y, 20.0, 1.0);\n"
    "    }"
    "}\n";

const char* text_frag = "#version 300 es\n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "uniform sampler2D distance_field;\n"
    "in vec2 frag_tex_coord;\n"
    "in vec4 frag_color;\n"
    "layout(location=0) out vec4 output_color;\n"
    "\n"
    "       vec3 glyph_color    = vec3(1.0,1.0,1.0);\n"
    "const float glyph_center   = 0.50;\n"
    "       vec3 outline_color  = vec3(0.0,0.0,0.0);\n"
    "const float outline_center = 0.55;\n"
    "       vec3 glow_color     = vec3(1.0,1.0,1.0);\n"
    "const float glow_center    = 1.25;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    vec4  color = texture(distance_field, frag_tex_coord);\n"
    "    float dist  = color.r;\n"
    "    float width = fwidth(dist);\n"
    "    float alpha = smoothstep(0.5-width, 0.5+width, dist);\n"
    "    output_color = vec4(frag_color.rgb, alpha*frag_color.a);\n"
    "    //output_color = vec4(frag_color.rgb, 0.001*alpha*frag_color.a+0.99);\n"
    "    //float test = 1.0f*dist;\n"
    "    //output_color = vec4(test,test,test, 0.001*alpha*frag_color.a+0.99);\n"
    "    //output_color = max(vec4(frag_color.rgb, alpha*frag_color.a),vec4(1.0,0.0,0.0,1.0));\n"
    "    //output_color.g = 0.0;\n"
    "\n"
    "    // Glow + outline\n"
    "    vec3 rgb = mix(glow_color, glyph_color, alpha);\n"
    "    float mu = smoothstep(glyph_center, glow_center, sqrt(dist));\n"
    "    color = vec4(rgb, max(alpha,mu));\n"
    "    float beta = smoothstep(outline_center-width, outline_center+width, dist);\n"
    "    rgb = mix(outline_color, color.rgb, beta);\n"
    "    output_color = vec4(rgb, max(color.a,beta));\n"
    "    if (output_color.a < 0.2) {\n"
    "        output_color.a = 0.0;\n"
    "    }\n"
    "\n"
    "}\n";

const char* monitor_poly_vert = "#version 300 es\n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "\n"
    "\n"
    "// Uniforms ----------------------\n"
    " uniform mat4 model_view_project;\n"
    " uniform mat4 model_view_project_selected;\n"
    "\n"
    "// Block Uniforms ----------------\n"
    "layout(std140) uniform Warpings\n"
    "{\n"
    "    vec2 warp;\n"
    "};\n"
    "\n"
    "\n"
    "// Vertex Attributes -------------\n"
    "in uint comp_shape_id;\n"
    "in vec3 vertex;\n"
    "in vec2 scale;\n"
    "in float rotate;\n"
    "in vec3 translate;\n"
    "in vec3 color;\n"
    "in uint state;\n"
    "\n"
    "\n"
    "// Outputs -----------------------\n"
    "out vec2 out_tex_coord;\n"
    "out vec4 out_color;\n"
    "\n"
    "\n"
    "// Program -----------------------\n"
    "void main()\n"
    "{\n"
    "    if (bool(state & 0x2u)) {"
    "        out_color = vec4(0.9f, 0.9f, 0.0f, 1.0f);"
    "    } else { "
    "        out_color = vec4(color,1.0f);\n"
    "    }"
    "    vec3 warped = vertex;\n"
    "    warped.x *= scale.x;\n"
    "    warped.y *= scale.y;\n"
    "    float rx = cos(rotate)*warped.x - sin(rotate)*warped.y;\n"
    "    float ry = sin(rotate)*warped.x + cos(rotate)*warped.y;\n"
    "    warped = vec3(rx, ry, warped.z) + translate;\n"
    "    if (bool(state & 0x1u)) {"
    "        gl_Position = model_view_project_selected*vec4(warped,1.0f);\n"
    "    } else {"
    "        gl_Position = model_view_project*vec4(warped,1.0f);\n"
    "    }"
    "}\n";

const char * monitor_poly_frag = "#version 300 es\n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "in vec4 out_color;\n"
    "layout(location=0) out vec4 output_color;\n"
    "void main()\n"
    "{\n"
    "    output_color = out_color;\n"
    "}\n";


