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

class gBufferRenderer
{
public:
	friend class Renderer;

	GLuint gbuffer_fbo;

	TexturePtr position_tbo;
	TexturePtr normal_tbo;
	TexturePtr albedospec_tbo;

	void init()
	{
		glGenFramebuffers(1, &gbuffer_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);

		//position
		position_tbo = Texture::create();
		glGenTextures(1, &position_tbo->id);
		glBindTexture(GL_TEXTURE_2D, position_tbo->id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screen.width, screen.height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position_tbo->id, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//normal
		normal_tbo = Texture::create();
		glGenTextures(1, &normal_tbo->id);
		glBindTexture(GL_TEXTURE_2D, normal_tbo->id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screen.width, screen.height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tbo->id, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// color spec
		albedospec_tbo = Texture::create();
		glGenTextures(1, &albedospec_tbo->id);
		glBindTexture(GL_TEXTURE_2D, albedospec_tbo->id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen.width, screen.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedospec_tbo->id, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
		glDrawBuffers(3, attachments);

		// add depth bufer,, etc..
		GLuint rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen.width, screen.height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void bind_gbuffer_fbo()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
		glViewport(0, 0, screen.width, screen.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		
	}

	void draw_screen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(screen.viewport_.x, screen.viewport_.y, screen.viewport_.z, screen.viewport_.w);
		glClearColor(0.4f, 0.2f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		//screen_shader->use();
	//	screen_shader->set("screen_texture", screen_tbo);
		//glBindVertexArray(quad_vao);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
	//	glEnable(GL_DEPTH_TEST);
	}
};

class DepthRenderer
{
// 미리계산가능한 섀도우맵은 미리 계산해두자,
public:
	friend class Renderer;
	static const GLuint SHADOW_WIDTH_H = 1024 * 20, SHADOW_HEIGHT_H = 1024 * 20;
	static const GLuint SHADOW_WIDTH_L = 1024, SHADOW_HEIGHT_L = 1024;

	GLuint directional_depthmap_fbo;
	TexturePtr directional_depthmap_tbo;
	vector<glm::mat4> directional_lightspace_mat;

	GLuint point_depthcubemap_fbo;
	TexturePtr point_depthcubemap_tbo;
	vector<array<glm::mat4, 6>> point_lightspace_mat;

	void init()
	{
		// directional_depthmap
		glGenFramebuffers(1, &directional_depthmap_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, directional_depthmap_fbo);

		directional_depthmap_tbo = Texture::create();
		glGenTextures(1, &directional_depthmap_tbo->id);
		glBindTexture(GL_TEXTURE_2D, directional_depthmap_tbo->id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH_H, SHADOW_HEIGHT_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float bordercolor[] = { 1.0,1.0,1.0,1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);

		glBindFramebuffer(GL_FRAMEBUFFER, directional_depthmap_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directional_depthmap_tbo->id, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// point_depthcubemap
		glGenFramebuffers(1, &point_depthcubemap_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, point_depthcubemap_fbo);

		point_depthcubemap_tbo = Texture::create();
		glGenTextures(1, &point_depthcubemap_tbo->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, point_depthcubemap_tbo->id);
		for (GLuint i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH_L, SHADOW_HEIGHT_L, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, point_depthcubemap_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, point_depthcubemap_tbo->id, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//
	}

	int add_lightspace_mat(const DirectionalLightPtr& light)
	{
		float n = screen.n; float f = screen.f;
		glm::mat4 lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, n, f);
		glm::mat4 lightView = glm::lookAt(-light->direction * 1000, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		directional_lightspace_mat.emplace_back(lightSpaceMatrix);
		return directional_lightspace_mat.size() - 1;
	}

	int add_lightspace_mat(const PointLightPtr& light)
	{
		float aspect = (float)SHADOW_WIDTH_L / (float)SHADOW_HEIGHT_L; 
		float n = screen.n; float f = screen.f; 
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, n, f);
		auto lightpos = light->position;

		auto& mats = point_lightspace_mat.emplace_back();
		mats[0] = shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		mats[1] = shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		mats[2] = shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
		mats[3] = shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
		mats[4] = shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		mats[5] = shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

		return point_lightspace_mat.size() - 1;
	}

	void bind_directional_depthmap_fbo(const ShaderPtr& depthmap_shader, int lightmat_index)
	{
		glViewport(0, 0, SHADOW_WIDTH_H, SHADOW_HEIGHT_H);
		glBindFramebuffer(GL_FRAMEBUFFER, directional_depthmap_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		depthmap_shader->use();
		depthmap_shader->set("u_lightpos_mat", directional_lightspace_mat[lightmat_index]);
	}

	void bind_point_depthmap_fbo(const ShaderPtr& depthmap_shader, const PointLightPtr& light, int lightmat_index)
	{
		glViewport(0, 0, SHADOW_WIDTH_L, SHADOW_HEIGHT_L);
		glBindFramebuffer(GL_FRAMEBUFFER, point_depthcubemap_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		depthmap_shader->use();
		depthmap_shader->set("u_lightpos_mat", point_lightspace_mat[lightmat_index]);
		depthmap_shader->set("u_lightpos", light->position);
		depthmap_shader->set("u_far_plane", screen.f);
	}
};

class ScreenRenderer
{
public:
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
		glEnable(GL_DEPTH_TEST);
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
	GET_REF(depth_renderer);
	GET_REF(screen_renderer);
	GET_REF(gbuffer_renderer);

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
	
	//

	unique_ptr<ScreenRenderer> screen_renderer_;
	unique_ptr<DepthRenderer> depth_renderer_;
	ShaderPtr directional_depthmap_shader_;
	ShaderPtr point_depthmap_shader_;

	unique_ptr<gBufferRenderer> gbuffer_renderer_;
	ShaderPtr gbuffer_shader_;

	//
	UBO<glm::mat4> ubo_vp_mat{ 0 };
	UBO<glm::mat4> ubo_lightspace_mat{ 1 };
	//

	vector<ObjPtr> cars_;

	//
	ObjPtr player_;
	ObjPtr ghost_;
	//

	ShaderPtr billboard_shader_;
	InstancingObj grasses_;


};



