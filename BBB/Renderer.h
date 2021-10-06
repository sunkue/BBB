#pragma once


#include "VAO_OBJ.h"

//////////////////////////////////////////////////////

struct SCREEN 
{
	GLfloat fovy{ 60.0f }; //glm::Radians(fovy)
	GLsizei width{ 500 };	//W/H
	GLsizei height{ 500 };	//W/H
	GLfloat n{ 0.1f };
	GLfloat f{ 20000.0f };
	
	GLfloat aspect()const { return (GLfloat)width / height; }
	glm::mat4 proj_mat()const {
		return glm::perspective(glm::radians(fovy), aspect(), n, f);
	}
};

//////////////////////////////////////////////////////

struct RESOURCE
{

};

//////////////////////////////////////////////////////

/* clients drawer */
class Renderer
{
public:
	static Renderer& instance()
	{
		static Renderer _instance;
		return _instance;
	}


public:
	void draw();
	void ready_draw();

	glm::mat4 proj_mat()const { return _screen.proj_mat(); }

	GLuint get_uloc_mvp_mat()const { return _uloc_mvp_mat; }
	CameraPtr get_main_camera()const { return _main_camera; }

private:
	void init();
	void init_shader();
	void init_resources();

	bool read_file(string_view filename, std::string& target);
	void add_shader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint compile_shader(string_view filenameVS, string_view filenameFS);
	GLuint create_vao(const glm::vec3* vertices, GLsizei vertices_num, const INDEX* indices, GLsizei indices_num);
	


private:
	Renderer();
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;


private:
	GLuint _shader;

	GLuint _uloc_mvp_mat;

	CameraPtr _main_camera;
	SCREEN _screen;

	vector<ObjPtr> _cars;

	unique_ptr<struct RESOURCE> _resource;


};




