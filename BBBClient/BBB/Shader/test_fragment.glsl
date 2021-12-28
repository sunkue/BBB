
out vec4 o_flagcolor;

void main() 
{ 
	vec3 world_pos = fs_in.world_pos;
	vec3 normal = fs_in.normal;
	vec2 texcoord = fs_in.texcoord;
	vec4 lightspacefragpos = fs_in.lightspacefragpos;

	vec3 albedo = vec3(texture(u_material.albedo1, texcoord));
	vec3 specular_color = vec3(texture(u_material.specular1, texcoord));
	float shininess = 512;
	float shadow = caculate_shadow(lightspacefragpos, normal, u_directinal_light.direction);
	vec3 result = caculate_light(u_spot_light, normal, world_pos, albedo, specular_color, shininess, shadow);
	result = result + caculate_light(u_point_light, normal, world_pos, albedo, specular_color, shininess, shadow);
	result = result + caculate_light(u_directinal_light, normal, world_pos, albedo, specular_color, shininess, shadow);

	o_flagcolor = vec4(result, 1);
}

