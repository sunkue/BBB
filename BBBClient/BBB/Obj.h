#pragma once

#include "Light.h"
#include "Model.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"
#include "Collision.h"

/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
class Obj;
class Camera;

__interface IObj
{
	void update_uniform_vars(const ShaderPtr& shader)const;
	void update(float time_elapsed);
	void draw(const ShaderPtr& shader)const;
	void collision_detect(const Obj& other)const;
	void update_camera(class Camera* camera, float time_elpased) const;
	bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key);
	bool process_input(const MOUSE_EVENT_MANAGER::scroll_event& scroll);
	bool process_input(const MOUSE_EVENT_MANAGER::button_event& button);
	bool process_input(const MOUSE_EVENT_MANAGER::pos_event& pos);
	void draw_gui();
};

/* 그려지는 모든 물체 */
class Obj : public IObj
{
public:
	
	explicit Obj(const ModelPtr& model) : model_{ model } {}
	virtual ~Obj() {}

	glm::mat4 model_mat()const { return glm::translate(translate_) * glm::toMat4(quaternion_) * glm::scale(scale_); }

	glm::vec3 get_position()const { return translate_; }
	glm::quat get_rotation()const { return quaternion_; }
	glm::vec3 get_scale()const { return scale_; }

	glm::vec3 get_head_dir()const { return get_rotation() * HEADING_DEFAULT; }
	glm::vec3 get_up_dir()const { return get_rotation() * UP_DEFAULT; }
	glm::vec3 get_right_dir()const { return get_rotation() * RIGHT_DEFAULT; }

	void rotate(const glm::quat& q) { quaternion_ *= q; boundings_.rotate(q); }
	void move(const glm::vec3& dif) { translate_ += dif; boundings_.move(dif); }
	void scaling(const glm::vec3& ratio) { scale_ *= ratio; boundings_.scaling(ratio); }

public:
	virtual void update_uniform_vars(const ShaderPtr& shader)const;

	virtual void update(float time_elapsed) {}

	virtual void draw(const ShaderPtr& shader)const
	{
		model_->draw(shader);
		boundings_.draw();
	}
	virtual void draw_gui() 
	{
		boundings_.draw_gui();
	};

	virtual void collision_detect(const Obj& other)const {}

	virtual void update_camera(Camera* camera, float time_elpased) const {}
	virtual bool process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key) { return false; };
	virtual bool process_input(const MOUSE_EVENT_MANAGER::scroll_event& scroll) { return false; };
	virtual bool process_input(const MOUSE_EVENT_MANAGER::button_event& button) { return false; };
	virtual bool process_input(const MOUSE_EVENT_MANAGER::pos_event& pos) { return false; };

public:
	glm::vec3 get_project_pos(glm::vec3 origin = {});


private:
	glm::vec3 translate_;
	glm::quat quaternion_;
	glm::vec3 scale_{ V3_DEFAULT };
	ModelPtr model_;

public:
	GET_REF(boundings);
private:
	Boundings boundings_;
};
using ObjPtr = shared_ptr<Obj>;

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

	void setup_instance_attribute(string_view name, const auto* data, GLenum type = GL_FLOAT, GLuint divisor = 1)
	{
		GLuint inst_abo;
		glGenBuffers(1, &inst_abo);
		glBindBuffer(GL_ARRAY_BUFFER, inst_abo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(*data) * num_inst_, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(vao);
		auto index = attbLoc_[name];
			//glGetAttribLocation(shader->get_shader_id(), name.data());
		glEnableVertexAttribArray(index);
		glBindBuffer(GL_ARRAY_BUFFER, inst_abo);
		glVertexAttribPointer(index, sizeof(*data) / 4, type, GL_FALSE, sizeof(*data), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(index, divisor);
		glBindVertexArray(0);
	}

	void update_uniform_vars(const ShaderPtr& shader)const;


	void draw(const ShaderPtr& shader)const
	{
		shader->use();
		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, verticlesize_, num_inst_);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	SET(num_inst);
	GET(num_inst);
	GET_REF(textures);

	void add_texture(TexturePtr& texture) { textures_.emplace_back(texture); }

	void add_instancing_attribute(string_view name, GLuint attbLoc) { attbLoc_[name] = attbLoc; }

private:
	GLuint vao;
	GLuint num_inst_;
	GLsizei verticlesize_;
	vector<TexturePtr> textures_;
	unordered_map<string_view, GLuint> attbLoc_;
};
using InstancingObjPtr = shared_ptr<InstancingObj>;

///////////////////////////////////////////
