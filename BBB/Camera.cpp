#include "stdafx.h"

#include "Camera.h"
#include "DynamicObj.h"

void Camera::update(float time_elpased)
{

	auto tick_t = GAME_SYSTEM::instance().tick_time();
	//auto m = trans * glm::toMat4(target_rotate) * scale;
	//_position = m * V4_DEFAULT;
	/* 구체보간해서 이동, 카메라 스무스 딜레이 회전. */
	{
		auto target_rotate = _ownner->get_rotation();

		static auto prev_rotate = glm::quat();
		float ra = static_cast<float>(tick_t.count()) / _rotate_lag.count();
		auto rotate = glm::mix(prev_rotate, target_rotate, clamp(ra, 0.f, 1.f));
		rotate = glm::slerp(prev_rotate, target_rotate, clamp(ra, 0.f, 1.f));
		prev_rotate = target_rotate;
		auto trans_diff = glm::translate(_diff);
		auto trans = glm::translate(_ownner->get_position());
		auto scale = glm::scale(_ownner->get_scale());
		auto m = trans * glm::toMat4(rotate) * trans_diff * scale;
		_position = m * V4_DEFAULT;
	}

	_target = _ownner->get_position() + _ownner->get_head_dir() * 10.0f;
	//_target = _ownner->get_position();


	/* shake */
	if (_shaking)
	{
		_shaking_time -= time_elpased;
		if (_shaking_time <= 0.f)
		{
			_shaking = false;
		}
		constexpr float magic_num = 0.171875;
		constexpr float limit_x = 1.0f;
		constexpr float limit_y = 1.0f;
		constexpr float limit_z = 1.0f;
		constexpr float friq = 10.f;
		const float f = static_cast<float>(rand() % 10);
		_position.x += cos(_shaking_time * friq * f) * magic_num;
		_position.y += sin(_shaking_time * friq * f) * magic_num;
		_position.z += cos(_shaking_time * friq * f) * magic_num;
	}


	return;
}
