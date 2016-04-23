
#version 330

layout(std140) uniform Inputs
{
    mat4 model_view;
    mat4 projection;
    mat4 model_view_projection;
    uniform vec4 tint_color;
};

in vec4 position;
in vec3 normal;
in vec2 tex_coord;

out VertexAttributes
{
    smooth vec2 tex_coord;
    smooth vec3 normal;
} attr_out;

void main()
{ 
    gl_Position = model_view_projection*position;
    attr_out.normal=normal;
    attr_out.tex_coord=tex_coord;
    
    //gl_Position = gl_ModelVieProjectionMatrix*position;
    //gl_Position=gl_ModelViewProjectionMatrix*gl_vertex;	
    //gl_Position=ftransform();
}
