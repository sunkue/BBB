#version 450


layout (std140, binding = 0) uniform VP_MAT
{
	mat4 u_vp_mat;
};

uniform float u_time;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

// instance values
in float a_scale;
in float a_yaw;
in float a_shearseed;
in vec3 a_translate;

out VS_OUT
{
	vec3 normal;
	vec2 texcoord;
	flat int texture_index;
} vs_out;


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

void main()
{	
	//scale
	vec4 pos = vec4(a_position * a_scale, 1.f);
	
	//rotate
	vec2 rot = rotate(pos.xz, a_yaw);
	pos = vec4(rot.x , pos.y, rot.y, 1.f);
	
	//shear
	float seed = u_time + a_shearseed;
	float x = cos(seed) / 8;
	pos = shear(pos, x);

	//translate
	pos = vec4(pos.xyz + a_translate, 1.f);

	gl_Position = u_vp_mat * pos;
	vs_out.texture_index = gl_InstanceID;
	vs_out.normal = a_normal;	
	vs_out.texcoord = a_texcoord;
}
