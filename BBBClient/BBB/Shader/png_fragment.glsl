#version 450




uniform sampler2D u_tex_sampler[4];

in VS_OUT
{
	vec3 normal;
	vec3 world_pos;
	vec2 texcoord;
	flat int texture_index;
} vs_in;

out vec4 o_flagcolor;

void main()
{
	vec3 c = vs_in.normal;
	c-= 0.5f;
		vec2 texcoord = vs_in.texcoord * 50.f;

	
	o_flagcolor = texture(u_tex_sampler[vs_in.texture_index], vs_in.texcoord);

	if(o_flagcolor.a < 0.1)
        discard;

	vec3 result = caculate_light(u_spot_light, gs_in.normal, gs_in.world_pos, o_flagcolor, o_flagcolor, shininess);
	result = result + caculate_light(u_point_light, gs_in.normal, gs_in.world_pos, o_flagcolor, o_flagcolor, shininess);
	//result = result + caculate_light(u_directinal_light, gs_in.normal, gs_in.world_pos, albedo, specular_color, shininess);
	o_flagcolor = vec4(result, 1); 
}
