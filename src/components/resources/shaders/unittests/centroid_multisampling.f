#version 330

// Uniforms
layout(std140) uniform Inputs
{
    mat4 model_view;
    mat4 projection;
    mat4 model_view_projection;
};
uniform sampler2D image_texture;

// Outputs
layout(location=0) out vec4 output_color;

// Inputs
in VertexAttributes
{
    smooth centroid vec3 normal;
    smooth centroid vec2 tex_coord;
} attr_in;

void main()
{
    output_color=texture(image_texture,attr_in.tex_coord).xyzw;

    // Debugging
    /*
    output_color=vec4(0.0);
    if(smooth_tex_coord.x<0.0)
    {
        output_color+=vec4(-10,0,0,0);
    }
    if(smooth_tex_coord.y<0.0)
    {
        output_color+=vec4(0,smooth_tex_coord.y,0,0);
    }
    if(smooth_tex_coord.x>1.0)
    {
        output_color+=vec4(0,0,10,0);
    }
    if(smooth_tex_coord.y>1.0)
    {
        output_color+=vec4(0,0,0,10);
    }
    */
}


