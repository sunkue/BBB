#version 450


uniform mat4 u_mvp_mat;

in vec3 a_position;
//in vec2 a_texpos;


//out vec2 v_TexPos;
out vec3 frag_c;

void main()
{	
	gl_Position = u_mvp_mat*vec4(a_position, 1.0f);

	//v_TexPos = a_texpos;
	frag_c = a_position;

}
