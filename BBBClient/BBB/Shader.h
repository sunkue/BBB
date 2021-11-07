#pragma once

using ShaderPtr = unique_ptr<class Shader>;

class Shader
{
public:
	Shader(string_view filenameVS, string_view filenameFS, vector<string_view>& includesFS);

	void use()const;

	template<class T> void set(const string& uniform_var_name, const T& value)const;
	void set_texture(string_view uniform_var_name, GLuint texture)const;

	GLuint create_vao(const Vertex* vertices, GLsizei vertices_num)const;

	GET(shader_id);
private:
	void add_shader(GLuint ShaderProgram, const char* pShaderText, GLenum shader_type);
	GLuint compile_shader(string_view filenameVS, string_view filenameFS, vector<string_view>& includesFS);

private:
	GLuint shader_id_;
};


#include "Shader.hpp"