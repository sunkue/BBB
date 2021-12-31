#version 450

uniform sampler2D u_tex_sampler[4];

flat in int texture_index;
in vec2 texcoord;

void main()
{
	float alpha = texture(u_tex_sampler[texture_index], texcoord).a;
//	if (alpha < 0.1)
//		discard;
//  if 최적화 엄청 심하네,, 그림자맵 해상도에 비례해서 더 그럼..

	gl_FragDepth = gl_FragCoord.z;
}
