#pragma once
#include "stdafx.h"

namespace sunkueglm
{
	inline glm::quat quat_from2vectors(glm::vec3 u, glm::vec3 v = glm::vec3{ 1, 0, 0 })
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

	inline GLfloat cos_from2vectors(glm::vec3 u, glm::vec3 v = glm::vec3{ 1, 0, 0 })
	{
		auto nu = glm::normalize(u);
		auto nv = glm::normalize(v);
		auto normalized_dot = glm::dot(nu, nv);
		return normalized_dot;
	}

} using namespace sunkueglm;

struct rotator
{
	float yaw{ 0 };
	float pitch{ 0 };
	float roll{ 0 };
};