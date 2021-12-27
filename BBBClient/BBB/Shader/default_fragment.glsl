#version 450


struct Material 
{ 
	sampler2D albedo1;
	sampler2D specular1; 
	float shininess; 
}; 

uniform Material u_material;

in VS_OUT
{
	vec4 lightspacefragpos;
	vec3 world_pos;
	vec3 normal;
    vec2 texcoord;
} fs_in;

out vec4 o_flagcolor;

void main()
{	
	o_flagcolor = texture(u_material.albedo1,fs_in.texcoord);
}
