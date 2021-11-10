#include "stdafx.h"
#include "Camera.h"
#include "DynamicObj.h"

void Camera::update(float time_elpased)
{
	if (ownner_)
	{
		ownner_->update_camera(this, time_elpased);
	}

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
