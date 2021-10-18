#pragma once

#include "stdafx.h"

const int g_WindowSizeX = 1240;
const int g_WindowSizeY = 720;

const glm::vec3 V_ZERO{ glm::vec3(0) };
const glm::vec3 X_DEFAULT{ 1, 0, 0 };
const glm::vec3 Y_DEFAULT{ 0, 1, 0 };
const glm::vec3 Z_DEFAULT{ 0, 0, 1 };
const glm::vec3 V3_DEFAULT{ glm::vec3(1) };
const glm::vec4 V4_DEFAULT{ glm::vec3(0), 1 };
const glm::mat4 M_DEFAULT{ 1 };
const float ROOT3{ 1.732421875 };
const float HALF_ROOT3 = { 0.8662109375 };

/* max = 256 */
using INDEX = uint8_t;

struct VERTEX
{
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};

const VERTEX box[36]
{
	/* +Y */
	{ {-1,  1, -1}, Y_DEFAULT, {0, 0}},
	{ {-1,  1,  1}, Y_DEFAULT, {1, 0}},
	{ { 1,  1, -1}, Y_DEFAULT, {0, 1}},

	{ { 1,  1, -1}, Y_DEFAULT, {0, 1}},
	{ {-1,  1,  1}, Y_DEFAULT, {1, 0}},
	{ { 1,  1,  1}, Y_DEFAULT, {1, 1}},
	/* +X */
	{ { 1,  1,  1}, X_DEFAULT, {0, 1}},
	{ { 1, -1,  1}, X_DEFAULT, {0, 0}},
	{ { 1, -1, -1}, X_DEFAULT, {1, 0}},

	{ { 1, -1, -1}, X_DEFAULT, {1, 0}},
	{ { 1,  1, -1}, X_DEFAULT, {1, 1}},
	{ { 1,  1,  1}, X_DEFAULT, {0, 1}},
	/* +Z */
	{ { 1,  1,  1}, Z_DEFAULT, {1, 1}},
	{ {-1,  1,  1}, Z_DEFAULT, {0, 1}},
	{ { 1, -1,  1}, Z_DEFAULT, {1, 0}},

	{ { 1, -1,  1}, Z_DEFAULT, {1, 0}},
	{ {-1,  1,  1}, Z_DEFAULT, {0, 1}},
	{ {-1, -1,  1}, Z_DEFAULT, {0, 0}},
	/* -X */
	{ {-1, -1,  1}, -X_DEFAULT, {1, 0}},
	{ {-1,  1,  1}, -X_DEFAULT, {1, 1}},
	{ {-1, -1, -1}, -X_DEFAULT, {0, 0}},

	{ {-1, -1, -1}, -X_DEFAULT, {0, 0}},
	{ {-1,  1,  1}, -X_DEFAULT, {1, 1}},
	{ {-1,  1, -1}, -X_DEFAULT, {0, 1}},
	/* -Z */
	{ {-1,  1, -1}, -Z_DEFAULT, {1, 1}},
	{ { 1,  1, -1}, -Z_DEFAULT, {0, 1}},
	{ {-1, -1, -1}, -Z_DEFAULT, {1, 0}},

	{ {-1, -1, -1}, -Z_DEFAULT, {1, 0}},
	{ { 1,  1, -1}, -Z_DEFAULT, {0, 1}},
	{ { 1, -1, -1}, -Z_DEFAULT, {0, 0}},
	/* -Y */
	{ { 1, -1, -1}, -Y_DEFAULT, {0, 1}},
	{ { 1, -1,  1}, -Y_DEFAULT, {1, 1}},
	{ {-1, -1,  1}, -Y_DEFAULT, {1, 0}},

	{ {-1, -1,  1}, -Y_DEFAULT, {1, 0}},
	{ {-1, -1, -1}, -Y_DEFAULT, {0, 0}},
	{ { 1, -1, -1}, -Y_DEFAULT, {0, 1}}
};



const VERTEX cross_billboard_3[18]
{
	/* - */
	{ {HALF_ROOT3,  2, 0}, Y_DEFAULT, {0, 1}},
	{ {HALF_ROOT3, 0,  2}, Y_DEFAULT, {1, 0}},
	{ {HALF_ROOT3,  2,  2}, Y_DEFAULT, {1, 1}},

	{ {HALF_ROOT3,  2, 0}, Y_DEFAULT, {0, 1}},
	{ {HALF_ROOT3, 0, 0}, Y_DEFAULT, {0, 0}},
	{ {HALF_ROOT3, 0,  2}, Y_DEFAULT, {1, 0}},

	/* / */
	{ {0,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ { ROOT3, 0,  1.5}, Y_DEFAULT, {1, 0}},
	{ { ROOT3,  2,  1.5}, Y_DEFAULT, {1, 1}},

	{ {0,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ {0, 0, 0.5}, Y_DEFAULT, {0, 0}},
	{ { ROOT3, 0,  1.5}, Y_DEFAULT, {1, 0}},

	/* \ */
	{ { ROOT3,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ {0, 0,  1.5}, Y_DEFAULT, {1, 0}},
	{ {0,  2,  1.5}, Y_DEFAULT, {1, 1}},

	{ { ROOT3,  2, 0.5}, Y_DEFAULT, {0, 1}},
	{ { ROOT3, 0, 0.5}, Y_DEFAULT, {0, 0}},
	{ {0, 0,  1.5}, Y_DEFAULT, {1, 0}}
};