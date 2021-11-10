




uniform sampler2D u_tex_sampler[4];

flat in int texture_index;

out vec4 o_flagcolor;

void main()
{	
	vec3 world_pos = fs_in.world_pos;
	vec3 normal = fs_in.normal;
	vec2 texcoord = fs_in.texcoord;

	o_flagcolor = texture(u_tex_sampler[texture_index], texcoord);
	if(o_flagcolor.a < 0.1)
        discard;
	float shininess = 512;
	vec3 result = caculate_light(u_spot_light, normal, world_pos, o_flagcolor.xyz, o_flagcolor.xyz, shininess);
	result = result + caculate_light(u_point_light, normal, world_pos, o_flagcolor.xyz, o_flagcolor.xyz, shininess);
	result = result + caculate_light(u_directinal_light, normal, world_pos, o_flagcolor.xyz, o_flagcolor.xyz, shininess);
	o_flagcolor = vec4(result, 1); 
}
