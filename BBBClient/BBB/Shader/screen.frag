#version 450

uniform sampler2D screen_texture; 
uniform sampler2D bg_texture; 
uniform sampler2D n_texture;

in vec2 texcoord; 

out vec4 o_flagcolor;


vec4 origin()
{
    vec4 bg = texture(bg_texture,texcoord) * texture(n_texture,texcoord).a;

	return texture(screen_texture, texcoord) + bg;
}

vec4 inversion()
{
	return vec4(vec3(vec4(1) - texture(screen_texture, texcoord)), 1);
}

vec4 gray_scale()
{
	vec4 color = texture(screen_texture, texcoord);
    float average = (color.r + color.g + color.b) / 3.0;
    return vec4(average, average, average, 1.0);
}

vec4 gray_scale2()
{
	vec4 color = texture(screen_texture, texcoord);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return vec4(average, average, average, 1.0);
}

// 커널들은 합계가 1이다. 꼭9개일 필요는 없긴하다.
const float sharpen[9] = float[]
(
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);
 
const float sharpen2[9] = float[]
(
    -1, -4, -1,
    -4,  20, -4,
    -1, -4, -1
);
 
float blur[9] = float[]
(
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float blur2[9] = float[]
(
    0,      1.0/9, 0,
    1.0/9,  5.0/9, 1.0/9,
    0,      1.0/9, 0
);

const float edge[9] = float[]
(
    1, 1, 1,
    1,-8, 1,
    1, 1, 1
);


vec4 kerneling(float kernel[9])
{
	const float offset = 1.0 / 300.0;  

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 좌측 상단
        vec2( 0.0f,    offset), // 중앙 상단
        vec2( offset,  offset), // 우측 상단
        vec2(-offset,  0.0f),   // 좌측 중앙
        vec2( 0.0f,    0.0f),   // 정중앙
        vec2( offset,  0.0f),   // 우측 중앙
        vec2(-offset, -offset), // 좌측 하단
        vec2( 0.0f,   -offset), // 중앙 하단
        vec2( offset, -offset)  // 우측 하단   
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
	    sampleTex[i] = vec3(texture(screen_texture, texcoord.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return vec4(col, 1.0);
} 

void main() 
{ 
    float gamma = 1.6f;
    o_flagcolor = vec4(pow(origin().rgb, vec3(1.0 / gamma)), 1.0f);
   // o_flagcolor = vec4(pow(kerneling(edge).rgb, vec3(1.0 / gamma)), 1.0f);
} 

