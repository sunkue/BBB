#version 450

uniform sampler2D u_tex_sampler[4];

flat in int texture_index;
in vec2 texcoord;

void main()
{
	float alpha = texture(u_tex_sampler[texture_index], texcoord).a;
//	if (alpha < 0.1)
//		discard;
//  if ����ȭ ��û ���ϳ�,, �׸��ڸ� �ػ󵵿� ����ؼ� �� �׷�..

	gl_FragDepth = gl_FragCoord.z;
}
