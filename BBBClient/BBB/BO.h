#pragma once

namespace BO_HELPER
{
	GLuint get_next_binding_point();
}

template<class T>
struct UBO
{
	UBO()
	{
		binding = BO_HELPER::get_next_binding_point();
		glGenBuffers(1, &ubo_id);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(T), NULL, GL_STATIC_DRAW);
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