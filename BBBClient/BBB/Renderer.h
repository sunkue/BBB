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

	bool read_file(string_view filename, std::string& target);
	void add_shader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint compile_shader(string_view filenameVS, string_view filenameFS);
	GLuint create_vao(GLuint shader, const VERTEX* vertices, GLsizei vertices_num);
	void load_texture();
	void load_model();

private:
	unique_ptr<struct RESOURCE> resource_;
	CameraPtr main_camera_;
	SCREEN& screen_ = screen;
	glm::mat4 vp_mat_;

	ShaderPtr testing_shader_;
	//
	ShaderPtr default_shader_;
	ObjPtr sky_box_;
	GLuint sky_box_tex_;


	//
	vector<ObjPtr> cars_;
	Player0Ptr player_;
	GLuint player_tex_;

	//
	ShaderPtr terrain_shader_;
	GLuint terrain_tex_;
	ObjPtr terrain_;

	//
protected:
	ShaderPtr billboard_shader_;
	GLuint billboard_tex0_;
	GLuint billboard_tex1_;
	GLuint billboard_tex2_;
	GLuint billboard_tex3_;
	vector<Billboard> grasses_;

};




