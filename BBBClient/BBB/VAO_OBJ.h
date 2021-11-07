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

class Billboard : public OBJ
{
public:
	explicit Billboard(const ModelPtr& model) : OBJ{ model } {}
public:
	void update_uniform_vars(const class Shader* shader)const;
};

///////////////////////////////////////////
