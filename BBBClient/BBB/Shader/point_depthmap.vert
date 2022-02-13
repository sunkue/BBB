#version 450

uniform mat4 u_m_mat;

layout(location = 0) in vec3 a_position;


void main()
{
	gl_Position = u_m_mat * vec4(a_position, 1.0);
}
