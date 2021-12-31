#version 450

layout(location = 0) out vec3 g_world_pos;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedospec;

in VS_OUT
{
	vec3 world_pos;
	vec3 normal;
	vec2 texcoord;
} fs_in;

uniform sampler2D u_tex_sampler[4];

flat in int texture_index;


void main()
{	
	vec3 world_pos = fs_in.world_pos;
	vec3 normal = fs_in.normal;
	vec2 texcoord = fs_in.texcoord;

	vec4 albedo = texture(u_tex_sampler[texture_index], texcoord);
	if(albedo.a < 0.1)
        discard;

	g_world_pos = world_pos;
	g_normal = normal;
	g_albedospec.rgb = albedo.rgb;
	g_albedospec.a = 0;
}
