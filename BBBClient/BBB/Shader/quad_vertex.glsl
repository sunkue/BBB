#version 450


layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord;
out vec2 texcoord;

void main()
{
    texcoord = a_texcoord;
    gl_Position = vec4(a_position.x, a_position.y, 0.0, 1.0); 
}  
