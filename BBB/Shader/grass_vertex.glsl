#version 450


uniform mat4 u_mvp_mat;
uniform uint u_time;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec3 v_normal;
varying vec2 v_texcoord;

void main()
{	
	gl_Position = u_mvp_mat * vec4(a_position, 1.0f);


	v_normal = a_normal;
	
	vec2 new_texcoord;
	//float add = smoothstep(0.0f, 1.0f, sin(float(u_time) * x  / 50000.f) * a_texcoord.y /8); // ±òÂ¦
	//float add = smoothstep(0.0f, 0.5f, sin(float(u_time) * x  / 50000.f) * a_texcoord.y /8);
	float x = gl_Position.x;
	float ww = sin(float(u_time)/ 500.f + x);
	new_texcoord.x = a_texcoord.x + ww * a_texcoord.y /4;
	new_texcoord.y = a_texcoord.y;

	v_texcoord = new_texcoord;
}
