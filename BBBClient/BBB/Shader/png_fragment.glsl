#version 450




uniform sampler2D u_tex_sampler[4];

in VS_OUT
{
	vec3 normal;
	vec2 texcoord;
	flat int texture_index;
} vs_in;

out vec4 o_flagcolor;

void main()
{
	vec3 c = vs_in.normal;
	c-= 0.5f;
		vec2 texcoord = vs_in.texcoord * 50.f;

	
	o_flagcolor = texture(u_tex_sampler[vs_in.texture_index], vs_in.texcoord) + vec4(0.0f);

	if(o_flagcolor.a < 0.1)
        discard;

	o_flagcolor += vec4(c, 1.0f)*0.01f;
}
