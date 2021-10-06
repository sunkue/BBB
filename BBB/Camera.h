#pragma once



/// /////////////////////////////////////////////////////////////////////////

class OBJ;
/* 소유된 (붙은) 카메라 */
class Camera 
{
	friend class OBJ;
public:
	void update(const OBJ& ownner);
	void set_diff(glm::vec3 d) { _diff = d; };

	glm::mat4 view_mat()const { return glm::lookAt(_position, _target, _up); };

	glm::vec3 get_dir()const { return glm::normalize(_position - _target); };
	glm::vec3 get_right()const { return glm::normalize(glm::cross(_up, get_dir())); };
	glm::vec3 get_up()const { return _up; };

private:
	milliseconds _rotate_lag{ 200ms };
	glm::vec3 _diff{};
	

	glm::vec3 _position{ 0.f,3.f,5.f };
	glm::vec3 _target{ V_ZERO };
	glm::vec3 _up{ Y_DEFAULT };
};

using CameraPtr = shared_ptr<Camera>;

/// /////////////////////////////////////////////////////////////////////////
