

layout(std140, binding = 1) uniform LIGHTSPACE_MAT
{
	mat4 u_lightspace_mat;
};

uniform sampler2D g_world_pos;
uniform sampler2D g_normal;
uniform sampler2D g_albedospec;

uniform sampler2D u_shadowmap;

uniform SpotLight u_spot_light;
uniform PointLight u_point_light;
uniform DirectionalLight u_directinal_light;

uniform vec3 u_view_pos;

in vec2 texcoord;

out vec4 o_flagcolor;

void main() 
{ 
	vec3 world_pos = texture(g_world_pos, texcoord).rgb;
	vec3 normal = texture(g_normal, texcoord).rgb;
	vec4 albedospec = texture(g_albedospec, texcoord);
	vec3 albedo = albedospec.rgb;
	float specular_color = albedospec.a;
	float shininess = 512;
	
	vec4 lightspacefragpos = u_lightspace_mat * vec4(world_pos, 1.0f);
	vec3 view_dir = normalize(u_view_pos - world_pos);

	float shadow = caculate_shadow(u_shadowmap, 1, lightspacefragpos, normal, u_directinal_light.direction);
	
	vec3 result = caculate_light(u_spot_light, view_dir, normal, world_pos, albedo, specular_color, shininess, 0);
	result = result + caculate_light(u_point_light, view_dir, normal, world_pos, albedo, specular_color, shininess, 0);
	result = result + caculate_light(u_directinal_light, view_dir, normal, world_pos, albedo, specular_color, shininess, shadow);

	o_flagcolor = vec4(result, 1);
} 

