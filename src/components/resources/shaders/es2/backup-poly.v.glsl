#version 100
 
precision mediump float;
 
uniform mat4 modelview;
uniform mat4 projection;

attribute vec3 vertex;
attribute vec2 tex_coord;
attribute vec4 color;

varying vec2 out_tex_coord;
varying vec4 out_color;


void main()
{
    out_tex_coord.xy  = tex_coord.xy;
    out_color         = color;
    gl_Position       = projection*(modelview*vec4(vertex,1.0));
}
