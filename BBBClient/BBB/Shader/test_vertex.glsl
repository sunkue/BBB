#version 450

// 역행렬은 비싼연산, cpu에서 계산해서 유니폼으로 한번에 넘길 필요가 잇다.

uniform mat4 u_vp_mat;
uniform mat4 u_m_mat;
uniform vec3 u_light_pos;
uniform vec3 u_view_pos;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

out vec3 v_world_pos;
out vec3 v_normal;
out vec2 v_texcoord;

void main()
{	
	v_world_pos = vec3(u_m_mat * vec4(a_position, 1.0f));
	v_normal = mat3(transpose(inverse(u_m_mat))) * a_normal;
	v_texcoord = a_texcoord;

	gl_Position = u_vp_mat * vec4(v_world_pos, 1);
}
