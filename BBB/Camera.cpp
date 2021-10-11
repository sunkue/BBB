#include "stdafx.h"

#include "Camera.h"


void Camera::update()
{
	auto trans = glm::translate(_ownner->get_position()+_diff);
	auto target_rotate = _ownner->get_rotation();
	auto scale = glm::scale(_ownner->get_scale());

	auto tick_t = GAME_SYSTEM::instance().tick_time();
	//auto m = trans * glm::toMat4(target_rotate) * scale;
	//_position = m * V4_DEFAULT;
	/* ��ü�����ؼ� �̵�, ī�޶� ������ ������ ȸ��. */
	{
		static auto prev_rotate = glm::quat();
		float ra = static_cast<float>(tick_t.count()) / _rotate_lag.count();
		auto	rotate = glm::mix(prev_rotate, target_rotate, clamp(ra, 0.f, 1.f));
		prev_rotate = target_rotate;
		auto m = trans * glm::toMat4(rotate) * scale;
		_position = m * V4_DEFAULT;
	}


	_target = _ownner->get_position();
	return;




}
