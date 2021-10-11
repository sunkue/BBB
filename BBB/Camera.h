#pragma once


#include "VAO_OBJ.h"

/// /////////////////////////////////////////////////////////////////////////

class OBJ;
using ObjPtr = shared_ptr<OBJ>;
/* 소유된 (붙은) 카메라 */
class Camera 
{
	friend class OBJ;
public:
	void update();
	void set_diff(glm::vec3 d) { _diff = d; };

	glm::mat4 view_mat()const { return glm::lookAt(_position, _target, _up); };

	glm::vec3 get_dir()const { return glm::normalize(_position - _target); };
	glm::vec3 get_right()const { return glm::normalize(glm::cross(_up, get_dir())); };
	glm::vec3 get_up()const { return _up; };

	void set_ownner(ObjPtr ownner) { _ownner = ownner; }

private:
	milliseconds _rotate_lag{ 200ms };
	glm::vec3 _diff{};
	
	ObjPtr _ownner;

	glm::vec3 _position{ V_ZERO };
	glm::vec3 _target{ V_ZERO };
	glm::vec3 _up{ Y_DEFAULT };
};

using CameraPtr = shared_ptr<Camera>;

/// /////////////////////////////////////////////////////////////////////////
