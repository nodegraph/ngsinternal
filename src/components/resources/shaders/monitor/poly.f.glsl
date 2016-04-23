#version 330

in vec4 out_color;

// Outputs
layout(location=0) out vec4 output_color;


void main()
{
    output_color = out_color;
}
