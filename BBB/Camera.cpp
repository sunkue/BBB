#include "stdafx.h"
#include "Camera.h"
#include "VAO_OBJ.h"

void Camera::update(const OBJ& ownner)
{
	Sleep(10);
	glm::vec3 EulerAngles(0, 0.0, 0.1);
	glm::quat q = EulerAngles;
	_position = glm::toMat4(q) * glm::vec4{ _position, 1.0f };

	return;
	auto trans = glm::translate(ownner.get_position()+_diff);
	auto target_rotate = ownner.get_rotation();
	auto scale = glm::scale(ownner.get_scale());

	auto tick_t = GAME_SYSTEM::instance().tick_time();

	/* 구체보간해서 이동, 카메라 스무스 딜레이 회전. */
	{
		static auto prev_rotate = glm::quat();
		float ra = static_cast<float>(tick_t.count()) / _rotate_lag.count();
		auto	rotate = glm::mix(prev_rotate, target_rotate, clamp(ra, 0.f, 1.f));
		prev_rotate = target_rotate;
		auto m = trans * glm::toMat4(rotate) * scale;

		_position = m * glm::vec4{};
	}




}
