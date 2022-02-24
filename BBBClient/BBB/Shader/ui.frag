#version 450

uniform sampler2D u_texture; 

in vec2 texcoord; 

out vec4 o_flagcolor;

void main() 
{ 
    o_flagcolor = texture(u_texture, texcoord);
} 

