#version 330

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 30) out;

in VertexAttributes
{
    vec3 normal;
    vec2 tex_coord;
} attr_in[];

out VertexAttributes
{
    smooth vec3 normal;
    smooth vec2 tex_coord;
} attr_out;

void main()
{
    for(int i=0; i<3; i++)
    {
        gl_Position=gl_in[i].gl_Position;
        attr_out.normal=attr_in[i].normal;
        attr_out.tex_coord=attr_in[i].tex_coord;
        EmitVertex();
    }

    EndPrimitive();
}
