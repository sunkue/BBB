#pragma once


#include "DynamicObj.h"
#include "Camera.h"
#include "Shader.h"
#include "BO.h"

//////////////////////////////////////////////////////

struct SCREEN
{
	GLfloat fovy{ 45.0f }; //glm::Radians(fovy)
	GLsizei width{ g_WindowSizeX };	//W/H
	GLsizei height{ g_WindowSizeY };	//W/H
	GLfloat n{ 0.1f };
	GLfloat f{ 20000.0f };
	glm::vec4 viewport_{};
	GLfloat aspect()const { return (GLfloat)width / height; }
	glm::mat4 proj_mat()const {
		return glm::perspective(glm::radians(fovy), aspect(), n, f);
	}
};
extern SCREEN screen;

//////////////////////////////////////////////////////





//////////////////////////////////////////////////////


//////////////////////////////////////////////////////

/* clients drawer */
class Renderer
{
private:
	SINGLE_TON(Renderer);
	~Renderer();

public:
	void draw();
	void ready_draw();

	void reshape(int w, int h);

	glm::mat4 proj_mat()const { return screen_.proj_mat(); }

	CameraPtr get_main_camera()const { return main_camera_; }
	Player0Ptr get_player()const { return player_; }

private:
	void init();
	void init_shader();
	void init_resources();

	void load_texture();
	void load_model();

private:
	CameraPtr main_camera_;
	SCREEN& screen_ = screen;

	ShaderPtr testing_shader_;

	DirectionalLightPtr testing_directional_light_;
	PointLightPtr testing_point_light_;
	SpotLightPtr testing_spot_light_;
	
	CubeMapPtr skybox;
	ObjPtr default_map;
	
	struct ScreenRenderer
	{
		GLuint predraw_fbo;
		GLuint predraw_rbo;
		TexturePtr predraw_tbo;

		GLuint screen_fbo;
		TexturePtr screen_tbo;

		GLuint quad_vao;
		ShaderPtr screen_shader;

		ScreenRenderer()
		{
			glGenVertexArrays(1, &quad_vao);
			GLuint vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);
			struct QUAD
			{
				glm::vec2 pos;
				glm::vec2 tex;
			};
			QUAD quadv[] =
			{
				{{-1,-1},{0,0}},
				{{ 1, 1},{1,1}},
				{{-1, 1},{0,1}},
				{{-1,-1},{0,0}},
				{{ 1,-1},{1,0}},
				{{ 1, 1},{1,1}}
			};

			glBindVertexArray(quad_vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadv), quadv, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, pos));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, tex));

			glBindVertexArray(0);

			/// /

			glGenFramebuffers(1, &predraw_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, predraw_fbo);

			predraw_tbo = Texture::create();
			glGenTextures(1, &predraw_tbo->id);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, predraw_tbo->id);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB,
				screen.width, screen.height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0
				, GL_TEXTURE_2D_MULTISAMPLE, predraw_tbo->id, 0);

			glGenRenderbuffers(1, &predraw_rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, predraw_rbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screen.width, screen.height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, predraw_rbo);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);



			glGenFramebuffers(1, &screen_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

			screen_tbo = Texture::create();
			glGenTextures(1, &screen_tbo->id);
			glBindTexture(GL_TEXTURE_2D, screen_tbo->id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_tbo->id, 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cerr << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			vector<string> VS; VS.emplace_back("./Shader/screen_vertex.glsl"sv);
			vector<string> FS; FS.emplace_back("./Shader/screen_fragment.glsl"sv);
			vector<string> GS;
			screen_shader = Shader::create(VS, FS, GS);
		}

		void bind_scene_fbo()
		{
			glViewport(0, 0, screen.width, screen.height);
			glBindFramebuffer(GL_FRAMEBUFFER, predraw_fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}

		void blit_fbo()
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, predraw_fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screen_fbo);
			glBlitFramebuffer(0, 0, screen.width, screen.height, 0, 0, screen.width, screen.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		void draw_screen()
		{
			glViewport(screen.viewport_.x, screen.viewport_.y, screen.viewport_.z, screen.viewport_.w);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.4f, 0.2f, 0.0f, 1.0f);
			glDisable(GL_DEPTH_TEST);
			
			screen_shader->use();
			screen_shader->set("screen_texture", screen_tbo);

			glBindVertexArray(quad_vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	};

	shared_ptr<ScreenRenderer> screen_renderer;

	//
	UBO<glm::mat4> ubo_vp_mat{ 0 };
	//

	//
	vector<ObjPtr> cars_;
	Player0Ptr player_;
	//

	ShaderPtr billboard_shader_;
	InstancingObj grasses_;

};



