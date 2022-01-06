#version 450

//

vec2 rotate(vec2 v, float radian)
{
	float s = sin(radian);
	float c = cos(radian);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

vec4 shear(vec4 v, float x)
{
	mat4 m = mat4(1);
	m[1][0] = x;
	m[1][2] = x;
	return m * v;
}

//

uniform float u_time;

//

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

// instance values
layout(location = 10) in float a_scale;
layout(location = 11) in float a_yaw;
layout(location = 12) in float a_shearseed;
layout(location = 13) in vec3 a_translate;

////


////

vec4 caculate_pos()
{
	//scale
	vec4 pos = vec4(a_position * a_scale, 1.f);

	//rotate
	vec2 rot = rotate(pos.xz, a_yaw);
	pos = vec4(rot.x, pos.y, rot.y, 1.f);

	//shear
	float seed = u_time + a_shearseed;
	float x = cos(seed) / 8;
	pos = shear(pos, x);

	//translate
	pos = vec4(pos.xyz + a_translate, 1.f);

	return pos;
}

///
