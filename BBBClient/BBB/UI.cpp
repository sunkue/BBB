#include "stdafx.h"
#include "UI.h"
#include "Renderer.h"


/// //////////////////////////////////////

void UI::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(screen.viewport_.x, screen.viewport_.y, screen.viewport_.z, screen.viewport_.w);
	glDisable(GL_DEPTH_TEST);

	ranking_.draw();

	glEnable(GL_DEPTH_TEST);
}

/// //////////////////////////////////////

UI::RankingUI::RankingUI()
{
	//// init players & ui pos
	{
		glm::vec2 sp = start_pos;

		auto playernum = Renderer::get().get_cars().size();
		players_.reserve(playernum);
		dynamic_sps_.reserve(playernum);
		static_sps_.reserve(playernum);

		for (int i = 0; i < playernum; i++)
		{
			players_.emplace_back(Texture::create());
			players_.back()->id = CreatePngTexture("./Resource/Texture/grass/grass0.png");

			static_sps_.emplace_back(sp);
			dynamic_sps_.emplace_back(sp);

			sp -= diff;
		}
	}

	//// init shader
	{
		vector<string> VS, FS, GS;
		VS.emplace_back("./Shader/ui.vert"sv);
		FS.emplace_back("./Shader/ui.frag"sv);
		shader_ = Shader::create(VS, FS, GS);
	}


	//// init frame_vao
	{
		glGenVertexArrays(1, &frame_vao_);
		GLuint vbo;
		glGenBuffers(1, &vbo);
		struct QUAD
		{
			glm::vec2 pos;
			glm::vec2 tex;  // gl_Position.y / 2 + 0.5f
		};

		glm::vec2 start_pos;
		float w = width;
		float h = heigth;

		// 프레임 위치는 윈도우 좌표계로 그리자. 스타트 포인트 + w/h
		QUAD quadv[] =
		{
			{ { 0,-h}, {0,0} }
			,{{ w, 0}, {1,1} }
			,{{ 0, 0}, {0,1} }
			,{{ 0,-h}, {0,0} }
			,{{ w,-h}, {1,0} }
			,{{ w, 0}, {1,1} }
		};

		glBindVertexArray(frame_vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadv), quadv, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, pos));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, tex));

		glBindVertexArray(0);
	}
}

void UI::RankingUI::draw()
{
	shader_->use();

	for (int i = 0; i < players_.size(); i++)
	{
		// update uniforms.
		shader_->set("u_texture", players_[i]);
		shader_->set("u_startpos", dynamic_sps_[i]);

		// draw
		glBindVertexArray(frame_vao_);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

/// //////////////////////////////////////