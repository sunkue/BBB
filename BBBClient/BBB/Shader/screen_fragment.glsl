#version 450

uniform sampler2D screen_texture; 

in vec2 texcoord; 

out vec4 o_flagcolor;

void main() 
{ 
	o_flagcolor = vec4(texture(screen_texture, texcoord)); 
}

