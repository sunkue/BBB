#pragma once

#include "FileHelper.h"


/// /////////////////////////////////////////////////////////////////////////

class Obj;

/* 소유된 (붙은) 카메라 */
using CameraPtr = shared_ptr<class Camera>;
class Camera : public IDataOnFile
{

private:
	virtual void save_file_impl(ofstream& file) final;
	virtual void load_file_impl(ifstream& file) final;

public:
	void update(float time_elapsed);

	glm::mat4 view_mat()const { return glm::lookAt(position_, target_, up_); };

	glm::vec3 get_look_dir()const { return glm::normalize(target_ - position_); };
	glm::vec3 get_right()const { return glm::normalize(glm::cross(up_, -get_look_dir())); };

	void draw_gui()
	{
		gui::Begin("Camera");
		
		GUISAVE();
		GUILOAD();

		auto look = get_look_dir();
		auto right = get_right();

		gui::DragFloat3("look", glm::value_ptr(look));
		gui::DragFloat3("right", glm::value_ptr(right));
		gui::DragFloat3("Up", glm::value_ptr(up_));
		gui::End();
	}

	SET(ownner);
	GET(ownner);

	SET(diff);
	GET(diff);

	SET(target);
	GET(target);

	SET(up);
	GET(up);

	GET(position);
	SET(position);

public:
	GET(shaking);
	GET(shaking_time);
	void camera_shake(float duration) { shaking_ = true; shaking_time_ = duration; };
private:
	bool shaking_ = false;
	float shaking_time_ = 0.f;

private:
	Obj* ownner_ = nullptr;

	glm::vec3 diff_{};

	glm::vec3 position_{ V_ZERO };
	glm::vec3 target_{ V_ZERO };
	glm::vec3 up_{ Y_DEFAULT };
};


/// /////////////////////////////////////////////////////////////////////////
