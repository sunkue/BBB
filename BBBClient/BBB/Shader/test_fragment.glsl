
void main() 
{ 
	vec3 albedo = vec3(texture(u_material.albedo1, v_texcoord));
	vec3 specular_color = vec3(texture(u_material.specular1, v_texcoord));
	float shininess = 32;
	vec3 result = caculate_light(u_spot_light, albedo, specular_color, shininess);
	result = result + caculate_light(u_point_light, albedo, specular_color, shininess);
	result = result + caculate_light(u_directinal_light, albedo, specular_color, shininess);
	gl_FragColor = vec4(result, 1); 
}

