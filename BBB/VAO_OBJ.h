#pragma once

#include "Camera.h"


/////////////////////////////////////////////////////////////////

struct OBJ_DATA
{
	GLuint vao;
};
using ObjDataPtr = shared_ptr<OBJ_DATA>;
/////////////////////////////////////////////////////////////////

/* 그려지는 모든 물체 */
class OBJ
{
public:
	explicit OBJ(ObjDataPtr obj_data, GLuint shader) : _obj_data{ obj_data }, _shader{ shader }{}


	glm::mat4 model_mat()const { return glm::translate(_translate) * glm::toMat4(_quaternion)* glm::scale(_scale); }

	glm::vec3 get_position()const { return glm::vec3{ _translate }; }
	glm::quat get_rotation()const { return _quaternion; }
	glm::vec3 get_scale()const { return _scale; }

	glm::vec3 get_head_dir()const { return get_rotation() * X_DEFAULT; }

	void rotate(glm::quat q) { _quaternion = q * _quaternion; }
	void move(glm::vec3 dif) { _translate += dif; }
	void scaling(glm::vec3 ratio) { _scale *= ratio; }
public:
	void set_camera(CameraPtr camera) { _camera = camera; };

public:
	void bind_vao()const{ glBindVertexArray(_obj_data->vao); }
	void update_uniform_vars()const;

public:
	void update()
	{
		if (_camera)
			_camera->update(*this);
	}

private:
	ObjDataPtr _obj_data;
	GLuint _shader;

	glm::vec3 _translate;
	glm::quat _quaternion;
	glm::vec3 _scale{ V3_DEFAULT };

	CameraPtr _camera;
};
using ObjPtr = shared_ptr<OBJ>;
