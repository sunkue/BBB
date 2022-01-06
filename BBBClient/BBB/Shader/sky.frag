#version 450

layout(std140) uniform RESOLUTION
{
	uniform vec2 u_resolution; // w,h
};

layout(std140) uniform INV_P_MAT
{
	uniform mat4 u_inv_proj;
};

layout(std140) uniform INV_V_MAT
{
	uniform mat4 u_inv_view;
};

uniform vec3 u_skycolor_bottom;
uniform vec3 u_skycolor_top;

uniform vec3 u_sun_dir;
uniform vec3 u_sun_color;

in vec3 texcoord;

out vec4 o_flagcolor;

vec3 computeClipSpaceCoord(ivec2 fragCoord)
{
	vec2 ray_nds = 2.0*vec2(fragCoord.xy)/u_resolution.xy - 1.0;
	return vec3(ray_nds, 1.0);
}

vec2 computeScreenPos(vec2 ndc)
{
	return (ndc*0.5 + 0.5);
}

vec3 computeSun(vec3 d, float powExp)
{
	float sun = clamp( dot(-u_sun_dir, d), 0.0, 1.0 );
	vec3 col = 0.8 * u_sun_color * pow( sun, powExp );
	return col;
}

vec4 colorCubeMap(vec3 d)
{
	vec3 col = mix(u_skycolor_bottom, u_skycolor_top, 
	clamp(1 - exp(8.5-17.*clamp(normalize(d).y * 0.5 + 0.6,0.0,1.0)),0.0,1.0));
	
	//vec3 col = vec3(0.6,0.71,0.85) - 0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;

	col +=  computeSun(d, 350.0);

	return vec4(col, 1.0);
}

void main()
{    
	ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	vec4 ray_clip = vec4(computeClipSpaceCoord(fragCoord), 1.0);
	vec4 ray_view = u_inv_proj * ray_clip;
	ray_view = vec4(ray_view.xy, -1.0, 0.0);
	vec3 world_dir = (u_inv_view * ray_view).xyz;
	world_dir = normalize(world_dir);

	vec4 bg = colorCubeMap(world_dir);

	o_flagcolor = vec4(bg.rgb,1.0);
}