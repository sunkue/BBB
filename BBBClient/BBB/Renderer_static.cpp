#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer()
{
	glClearDepth(1.f);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, screen.width, screen.height);
	init();
}


Renderer::~Renderer()
{
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
		screen_.viewport_ = { 0, (h - new_h) / 2, w, new_h };
	}
	else
	{
		const float ratio_h = (float)h / s_h;
		const int new_w = static_cast<int>(s_w * ratio_h);
		screen_.viewport_ = { (w - new_w) / 2, 0, new_w, h };
	}

	draw();
}