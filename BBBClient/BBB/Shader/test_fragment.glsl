
out vec4 o_flagcolor;

void main() 
{ 
	vec3 albedo = vec3(texture(u_material.albedo1, gs_in.texcoord));
	vec3 specular_color = vec3(texture(u_material.specular1, gs_in.texcoord));
	float shininess = 512;
	vec3 result = caculate_light(u_spot_light, gs_in.normal, gs_in.world_pos, albedo, specular_color, shininess);
	result = result + caculate_light(u_point_light, gs_in.normal, gs_in.world_pos, albedo, specular_color, shininess);
	//result = result + caculate_light(u_directinal_light, gs_in.normal, gs_in.world_pos, albedo, specular_color, shininess);
	o_flagcolor = vec4(result, 1); 
}

