#version 450

struct Material 
{ 
	vec3 ambient; 
	vec3 diffuse; 
	vec3 specular; 
	float shininess; 
}; 

uniform Material u_material;
uniform vec3 u_light_pos;
uniform vec3 u_view_pos;

varying vec3 v_world_pos;
varying vec3 v_normal;
varying vec2 v_texcoord;

void main() 
{ 
	vec3 lightColor = vec3(0.5);
	vec3 objectColor = vec3(v_texcoord, 1.0f);

	/////
	vec3 ambient = lightColor * u_material.ambient; 
	/////
	vec3 normal = normalize(v_normal); 
	vec3 lightDir = normalize(u_light_pos - v_world_pos);
	float diff = max(dot(normal, lightDir), 0.0); 
	vec3 diffuse = diff * u_material.diffuse;
	/////
	vec3 viewDir = normalize(u_view_pos - v_world_pos); 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
	vec3 specular =  spec * u_material.specular;
	/////
	
	vec3 result =  (ambient + diffuse + specular) * objectColor; 
	gl_FragColor = vec4(result, 1.0); 
}

