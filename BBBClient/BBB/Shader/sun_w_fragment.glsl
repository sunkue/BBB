#version 450

uniform vec3 u_sun_dir;
uniform float u_sun_size;

in vec2 texcoord;

out vec4 o_flagcolor;

vec2 get_sun_point()
{
	vec2 ret;

	ret.x = 0.5;
	ret.y = 0.5;

	return ret;
}

float get_dist(vec2 p)
{
	float distx = distance(texcoord.x, p.x);
	float disty = distance(texcoord.y, p.y);

	float ret = max(distx, disty);
//	distx , disty
	
	return ret;
}

void main() 
{ 
	float dist = get_dist(get_sun_point());
	float inside = step(dist, u_sun_size);
	o_flagcolor = vec4(1, 1, 1, 1);
} 

