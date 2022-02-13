#version 450

struct Material
{
	sampler2D albedo1;
	sampler2D specular1;
	float shininess;
};

layout(location = 0) out vec4 g_world_pos;
layout(location = 1) out vec4 g_normal;
layout(location = 2) out vec4 g_albedospec;

in VS_OUT
{
	vec3 world_pos;
	vec3 normal;
	vec2 texcoord;
} fs_in;

uniform Material u_material;

void main()
{
	vec3 world_pos = fs_in.world_pos;
	vec3 normal = fs_in.normal;
	vec2 texcoord = fs_in.texcoord;

	g_world_pos = vec4(world_pos, 0);
	g_normal = vec4(normal, 0);
	g_albedospec.rgb = texture(u_material.albedo1, texcoord).rgb;
	g_albedospec.a = texture(u_material.specular1, texcoord).r;
}

