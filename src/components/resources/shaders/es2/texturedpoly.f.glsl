#version 100
precision mediump float;

varying vec2 out_tex_coord;
uniform sampler2D texture_sampler;

uniform vec4 tint;
varying vec4 out_color;

void main()
{
    gl_FragColor = tint * texture(texture_sampler, out_tex_coord);
}
