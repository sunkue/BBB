#include "stdafx.h"

#include "Camera.h"
#include "DynamicObj.h"

void Camera::update(float time_elpased)
{
	glm::vec3 target_dir; // using for target & position

	/* target */
	{
		const auto head_dir = _ownner->get_head_dir();
		const auto moving_speed = reinterpret_cast<DynamicObj*>(_ownner)->get_speed();
		const auto moving_len = glm::length(moving_speed);
		auto moving_dir = glm::normalize(moving_speed);
		if (moving_len < 0.125f)
		{
			moving_dir = head_dir;
		}
		target_dir = glm::lerp(head_dir, moving_dir, clamp(0.2f * moving_len, 0.00f, 0.4f));
		_target = _ownner->get_position() + target_dir * 10.0f;
	}

	/* position */
	{
		auto rotate = quat_from2vectors({ _diff.x, 0,_diff.z }, { -target_dir.x, 0, -target_dir.z });
		const auto trans_diff = glm::translate(_diff);
		const auto trans = glm::translate(_ownner->get_position());
		const auto scale = glm::scale(_ownner->get_scale());
		auto m = trans * glm::toMat4(rotate) * scale * trans_diff;
		position_ = m * V4_DEFAULT;
	}

	/* shake */
	if (shaking_)
	{
		shaking_time_ -= time_elpased;
		if (shaking_time_ <= 0.f)
		{
			shaking_ = false;
		}
		constexpr float magic_limit = 0.171875;
		constexpr float friq = 10.f;
		const float rand_val = static_cast<float>(rand() % 10);
		auto random_friq = shaking_time_ * friq * rand_val;
		auto cos_friq = cos(random_friq) * magic_limit;
		auto sin_friq = sin(random_friq) * magic_limit;
		position_.x += cos_friq;
		position_.y += sin_friq;
		position_.z += cos_friq;
	}
}
