#pragma once


#include "DynamicObj.h"
#include "Camera.h"
#include "Shader.h"
#include "Sun.h"
#include "BO.h"
#include "TrackNode.h"

//////////////////////////////////////////////////////

struct SCREEN
{
	GLfloat fovy{ 45.0f }; //glm::Radians(fovy)
	const GLsizei width{ g_WindowSizeX };	//W/H
	const GLsizei height{ g_WindowSizeY };	//W/H
	GLfloat n{ 0.1f };
	GLfloat f{ 20000.f };
	glm::vec4 viewport_{};
	GLfloat aspect()const { return (GLfloat)width / height; }
	glm::mat4 proj_mat()const { return glm::perspective(glm::radians(fovy), aspect(), n, f); }
};
extern SCREEN screen;

//////////////////////////////////////////////////////

// [-1, 1] pos // [0,1] tex // screen covered quad.
class ScreenQuad
{
	SINGLE_TON(ScreenQuad);

public:
	// set using shader and uniforms first.
	void draw_quad();

private:
	GLuint quad_vao;
};

//////////////////////////////////////////////////////
struct GodRayParam : public IDataOnFile
{
protected:
	virtual void save_file_impl(ofstream& file);
	virtual void load_file_impl(ifstream& file);

public:
	bool enable_godray = true;

	float u_samples = 128;		// 샘플링수
	float u_decay = 0.98;		// 감쇠(샘플링 누적감쇠)
	float u_density = 0.9;		// 샘플링밀도
	float u_weight = 0.07;		// 가중치(샘플링 가중치)
	float u_exposure = 0.15;	// 최종노출도

	void draw_gui()
	{
		//gui::Begin("GodRay");
		gui::Text("Godray");
		GUISAVE(); GUILOAD();
		gui::Checkbox("Enable", &enable_godray);
		gui::DragFloat("samples", &u_samples, 0.1f, 0, 512, "%.f", 1);
		gui::DragFloat("decay", &u_decay, 0.001f, 0.9, 1, "%.3f", 1);
		gui::DragFloat("density", &u_density, 0.01f, 0, 1, "%.2f", 1);
		gui::DragFloat("weight", &u_weight, 0.001f, 0, 0.1, "%.3f", 1);
		gui::DragFloat("exposure", &u_exposure, 0.01f, 0, 1, "%.2f", 1);
		//gui::End();

	}

	void update_uniform_vars(const ShaderPtr& shader);
};

class SunRenderer
{
public:
	friend class Renderer;

	GLuint sky_fbo;
	TexturePtr skypass_tbo;
	//	ShaderPtr skypass_shader;

		// /*
	GLuint godray_fbo;
	TexturePtr godraypass_tbo;
	ShaderPtr godraypass_shader;

	GodRayParam godray_param;
	/**/

	void init();

	void bind_sky_fbo()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, sky_fbo);
		glViewport(0, 0, screen.width, screen.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

	}

	void draw_quad()
	{
		ScreenQuad::get().draw_quad();
	}

	void draw_godray()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, godray_fbo);
		glViewport(0, 0, screen.width, screen.height);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		if (false == godray_param.enable_godray)
		{
			return;
		}

		godraypass_shader->use();
		godraypass_shader->set("skypass_texture", skypass_tbo);
		godray_param.update_uniform_vars(godraypass_shader);

		draw_quad();
	}
};

class gBufferRenderer
{
public:
	friend class Renderer;

	GLuint gbuffer_fbo;
	TexturePtr worldpos_tbo;
	TexturePtr normal_tbo;
	TexturePtr albedospec_tbo;

	GLuint lightpass_fbo;
	TexturePtr lightpass_tbo;

	ShaderPtr lightpass_shader;

	void init();

	void bind_gbuffer_fbo()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo);
		glViewport(0, 0, screen.width, screen.height);
		glClearColor(0.2f, 0.1f, 0.6f, 1.0f); // background color =>alpha값 체킹
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		// draw objs
	}

	void bind_lightpass_fbo()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, lightpass_fbo);
		glViewport(0, 0, screen.width, screen.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		lightpass_shader->use();
		lightpass_shader->set("g_world_pos", worldpos_tbo);
		lightpass_shader->set("g_normal", normal_tbo);
		lightpass_shader->set("g_albedospec", albedospec_tbo);

		// set light uniforms..
	}

	void draw_quad()
	{
		lightpass_shader->use();
		ScreenQuad::get().draw_quad();
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

	void bind_directional_depthmap_fbo()
	{
		glViewport(0, 0, SHADOW_WIDTH_H, SHADOW_HEIGHT_H);
		glBindFramebuffer(GL_FRAMEBUFFER, directional_depthmap_fbo);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void set_directional_depthmap_shader(const ShaderPtr& depthmap_shader, int lightmat_index)
	{
		depthmap_shader->use();

		/// 동적그림자..

		float n = screen.n; float f = screen.f;
		glm::mat4 lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, n, f);
		glm::mat4 lightView = glm::lookAt(-Sky::get().get_sun_light()->direction * 1000, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		depthmap_shader->set("u_lightpos_mat", lightSpaceMatrix);

		///

		//depthmap_shader->set("u_lightpos_mat", directional_lightspace_mat[lightmat_index]);
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

	GLuint screen_fbo;
	TexturePtr screen_tbo;

	ShaderPtr screen_shader;

	void init();

	void blit_fbo(GLuint read_fbo)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screen_fbo);
		glBlitFramebuffer(0, 0, screen.width, screen.height, 0, 0, screen.width, screen.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void draw_screen(TexturePtr& bg, TexturePtr& godray)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(screen.viewport_.x, screen.viewport_.y, screen.viewport_.z, screen.viewport_.w);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		screen_shader->use();
		screen_shader->set("screen_texture", screen_tbo);
		screen_shader->set("bg_texture", bg);
		screen_shader->set("godray_texture", godray);

		ScreenQuad::get().draw_quad();

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

	glm::mat4 vp_mat()const
	{
		auto p = proj_mat();
		auto v = main_camera_->view_mat();
		return p * v;
	}

	GET_REF(main_camera);
	GET_REF(player);
	GET_REF(ghost);
	SET(ghost);
	GET_REF(cars);
	GET_REF(depth_renderer);
	GET_REF(screen_renderer);
	GET_REF(gbuffer_renderer);
	GET_REF(sun_renderer);
	GET_REF_UNSAFE(track);

	void swap_player_ghost();

private:
	void init();
	void init_shader();
	void init_resources();
	void load_texture();
	void load_model();

private:

private:
	CameraPtr main_camera_;
	SCREEN& screen_ = screen;

	DirectionalLightPtr testing_directional_light_;
	PointLightPtr testing_point_light_;
	SpotLightPtr testing_spot_light_;

	CubeMapPtr skybox;
	ObjPtr default_map;

	//

	unique_ptr<ScreenRenderer> screen_renderer_;

	unique_ptr<DepthRenderer> depth_renderer_;
	ShaderPtr directional_depthmap_shader_;
	ShaderPtr directional_grass_depthmap_shader_;
	ShaderPtr point_depthmap_shader_;

	unique_ptr<gBufferRenderer> gbuffer_renderer_;

	unique_ptr<SunRenderer> sun_renderer_;
	ObjPtr sun_;
	ShaderPtr sun_w_shader_;
	ShaderPtr sun_b_shader_;

	//
	ShaderPtr skypass_shader_;

	//
	UBO<glm::mat4> ubo_vp_mat;
	UBO<glm::mat4> ubo_inv_v_mat;
	UBO<glm::mat4> ubo_inv_p_mat;
	UBO<glm::mat4> ubo_lightspace_mat;
	UBO<DirectionalLight> ubo_sun;
	UBO<glm::vec2> ubo_default_buffer_resolution;
	//

	ShaderPtr default_g_shader_;
	vector<ObjPtr> cars_;
	//


	//
	ObjPtr player_;
	ObjPtr ghost_;
	//
	ShaderPtr grass_g_shader_;
	InstancingObjPtr grasses_;

	//
	//
	Track& track_ = Track::get();
};



