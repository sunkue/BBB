#pragma once

using ShaderPtr = unique_ptr<class Shader>;

class Shader
{
public:
	Shader(string_view filenameVS, string_view filenameFS);

	void use()const;

	template<class T> void set(string_view uniform_var_name, const T& value)const;
	void set_texture(string_view uniform_var_name, GLuint texture)const;
	GLuint create_vao(const VERTEX* vertices, GLsizei vertices_num)const;

private:
	void add_shader(GLuint ShaderProgram, const char* pShaderText, GLenum shader_type);
	GLuint compile_shader(string_view filenameVS, string_view filenameFS);

private:
	GLuint shader_id_;
};


#include "Shader.hpp"