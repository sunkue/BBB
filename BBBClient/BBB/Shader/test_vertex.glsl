#version 450

// 역행렬은 비싼연산, cpu에서 계산해서 유니폼으로 한번에 넘길 필요가 잇다.

layout(std140, binding = 0) uniform VP_MAT
{
	mat4 u_vp_mat;
};

// 여러개의 조명으로 만들 수 있다.
layout(std140, binding = 1) uniform LIGHTSPACE_MAT
{
	mat4 u_lightspace_mat;
};

uniform mat4 u_m_mat;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out VS_OUT
{
	vec4 lightspacefragpos;
	vec3 world_pos;
	vec3 normal;
	vec2 texcoord;
} vs_out;


void main()
{
	vs_out.world_pos = vec3(u_m_mat * vec4(a_position, 1.0f));
	vs_out.normal = mat3(transpose(inverse(u_m_mat))) * a_normal;
	vs_out.texcoord = a_texcoord;
	vs_out.lightspacefragpos = u_lightspace_mat * vec4(vs_out.world_pos, 1.0f);

	gl_Position = u_vp_mat * vec4(vs_out.world_pos, 1);
}
