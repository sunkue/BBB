#pragma once


#include "DynamicObj.h"
#include "Camera.h"
#include "Shader.h"

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

struct RESOURCE
{

};

//////////////////////////////////////////////////////

template<class T>
struct UBO
{
	UBO(GLuint binding) :binding{ binding }
	{
		glGenBuffers(1, &ubo_id);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);
		glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo_id);
	}

	void update(void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), data);
	}

	void bind(const ShaderPtr& shader, string_view name)
	{
		GLuint index = glGetUniformBlockIndex(shader->get_shader_id(), name.data());
		glUniformBlockBinding(shader->get_shader_id(), index, binding);
	}

private:
	GLuint ubo_id;
	GLuint binding;
};

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

	glm::mat4 vp_mat()const { return vp_mat_; }
	glm::mat4 proj_mat()const { return screen_.proj_mat(); }


	CameraPtr get_main_camera()const { return main_camera_; }
	Player0Ptr get_player()const { return player_; }

private:
	void init();
	void init_shader();
	void init_resources();

	GLuint create_vao(GLuint shader, const Vertex* vertices_, GLsizei vertices_num);
	void load_texture();
	void load_model();

private:
	unique_ptr<struct RESOURCE> resource_;
	CameraPtr main_camera_;
	SCREEN& screen_ = screen;
	glm::mat4 vp_mat_;
	ShaderPtr testing_shader_;
	ShaderPtr screen_shader_;
	DirectionalLightPtr testing_directional_light_;
	PointLightPtr testing_point_light_;
	SpotLightPtr testing_spot_light_;
	ObjPtr default_map;
	GLuint quad_vao;
	CubeMapPtr skybox;
	GLuint fbo;
	TexturePtr tbo;
	GLuint rbo;

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



