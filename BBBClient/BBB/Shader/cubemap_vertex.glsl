#version 450

uniform mat4 u_vp_mat; 

layout (location = 0) in vec3 a_position;

out vec3 v_texcoord; 

void main() 
{ 
	v_texcoord = a_position; 
	vec4 pos = u_vp_mat * vec4(a_position, 1.0); 
	gl_Position = pos.xyww;
}
