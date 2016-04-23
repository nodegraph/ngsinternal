#version 330
 
uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 tint;

in vec3 vertex;
in vec2 tex_coord;
in vec4 color;

out vec2 out_tex_coord;
out vec4 out_color;


void main()
{
    out_tex_coord.xy  = tex_coord.xy;
    out_color         = tint*color;
    gl_Position       = projection*(modelview*vec4(vertex,1.0));
}
