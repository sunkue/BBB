#include "stdafx.h"
#include "DynamicObj.h"
#include "KeyboardEvent.h"
#include "Camera.h"



bool VehicleObj::process_input(const KEY_BOARD_EVENT_MANAGER::key_event& key)
{
	bool pressed = (key.action != GLFW_RELEASE);
	switch (key.key)
	{
	case GLFW_KEY_W :
	{
		up_on_ = pressed;
	}
	CASE GLFW_KEY_S :
	{
		down_on_ = pressed;
	}
	CASE GLFW_KEY_A :
	{
		left_on_ = pressed;
	}
	CASE GLFW_KEY_D :
	{
		right_on_ = pressed;
	}
	CASE GLFW_KEY_SPACE :
	{
		brake_on_ = pressed;
	}
	break; default: return false;
	}

	return true;
}

void VehicleObj::update_state()
{
	const glm::vec3 _angular_power = Y_DEFAULT * 1.5f;
	constexpr float _acceleration_power = 16.f;
	constexpr float _friction_power = 2.0f;

	angular_speed_ = _angular_power * static_cast<int>(angular_control_);
	acceleration_ = _acceleration_power * static_cast<int>(accel_control_);

	// [B] if(_brake_on)_acceleration = 0;
	//_acceleration -= _acceleration * _brake_on;

	friction_ = _friction_power;
	// [B] if(_brake_on)_friction += _friction_power;
	friction_ += _friction_power * brake_on_;
	friction_ += _friction_power * brake_on_;

	int accel_control{ static_cast<int>(CONTROLL::none) };
	int angular_control{ static_cast<int>(CONTROLL::none) };
	accel_control += up_on_;
	accel_control -= down_on_;
	angular_control -= right_on_;
	angular_control += left_on_;
	accel_control_ = static_cast<CONTROLL>(accel_control);
	angular_control_ = static_cast<CONTROLL>(angular_control);
}

void VehicleObj::update_camera(Camera* camera, float time_elpased) const
{
	glm::vec3 target_dir; // using for target & position

	/* target */
	{
		const auto head_dir = get_head_dir();
		const auto moving_speed = get_linear_speed();
		const auto moving_len = glm::length(moving_speed);
		auto moving_dir = glm::normalize(moving_speed);
		if (moving_len < 0.125f)
		{
			moving_dir = head_dir;
		}
		target_dir = glm::lerp(head_dir, moving_dir, clamp(0.2f * moving_len, 0.00f, 0.4f));
		camera->set_target(get_position() + target_dir * 10.0f);
	}

	/* position */
	{
		auto diff = camera->get_diff();
		auto rotate = quat_from2vectors({ diff.x, 0,diff.z }, { -target_dir.x, 0, -target_dir.z });
		const auto trans_diff = glm::translate(diff);
		const auto trans = glm::translate(get_position());
		const auto scale = glm::scale(get_scale());
		auto m = trans * glm::toMat4(rotate) * scale * trans_diff;
		camera->set_position(m * V4_DEFAULT);
	}
}
