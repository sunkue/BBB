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
	glm::mat4 proj_mat()const { return glm::perspective(glm::radians(fovy), aspect(), n, f); }
};
extern SCREEN screen;

//////////////////////////////////////////////////////


class ScreenRenderer
{
	friend class Renderer;

	GLuint predraw_fbo;
	GLuint predraw_rbo;
	TexturePtr predraw_tbo;

	GLuint screen_fbo;
	TexturePtr screen_tbo;

	GLuint quad_vao;
	ShaderPtr screen_shader;

	void init();

	void bind_predraw_fbo()
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


//////////////////////////////////////////////////////


//////////////////////////////////////////////////////

/* clients drawer */
class Renderer
{
	friend GAME_SYSTEM;
private:
	SINGLE_TON(Renderer);
	~Renderer();

public:
	void draw();
	void ready_draw();

	void reshape(int w, int h);

	glm::mat4 proj_mat()const { return screen_.proj_mat(); }

	GET_REF(main_camera);
	GET_REF(player);	
	GET_REF(ghost);
	SET(ghost);
	GET_REF(cars);

	void swap_player_ghost();

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
	
	unique_ptr<ScreenRenderer> screen_renderer;

	//
	UBO<glm::mat4> ubo_vp_mat{ 0 };
	//

	vector<ObjPtr> cars_;

	//
	ObjPtr player_;
	ObjPtr ghost_;
	//

	ShaderPtr billboard_shader_;
	InstancingObj grasses_;

};



