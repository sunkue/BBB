#version 450

in vec3 v_texcoord;

uniform samplerCube u_cubemap; 

out vec4 o_fragcolor;

void main() 
{ 
	o_fragcolor = texture(u_cubemap, v_texcoord); 
}


