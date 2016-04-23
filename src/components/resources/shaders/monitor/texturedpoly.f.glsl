#version 330

uniform vec4 tint;

out vec4 output_color;
in vec4 out_color;

varying vec2 out_tex_coord;
uniform sampler2D texture_sampler;

void main()
{
    output_color = tint * texture(texture_sampler, out_tex_coord);
}




