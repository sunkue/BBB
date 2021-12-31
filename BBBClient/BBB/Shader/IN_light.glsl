#version 450

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

float caculate_attenuation(vec3 light_pos, vec3 attenuation_param, vec3 world_pos)
{
	float dist = length(light_pos - world_pos);
	float attenuation = 1 /
		(attenuation_param.x + attenuation_param.y * dist + attenuation_param.z * (dist * dist));
	return attenuation;
}

float caculate_shadow(sampler2D shadowmap, int samplelevel, vec4 lightspacefragpos, vec3 normal, vec3 lightdir)
{
	// perform perspective divide 
	vec3 projCoords = lightspacefragpos.xyz / lightspacefragpos.w;
	// transform to [0,1] range 
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range frag
	// PosLight as coords) 
	float closestDepth = texture(shadowmap, projCoords.xy).r;
	// get depth of current fragment from light's perspective 
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow 
	// need bias. to avoid moire
	
	float shadow = 0.0; 

	vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
	float bias = max(0.000002 * (1.0 - dot(normal, lightdir)), 0.00005);
	
	for (int x = -samplelevel; x <= samplelevel; ++x)
	{ 
		for (int y = -samplelevel; y <= samplelevel; ++y)
		{ 
			float pcfDepth = texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
		} 
	} 
	shadow /= pow(1 + samplelevel * 2, 2);

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

vec3 caculate_light(DirectionalLight light, vec3 view_dir, vec3 normal, vec3 world_pos, vec3 albedo, float specular_color, float shininess, float shadow)
{
	vec3 ambient = light.power.ambient * albedo;
	//
	normal = normalize(normal);
	vec3 light_dir = normalize(-light.direction);
	//
	float diff = max(dot(normal, light_dir), 0);
	vec3 diffuse = light.power.diffuse * (diff * albedo);
	//
	vec3 halfway_dir = normalize(light_dir + view_dir);
	float spec = pow(max(dot(normal, halfway_dir), 0), shininess);
	vec3 specular = light.power.specular * (spec * specular_color);
	//
	vec3 ret = (ambient + (1.0 - shadow) * (diffuse + specular));
	return ret;
}

vec3 caculate_light(PointLight light, vec3 view_dir, vec3 normal, vec3 world_pos, vec3 albedo, float specular_color, float shininess, float shadow)
{
	vec3 ambient = light.power.ambient * albedo;
	//
	normal = normalize(normal);
	vec3 light_dir = normalize(light.position - world_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	//
	float diff = max(dot(normal, light_dir), 0);
	vec3 diffuse = light.power.diffuse * (diff * albedo);
	//
	float spec = pow(max(dot(view_dir, reflect_dir), 0), shininess);
	vec3 specular = light.power.specular * (spec * specular_color);
	//
	float attenuation = caculate_attenuation(light.position, light.attenuation, world_pos);
	vec3 ret = (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation;
	return ret;
}

vec3 caculate_light(SpotLight light, vec3 view_dir, vec3 normal, vec3 world_pos, vec3 albedo, float specular_color, float shininess, float shadow)
{
	vec3 ambient = light.power.ambient * albedo;
	//
	vec3 light_dir = normalize(light.position - world_pos);
	float theta = dot(light_dir, normalize(-light.direction));
	float epsilon = light.in_cutoff - light.out_cutoff;
	float intensity = clamp((theta - light.out_cutoff) / epsilon, 0, 1);
	normal = normalize(normal);
	vec3 reflect_dir = reflect(-light_dir, normal);
	//
	float diff = max(dot(normal, light_dir), 0);
	vec3 diffuse = light.power.diffuse * (diff * albedo) * intensity;
	//
	float spec = pow(max(dot(view_dir, reflect_dir), 0), shininess);
	vec3 specular = light.power.specular * (spec * specular_color) * intensity;
	//
	float attenuation = caculate_attenuation(light.position, light.attenuation, world_pos);
	vec3 ret = (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation;
	return ret;
}

