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

vec4 gaussianBlur(sampler2D tex, vec2 uv)
{

#define  offset_x  1. / 1280
#define  offset_y  1. / 720

    vec2 offsets[9] = vec2[]
    (
        vec2(-offset_x,  offset_y), // top-left
        vec2( 0.0f,    offset_y), // top-center
        vec2( offset_x,  offset_y), // top-right
        vec2(-offset_x,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset_x,  0.0f),   // center-right
        vec2(-offset_x, -offset_y), // bottom-left
        vec2( 0.0f,   -offset_y), // bottom-center
        vec2( offset_x, -offset_y)  // bottom-right    
    );

	float kernel[9] = float[]
    (
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);
    
    vec4 sampleTex[9];

    for(int i = 0; i < 9; i++)
    {	
		vec4 pixel = texture(tex, uv.st + offsets[i]);
        sampleTex[i] = pixel;
    }
    vec4 col = vec4(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return col;
}

void main()
{    
	vec2 tc = texcoord;
	vec2 lightpos = u_texcoord_lightpos;
	vec2 delta_texcoord = tc.xy - lightpos;
	delta_texcoord *= u_density / NUM_SAMPLES;

	float illumination_decay = 1.0;

	vec3 result;
	//if(u_samples == 101)
	//	result = gaussianBlur(skypass_texture, tc).rgb * 0.4f;
	//else
		result = texture(skypass_texture ,tc).rgb * 0.4f;

	//if( lightpos.x > 0 && lightpos.x < 1 && lightpos.y > 0 && lightpos.y < 1) z 검사도 필요.
	if( 0 < u_light_dot_camera )
	{
		for( int i = 0; i < NUM_SAMPLES; i++)
		{
			tc -= delta_texcoord;
			vec3 sky_col = texture(skypass_texture ,tc).rgb;
			sky_col *= illumination_decay * u_weight;

			result += sky_col;

			illumination_decay *= u_decay;
		}

		result *= u_exposure;
	}

	o_flagcolor = vec4(result , 1);
}