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
	GLfloat f{ 20000.f };
	glm::vec4 viewport_{};
	GLfloat aspect()const { return (GLfloat)width / height; }
	glm::mat4 proj_mat()const { return glm::perspective(glm::radians(fovy), aspect(), n, f); }
};
extern SCREEN screen;

//////////////////////////////////////////////////////

class DepthRenderer
{
	friend class Renderer;
	static const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint depthmap_fbo;
	TexturePtr depthmap_tbo;

	ShaderPtr t_shader;

	void init()
	{
		glGenFramebuffers(1, &depthmap_fbo);

		depthmap_tbo = Texture::create();
		glGenTextures(1, &depthmap_tbo->id);
		glBindTexture(GL_TEXTURE_2D, depthmap_tbo->id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap_tbo->id, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		vector<string> VS; VS.emplace_back("./Shader/t_vertex.glsl"sv);
		vector<string> FS; FS.emplace_back("./Shader/t_fragment.glsl"sv);
		vector<string> GS;
		t_shader = Shader::create(VS, FS, GS);
	}

	void bind_depthmap_fbo(ShaderPtr& depthmap_shader, DirectionalLightPtr& light)
	{
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthmap_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, screen.n, screen.f);
		glm::mat4 lightView = glm::lookAt(-light->direction * 200, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		depthmap_shader->use();
		depthmap_shader->set("u_vp_mat", lightSpaceMatrix);
	}

	void draw_screen(GLuint quad_vao)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(screen.viewport_.x, screen.viewport_.y, screen.viewport_.z, screen.viewport_.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.4f, 0.2f, 0.0f, 1.0f);

		t_shader->use();
		t_shader->set("depthmap", depthmap_tbo);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};

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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(screen.viewport_.x, screen.viewport_.y, screen.viewport_.z, screen.viewport_.w);
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
	unique_ptr<DepthRenderer> depth_renderer;

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


	ShaderPtr depthmap_shader_;
};



