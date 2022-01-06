
uniform mat4 u_lightpos_mat;

//flat out int texture_index;
//out vec2 texcoord;

void main()
{
	vec4 pos = caculate_pos();

	//texture_index = gl_InstanceID % 4;
	//texcoord = a_texcoord;

	gl_Position = u_lightpos_mat * pos;
}
