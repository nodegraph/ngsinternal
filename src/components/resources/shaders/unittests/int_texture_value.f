#version 330

layout(std140) uniform Inputs
{
    mat4 model_view;
    mat4 projection;
    mat4 model_view_projection;
    uniform vec4 tint_color;
};
uniform isampler2D image_texture;

layout(location=0) out ivec4 output_color;

in VertexAttributes
{
    smooth vec2 tex_coord;
    smooth vec3 normal;
} attr_in;

void main()
{
    output_color=texture(image_texture,attr_in.tex_coord).xyzw;
    output_color+=ivec4(tint_color);
}


