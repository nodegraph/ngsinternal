#version 330

// Uniforms
layout(std140) uniform Inputs
{
    mat4 model_view;
    mat4 projection;
    mat4 model_view_projection;
    uniform vec4 tint_color;
};
uniform sampler2D image_texture;

// Outputs
layout(location=0) out vec4 output_color;

// Inputs
in VertexAttributes
{
    smooth vec3 normal;
    smooth vec2 tex_coord;
} attr_in;

void main()
{
    vec4 temp=texture(image_texture,attr_in.tex_coord).xyzw;
    output_color=temp+tint_color;
}


