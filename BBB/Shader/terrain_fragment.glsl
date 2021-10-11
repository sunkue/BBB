#version 450




uniform sampler2D u_tex_sampler;

varying vec3 v_normal;
varying vec2 v_texcoord;

void main()
{
	vec3 c = v_normal;
	c+=0.5f;
	gl_FragColor = texture(u_tex_sampler, v_texcoord) + vec4(0.1f);
	
	gl_FragColor += vec4(c, 1.0f)*0.1f;}
