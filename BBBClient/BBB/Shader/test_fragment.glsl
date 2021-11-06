
void main() 
{ 
	vec3 albedo = vec3(texture(u_material.albedo, v_texcoord));
	vec3 specular_color = vec3(texture(u_material.specular, v_texcoord));
	float shininess = u_material.shininess;
	vec3 result = caculate_light(u_spot_light, albedo, specular_color, shininess);
	result = result + caculate_light(u_point_light, albedo, specular_color, shininess);
	result = result + caculate_light(u_directinal_light, albedo, specular_color, shininess);
	gl_FragColor = vec4(result, 1); 
}

