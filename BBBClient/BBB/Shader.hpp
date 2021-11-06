#pragma once
#include "stdafx.h"
#include "Shader.h"


/////////////////////////////////////////

#define TEMPLATE_SHADER_SET(type) template<> inline void Shader::set<type>(string_view uniform_var_name, const type& value)const
#define UNIFORM_LOCATION glGetUniformLocation(shader_id_, uniform_var_name.data())

/////////////////////////////////////////

template<class T>
inline void Shader::set(string_view uniform_var, const T& value)const
{
	static_assert(false, "Shader::set T&, come to Shader.hpp and add template for type.");
}

/////////////////////////////////////////

TEMPLATE_SHADER_SET(bool)
{
	glUniform1i(UNIFORM_LOCATION, value);
}

TEMPLATE_SHADER_SET(int)
{
	glUniform1i(UNIFORM_LOCATION, value);
}

TEMPLATE_SHADER_SET(float)
{
	glUniform1f(UNIFORM_LOCATION, value);
}

TEMPLATE_SHADER_SET(glm::mat4)
{
	glUniformMatrix4fv(UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(value));
}

TEMPLATE_SHADER_SET(glm::vec2)
{
	glUniform2fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

TEMPLATE_SHADER_SET(glm::vec3)
{
	glUniform3fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

TEMPLATE_SHADER_SET(glm::vec4)
{
	glUniform4fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}


/////////////////////////////////////////

inline void Shader::set_texture(string_view uniform_var_name, GLuint texture) const
{
	glProgramUniform1i(shader_id_, UNIFORM_LOCATION, texture);
	glActiveTexture(GL_TEXTURE0 + texture);
	glBindTexture(GL_TEXTURE_2D, texture);
}

inline void Shader::set_light(const string& uniform_light_name, const LightPtr& light) const
{
	set(uniform_light_name + ".position", light->position);
	set(uniform_light_name + ".ambient", light->ambient);
	set(uniform_light_name + ".diffuse", light->diffuse);
	set(uniform_light_name + ".specular", light->specular);
}

/////////////////////////////////////////

#undef TEMPLATE_SHADER_SET
#undef UNIFORM_LOCATION