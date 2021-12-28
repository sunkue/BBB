#version 450

struct Material
{
	sampler2D albedo1;
	sampler2D specular1;
	float shininess;
};

layout(location = 0) out vec3 g_position;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedospec;

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} fs_in;

uniform Material u_material;

void main()
{
	vec3 position = fs_in.position;
	vec3 normal = fs_in.normal;
	vec2 texcoord = fs_in.texcoord;

	g_position = position;
	g_normal = normal;
	g_albedospec.rgb = texture(u_material.albedo1, texcoord).rgb;
	g_albedospec.a = texture(u_material.specular1, texcoord).r;
}

