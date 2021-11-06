

struct Material 
{ 
	sampler2D albedo;
	sampler2D specular; 
	float shininess; 
}; 

struct LightBasic
{
	vec3 ambient; 
	vec3 diffuse; 
	vec3 specular;
};

struct DirectionalLight
{ 
	LightBasic power;
	vec3 direction;
}; 

struct PointLight
{ 
	LightBasic power;
	vec3 position;
	vec3 attenuation;
}; 

struct SpotLight
{ 
	LightBasic power;
	vec3 position;
	vec3 direction;
	vec3 attenuation;
	float in_cutoff;
	float out_cutoff;
}; 


uniform SpotLight u_spot_light;
uniform PointLight u_point_light;
uniform DirectionalLight u_directinal_light;

uniform Material u_material;
uniform vec3 u_view_pos;


varying vec3 v_world_pos;
varying vec3 v_normal;
varying vec2 v_texcoord;


float caculate_attenuation(vec3 light_pos,vec3 attenuation_param, vec3 world_pos)
{
	float dist = length(light_pos - world_pos); 
	float attenuation = 1 / 
	(attenuation_param.x + attenuation_param.y * dist + attenuation_param.z * (dist * dist));
	return attenuation;
}

vec3 caculate_light(DirectionalLight light, vec3 albedo, vec3 specular_color, float shininess)
{
	vec3 ambient = light.power.ambient * albedo; 
	//
	vec3 normal = normalize(v_normal); 
	vec3 light_dir = normalize(-light.direction);
	vec3 reflect_dir = reflect(-light_dir, normal);
	//
	float diff = max(dot(normal, light_dir), 0); 
	vec3 diffuse = light.power.diffuse * (diff * albedo);
	//
	vec3 view_dir = normalize(u_view_pos - v_world_pos); 
	float spec = pow(max(dot(view_dir, reflect_dir), 0), shininess);
	vec3 specular = light.power.specular * (spec * specular_color);
	//
	vec3 ret = (ambient + diffuse + specular);
	return ret;
}

vec3 caculate_light(PointLight light, vec3 albedo, vec3 specular_color, float shininess)
{
	vec3 ambient = light.power.ambient * albedo; 
	//
	vec3 normal = normalize(v_normal); 
	vec3 light_dir = normalize(light.position - v_world_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	//
	float diff = max(dot(normal, light_dir), 0); 
	vec3 diffuse = light.power.diffuse * (diff * albedo);
	//
	vec3 view_dir = normalize(u_view_pos - v_world_pos); 
	float spec = pow(max(dot(view_dir, reflect_dir), 0), shininess);
	vec3 specular = light.power.specular * (spec * specular_color);
	//
	float attenuation = caculate_attenuation(light.position, light.attenuation, v_world_pos);
	vec3 ret = (ambient + diffuse + specular) * attenuation;
	return ret;
}

vec3 caculate_light(SpotLight light, vec3 albedo, vec3 specular_color, float shininess)
{
	vec3 ambient = light.power.ambient * albedo;
	//
	vec3 light_dir = normalize(light.position - v_world_pos);
	float theta = dot(light_dir, normalize(-light.direction));
	float epsilon = light.in_cutoff - light.out_cutoff; 
	float intensity = clamp((theta - light.out_cutoff) / epsilon, 0, 1);
	vec3 normal = normalize(v_normal); 
	vec3 reflect_dir = reflect(-light_dir, normal);
	//
	float diff = max(dot(normal, light_dir), 0); 
	vec3 diffuse = light.power.diffuse * (diff * albedo) * intensity;
	//
	vec3 view_dir = normalize(u_view_pos - v_world_pos); 
	float spec = pow(max(dot(view_dir, reflect_dir), 0), shininess);
	vec3 specular = light.power.specular * (spec * specular_color) * intensity;
	//
	float attenuation = caculate_attenuation(light.position, light.attenuation, v_world_pos);
	vec3 ret = (ambient + diffuse + specular) * attenuation;
	return ret;
}

