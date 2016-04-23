#version 100
precision mediump float;

uniform vec4 tint;

varying vec4 out_color;

void main()
{
    gl_FragColor = tint * vec4(out_color.rgba);
}
