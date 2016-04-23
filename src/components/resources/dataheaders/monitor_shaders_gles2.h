const char* text_vert = "#version 100 \n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "uniform mat4 model_view_project;\n"
    " uniform mat4 model_view_project_selected;\n"
    " // vertex attributes\n"
    "attribute vec3 vertex;\n"
    "attribute vec2 tex_coord;\n"
    " // instance attributes\n"
    "attribute vec2 scale;\n"
    "attribute vec2 translate1;\n"
    "attribute float rotation;\n"
    "attribute vec2 translate2;\n"
    "attribute vec2 coord_s;\n"
    "attribute vec2 coord_t;\n"
    "attribute float state;\n"
    " // vertex outputs\n"
    "varying vec2 frag_tex_coord;\n"
    "varying vec4 frag_color;\n"
    "void main()\n"
    "{\n"
    "    if (tex_coord == vec2(0.0,0.0)) {"
    "        frag_tex_coord.x = coord_s.x;"
    "        frag_tex_coord.y = coord_t.x;"
    "    } else if (tex_coord == vec2(1.0,0.0)) {"
    "        frag_tex_coord.x = coord_s.y;"
    "        frag_tex_coord.y = coord_t.x;"
    "    } else if (tex_coord == vec2(1.0,1.0)) {"
    "        frag_tex_coord.x = coord_s.y;"
    "        frag_tex_coord.y = coord_t.y;"
    "    } else if (tex_coord == vec2(0.0,1.0)) {"
    "        frag_tex_coord.x = coord_s.x;"
    "        frag_tex_coord.y = coord_t.y;"
    "    }"
    "    vec2 warped = vec2 (vertex.x, vertex.y);"
    "    warped = warped*scale + translate1;"
    "    float rx = cos(rotation)*warped.x - sin(rotation)*warped.y;\n"
    "    float ry = sin(rotation)*warped.x + cos(rotation)*warped.y;\n"
    "    warped = vec2(rx,ry) + translate2;"
    "    frag_color = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "    float depth = 10.0;\n"
    "    if (state > 0.0) {"
    "        gl_Position = model_view_project_selected*vec4(warped.x, warped.y, 20.0, 1.0);\n"
    "    } else {"
    "        gl_Position = model_view_project*vec4(warped.x, warped.y, 20.0, 1.0);\n"
    "    }"
    "}\n";

const char* text_frag = "#version 100 \n"
    "#extension GL_OES_standard_derivatives : enable\n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "uniform sampler2D distance_field;\n"
    "varying vec2 frag_tex_coord;\n"
    "varying vec4 frag_color;\n"
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
    "    vec4  color = texture2D(distance_field, frag_tex_coord);\n"
    "    float dist  = color.r;\n"
    "    float width = fwidth(dist);\n"
    "    float alpha = smoothstep(0.5-width, 0.5+width, dist);\n"
    "    gl_FragColor = vec4(frag_color.rgb, alpha*frag_color.a);\n"
    "    //gl_FragColor = vec4(frag_color.rgb, 0.001*alpha*frag_color.a+0.99);\n"
    "    //float test = 1.0*dist;\n"
    "    //gl_FragColor = vec4(test,test,test, 0.001*alpha*frag_color.a+0.99);\n"
    "    //gl_FragColor = max(vec4(frag_color.rgb, alpha*frag_color.a),vec4(1.0,0.0,0.0,1.0));\n"
    "    //gl_FragColor.g = 0.0;\n"
    "\n"
    "    // Glow + outline\n"
    "    vec3 rgb = mix(glow_color, glyph_color, alpha);\n"
    "    float mu = smoothstep(glyph_center, glow_center, sqrt(dist));\n"
    "    color = vec4(rgb, max(alpha,mu));\n"
    "    float beta = smoothstep(outline_center-width, outline_center+width, dist);\n"
    "    rgb = mix(outline_color, color.rgb, beta);\n"
    "    gl_FragColor = vec4(rgb, max(color.a,beta));\n"
    "    if (gl_FragColor.a < 0.2) {\n"
    "        gl_FragColor.a = 0.0;\n"
    "    }\n"
    "\n"
    "}\n";

const char* monitor_poly_vert = "#version 100 \n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "\n"
    "\n"
    "// Uniforms ----------------------\n"
    " uniform mat4 model_view_project;\n"
    " uniform mat4 model_view_project_selected;\n"
    "\n"
    "\n"
    "\n"
    "// Vertex Attributes -------------\n"
    "attribute vec3 vertex;\n"
    "attribute vec2 scale;\n"
    "attribute float rotate;\n"
    "attribute vec3 translate;\n"
    "attribute vec3 color;\n"
    "attribute float state;\n"
    "\n"
    "\n"
    "// Outputs -----------------------\n"
    "varying vec2 out_tex_coord;\n"
    "varying vec4 out_color;\n"
    "\n"
    "\n"
    "// Program -----------------------\n"
    "void main()\n"
    "{\n"
    "    if (bool(state > 2.0)) {"
    "        out_color = vec4(0.9, 0.9, 0.0, 1.0);"
    "    } else { "
    "        out_color = vec4(color,1.0);\n"
    "    }"
    "    vec3 warped = vertex;\n"
    "    warped.x *= scale.x;\n"
    "    warped.y *= scale.y;\n"
    "    float rx = cos(rotate)*warped.x - sin(rotate)*warped.y;\n"
    "    float ry = sin(rotate)*warped.x + cos(rotate)*warped.y;\n"
    "    warped = vec3(rx, ry, warped.z) + translate;\n"
    "    if (bool(state > 1.0)) {"
    "        gl_Position = model_view_project_selected*vec4(warped,1.0);\n"
    "    } else {"
    "        gl_Position = model_view_project*vec4(warped,1.0);\n"
    "    }"
    "}\n";

const char * monitor_poly_frag = "#version 100 \n"
    "precision mediump float;\n"
    "precision mediump int;\n"
    "varying vec4 out_color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = out_color;\n"
    "}\n";


