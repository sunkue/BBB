
layout(std140, binding = 0) uniform VP_MAT
{
	mat4 u_vp_mat;
};

out VS_OUT
{
	vec3 world_pos;
	vec3 normal;
	vec2 texcoord;
} vs_out;

flat out int texture_index;

void main()
{
	vec4 pos = caculate_pos();

	texture_index = gl_InstanceID % 4;

	vs_out.world_pos = pos.xyz;
	vs_out.normal = a_normal;
	vs_out.texcoord = a_texcoord;

	gl_Position = u_vp_mat * pos;
}