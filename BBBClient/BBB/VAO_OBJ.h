#pragma once

#include "Light.h"
#include "Model.h"

/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////

const glm::vec3 HEADING_DEFAULT = X_DEFAULT;

/* 그려지는 모든 물체 */
class OBJ
{
public:
	explicit OBJ(const ModelPtr& model) : model_{ model } {}

	glm::mat4 model_mat()const { return glm::translate(translate_) * glm::toMat4(quaternion_) * glm::scale(scale_); }

	glm::vec3 get_position()const { return glm::vec3{ translate_ }; }
	glm::quat get_rotation()const { return quaternion_; }
	glm::vec3 get_scale()const { return scale_; }

	glm::vec3 get_head_dir()const { return get_rotation() * HEADING_DEFAULT; }

	void rotate(glm::quat q) { quaternion_ = q * quaternion_; }
	void move(glm::vec3 dif) { translate_ += dif; }
	void scaling(glm::vec3 ratio) { scale_ *= ratio; }

public:
	void update_uniform_vars(const ShaderPtr& shader)const;

public:
	glm::vec3 get_project_pos(glm::vec3 origin = {});
	void draw(const ShaderPtr& shader)const
	{
		model_->draw(shader);
	}

private:
	glm::vec3 translate_;
	glm::quat quaternion_;
	glm::vec3 scale_{ V3_DEFAULT };
	ModelPtr model_;
};
using ObjPtr = shared_ptr<OBJ>;

///////////////////////////////////////////


class InstancingObj
{
public:
	void setup_mesh(const vector<Vertex>& vertices)
	{
		GLuint abo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &abo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, abo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, nor));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, tex));

		glBindVertexArray(0);

		verticlesize_ = vertices.size();
	}

	void setup_instance_attribute(const ShaderPtr& shader, string_view name, auto* data, GLenum type = GL_FLOAT, GLuint divisor = 1)
	{
		GLuint inst_abo;
		glGenBuffers(1, &inst_abo);
		glBindBuffer(GL_ARRAY_BUFFER, inst_abo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(*data) * num_inst_, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(vao);
		auto index = glGetAttribLocation(shader->get_shader_id(), name.data());
		glEnableVertexAttribArray(index);
		glBindBuffer(GL_ARRAY_BUFFER, inst_abo);
		glVertexAttribPointer(index, sizeof(*data) / 4, type, GL_FALSE, sizeof(*data), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(index, divisor);
		glBindVertexArray(0);
	}

	void draw()const
	{
		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, verticlesize_, num_inst_);
		glBindVertexArray(0);
	}

	SET(num_inst);
	GET(num_inst);
	GET_REF(textures);

	void add_texture(TexturePtr& texture) { textures_.emplace_back(texture); }
private:
	GLuint vao;
	GLuint num_inst_;
	GLsizei verticlesize_;
	vector<TexturePtr> textures_;
};

///////////////////////////////////////////
