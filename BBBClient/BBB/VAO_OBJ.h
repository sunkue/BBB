#pragma once

#include "Light.h"

/////////////////////////////////////////////////////////////////

struct OBJ_DATA
{
	GLuint vao;
};
using ObjDataPtr = shared_ptr<OBJ_DATA>;
/////////////////////////////////////////////////////////////////

const glm::vec3 HEADING_DEFAULT = X_DEFAULT;

/* 그려지는 모든 물체 */
class OBJ
{
public:
	explicit OBJ(const ObjDataPtr& obj_data, const MaterialPtr& material) : obj_data_{ obj_data }, material_{ material } {}

	glm::mat4 model_mat()const { return glm::translate(translate_) * glm::toMat4(quaternion_) * glm::scale(scale_); }

	glm::vec3 get_position()const { return glm::vec3{ translate_ }; }
	glm::quat get_rotation()const { return quaternion_; }
	glm::vec3 get_scale()const { return scale_; }

	glm::vec3 get_head_dir()const { return get_rotation() * HEADING_DEFAULT; }

	void rotate(glm::quat q) { quaternion_ = q * quaternion_; }
	void move(glm::vec3 dif) { translate_ += dif; }
	void scaling(glm::vec3 ratio) { scale_ *= ratio; }
	void set_material(const MaterialPtr& material) { material_ = material; }

public:
	void bind_vao()const { glBindVertexArray(obj_data_->vao); }
	void update_uniform_vars(const class Shader* shader)const;

public:
	glm::vec3 get_project_pos(glm::vec3 origin = {});


private:
	glm::vec3 translate_;
	glm::quat quaternion_;
	glm::vec3 scale_{ V3_DEFAULT };
	MaterialPtr material_;
	ObjDataPtr obj_data_;
};
using ObjPtr = shared_ptr<OBJ>;

///////////////////////////////////////////

class Billboard : public OBJ
{
public:
	explicit Billboard(const ObjDataPtr& obj_data, const MaterialPtr& material) : OBJ{ obj_data, material } {}
public:
	void update_uniform_vars(const class Shader* shader)const;
};

///////////////////////////////////////////
