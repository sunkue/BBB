#version 450

uniform vec2 u_startpos; 

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord;

out vec2 texcoord;

void main()
{
    texcoord = a_texcoord;
    vec2 pos=  a_position + u_startpos;
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0); 
}
