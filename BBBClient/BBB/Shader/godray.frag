#version 450


uniform vec2 u_texcoord_lightpos;
uniform float u_decay;
uniform float u_density;
uniform float u_samples;
uniform float u_weight;
uniform float u_exposure;
uniform float u_light_dot_camera;

uniform sampler2D skypass_texture;

in vec2 texcoord;

out vec4 o_flagcolor;

#define NUM_SAMPLES u_samples

void main()
{    
	vec2 tc = texcoord;
	vec2 lightpos = u_texcoord_lightpos;
	vec2 delta_texcoord = tc.xy - lightpos;
	delta_texcoord *= u_density / NUM_SAMPLES;

	float illumination_decay = 1.0;

	vec3 result = texture(skypass_texture ,tc).rgb * 0.4f;

	//if( lightpos.x > 0 && lightpos.x < 1 && lightpos.y > 0 && lightpos.y < 1) z 검사도 필요.
	if( 0.00001 < u_light_dot_camera )
	{
		for( int i = 0; i < NUM_SAMPLES; i++)
		{
			tc -= delta_texcoord;

			// 사물 뒤에 가려진 경우에, 좌표벗어난 곳은 사물이 안그려져있어서 빛줄기가 나와버림, if 제거.
			bvec2 lowerLimit = greaterThan(tc.xy, vec2(0));
			bvec2 upperLimit = lessThan(tc.xy, vec2(1));
			vec2 lL = vec2(lowerLimit);
			vec2 uL = vec2(upperLimit);
			float limit = lL.x * lL.y * uL.x * uL.y; 

			vec3 sky_col = texture(skypass_texture ,tc).rgb * limit;

			sky_col *= illumination_decay * u_weight;

			result += sky_col;

			illumination_decay *= u_decay;
		}

	}
	
	result *= u_exposure;

	o_flagcolor = vec4(result , 1);
}