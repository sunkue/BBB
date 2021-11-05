#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer()
{
	glClearDepth(1.f);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GLU_FILL);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init();
}


Renderer::~Renderer()
{
}

void Renderer::init()
{
	init_shader();
	init_resources();
}



GLuint Renderer::create_vao(GLuint shader, const VERTEX* vertices, GLsizei vertices_num)
{
	GLuint retvao;
	GLuint abo;
	GLuint ebo;

	glGenVertexArrays(1, &retvao);
	glBindVertexArray(retvao);

	glGenBuffers(1, &abo);
	glBindBuffer(GL_ARRAY_BUFFER, abo);
	glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(VERTEX), vertices, GL_STATIC_DRAW);


	GLint positionAttribute = glGetAttribLocation(shader, "a_position");
	if (positionAttribute == -1) {
		std::cerr << "position 속성 설정 실패" << '\n';
		exit(-1);
	}
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)offsetof(VERTEX, pos));

	GLint normalAttribute = glGetAttribLocation(shader, "a_normal");
	if (normalAttribute == -1) {
		std::cerr << "normal 속성 설정 실패" << '\n';
		exit(-1);
	}
	glEnableVertexAttribArray(normalAttribute);
	glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)offsetof(VERTEX, nor));

	GLint texcoordAttribute = glGetAttribLocation(shader, "a_texcoord");
	if (texcoordAttribute == -1) {
		std::cerr << "texcoord 속성 설정 실패" << '\n';
		exit(-1);
	}
	glEnableVertexAttribArray(texcoordAttribute);
	glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (const GLvoid*)offsetof(VERTEX, tex));


	glBindVertexArray(0);
	glDeleteBuffers(1, &abo);
	glDeleteBuffers(1, &ebo);

	return retvao;
}




void Renderer::reshape(const int w, const int h)
{
	const auto s_w = screen_.width;
	const auto s_h = screen_.height;
	const float s_aspect = screen_.aspect();
	const float aspect = (float)w / h;

	if (aspect < s_aspect)
	{
		const float ratio_W = (float)w / s_w;
		const int new_h = static_cast<int>(s_h * ratio_W);
		glViewport(0, (h - new_h) / 2, w, new_h);
	}
	else
	{
		const float ratio_h = (float)h / s_h;
		const int new_w = static_cast<int>(s_w * ratio_h);
		glViewport((w - new_w) / 2, 0, new_w, h);
	}


	draw();
}