#pragma once




/// /////////////////////////////////////////////////////////////////////////

class OBJ;

/* 소유된 (붙은) 카메라 */
class Camera 
{
	friend class OBJ;
public:
	void update(float time_elapsed);
	
	glm::vec3 get_diff()const { return _diff; };
	void set_diff(glm::vec3 d) { _diff = d; };

	glm::mat4 view_mat()const { return glm::lookAt(position_, _target, _up); };

	glm::vec3 get_dir()const { return glm::normalize(position_ - _target); };
	glm::vec3 get_right()const { return glm::normalize(glm::cross(_up, get_dir())); };
	glm::vec3 get_up()const { return _up; };

	void set_ownner(OBJ* ownner) { _ownner = ownner; }

	void camera_shake(float duration) { _shaking = true; _shaking_time = duration; };

	GET(position);
private:
	milliseconds _rotate_lag{ 200ms };
	glm::vec3 _diff{};
	
	OBJ* _ownner = nullptr;

	glm::vec3 position_{ V_ZERO };
	glm::vec3 _target{ V_ZERO };
	glm::vec3 _up{ Y_DEFAULT };

	bool _shaking = false;
	float _shaking_time = 0.f;
};

using CameraPtr = shared_ptr<Camera>;

/// /////////////////////////////////////////////////////////////////////////
