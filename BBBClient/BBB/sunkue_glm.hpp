#pragma once
#include "stdafx.h"

inline glm::quat quat_from2vectors(glm::vec3 u, glm::vec3 v)
{
	float norm_u_norm_v = sqrt(dot(u, u) * dot(v, v));
	float real_part = norm_u_norm_v + dot(u, v);
	glm::vec3 w;

	if (real_part < 1.e-6f * norm_u_norm_v)
	{
		real_part = 0.0f;
		w = abs(u.x) > abs(u.z) ? glm::vec3(-u.y, u.x, 0.f)
			: glm::vec3(0.f, -u.z, u.y);
	}
	else
	{
		w = cross(u, v);
	}

	return glm::normalize(glm::quat(real_part, w.x, w.y, w.z));
}
