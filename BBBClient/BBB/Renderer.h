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
	friend Billboard;
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
	
	ShaderPtr screen_shader_;
	GLuint quad_vao;
	GLuint fbo;
	TexturePtr tbo;
	GLuint rbo;

	//
	UBO<glm::mat4> ubo_vp_mat{ 0 };
	//
	ShaderPtr default_shader_;

	//
	vector<ObjPtr> cars_;
	Player0Ptr player_;
	//
	ShaderPtr terrain_shader_;
	ObjPtr terrain_;

	//
protected:
	ShaderPtr billboard_shader_;
	vector<Billboard> grasses_;

};



