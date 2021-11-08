#pragma once

using ShaderPtr = shared_ptr<class Shader>;
class Shader
{
public:

	void use()const;

	template<class T> void set(const string& uniform_var_name, const T& value)const;

	GET(shader_id);

	CREATE_SHARED(ShaderPtr, Shader);
private:
	Shader(string_view filenameVS, string_view filenameFS, vector<string_view>& includesFS);
	
private:
	void add_shader(GLuint ShaderProgram, const char* pShaderText, GLenum shader_type);
	GLuint compile_shader(string_view filenameVS, string_view filenameFS, vector<string_view>& includesFS);

private:
	GLuint shader_id_;
};


#include "Shader.hpp"