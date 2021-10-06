#version 450




//uniform sampler2D u_TexSampler;

//in vec2 v_TexPos;
in vec3 frag_c;

//out vec4 FragColor;

void main()
{
	//vec2 tex = v_TexPos;
	//FragColor = texture(u_TexSampler, tex) + vec4(0.1f);
	//FragColor = vec4(frag_c, 1.0f);
	gl_FragColor = vec4(frag_c, 1.0f);
}
