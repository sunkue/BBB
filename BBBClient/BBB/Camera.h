#pragma once




/// /////////////////////////////////////////////////////////////////////////

class OBJ;

/* 소유된 (붙은) 카메라 */
using CameraPtr = shared_ptr<class Camera>;
class Camera
{
	friend class OBJ;
public:
	void update(float time_elapsed);
	
	glm::vec3 get_diff()const { return _diff; };
	void set_diff(glm::vec3 d) { _diff = d; };

	glm::mat4 view_mat()const { return glm::lookAt(position_, _target, _up); };

	glm::vec3 get_look_dir()const { return glm::normalize(_target - position_); };
	glm::vec3 get_right()const { return glm::normalize(glm::cross(_up, - get_look_dir())); };
	glm::vec3 get_up()const { return _up; };

	void set_ownner(OBJ* ownner) { _ownner = ownner; }

	void camera_shake(float duration) { shaking_ = true; shaking_time_ = duration; };
	GET(shaking);
	GET(shaking_time);
	GET(position);
private:
	milliseconds _rotate_lag{ 200ms };
	glm::vec3 _diff{};
	
	OBJ* _ownner = nullptr;

	glm::vec3 position_{ V_ZERO };
	glm::vec3 _target{ V_ZERO };
	glm::vec3 _up{ Y_DEFAULT };

	bool shaking_ = false;
	float shaking_time_ = 0.f;
};


/// /////////////////////////////////////////////////////////////////////////
