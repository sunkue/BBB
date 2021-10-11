#pragma once

#include "stdafx.h"


const glm::vec3 V_ZERO{ glm::vec3(0.f) };
const glm::vec3 X_DEFAULT{ 1.f,0.f,0.f };
const glm::vec3 Y_DEFAULT{ 0.f,1.f,0.f };
const glm::vec3 Z_DEFAULT{ 0.f,0.f,1.f };
const glm::vec3 V3_DEFAULT{ glm::vec3(1.f) };
const glm::vec4 V4_DEFAULT{ glm::vec3(0.f),1.f };
const glm::mat4 M_DEFAULT{ 1.f };

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
	{ {-1.f,  1.f, -1.f}, Y_DEFAULT, {0, 0}},
	{ {-1.f,  1.f,  1.f}, Y_DEFAULT, {1, 0}},
	{ { 1.f,  1.f, -1.f}, Y_DEFAULT, {0, 1}},

	{ { 1.f,  1.f, -1.f}, Y_DEFAULT, {0, 1}},
	{ {-1.f,  1.f,  1.f}, Y_DEFAULT, {1, 0}},
	{ { 1.f,  1.f,  1.f}, Y_DEFAULT, {1, 1}},
	/* +X */
	{ { 1.f,  1.f,  1.f}, X_DEFAULT, {0.f, 1.f}},
	{ { 1.f, -1.f,  1.f}, X_DEFAULT, {0.f, 0.f}},
	{ { 1.f, -1.f, -1.f}, X_DEFAULT, {1.f, 0.f}},

	{ { 1.f, -1.f, -1.f}, X_DEFAULT, {1.f, 0.f}},
	{ { 1.f,  1.f, -1.f}, X_DEFAULT, {1.f, 1.f}},
	{ { 1.f,  1.f,  1.f}, X_DEFAULT, {0.f, 1.f}},
	/* +Z */
	{ { 1.f,  1.f,  1.f}, Z_DEFAULT, {1.f, 1.f}},
	{ {-1.f,  1.f,  1.f}, Z_DEFAULT, {0.f, 1.f}},
	{ { 1.f, -1.f,  1.f}, Z_DEFAULT, {1.f, 0.f}},

	{ { 1.f, -1.f,  1.f}, Z_DEFAULT, {1.f, 0.f}},
	{ {-1.f,  1.f,  1.f}, Z_DEFAULT, {0.f, 1.f}},
	{ {-1.f, -1.f,  1.f}, Z_DEFAULT, {0.f, 0.f}},
	/* -X */
	{ {-1.f, -1.f,  1.f}, -X_DEFAULT, {1.f, 0.f}},
	{ {-1.f,  1.f,  1.f}, -X_DEFAULT, {1.f, 1.f}},
	{ {-1.f, -1.f, -1.f}, -X_DEFAULT, {0.f, 0.f}},

	{ {-1.f, -1.f, -1.f}, -X_DEFAULT, {0.f, 0.f}},
	{ {-1.f,  1.f,  1.f}, -X_DEFAULT, {1.f, 1.f}},
	{ {-1.f,  1.f, -1.f}, -X_DEFAULT, {0.f, 1.f}},
	/* -Z */
	{ {-1.f,  1.f, -1.f}, -Z_DEFAULT, {1.f, 1.f}},
	{ { 1.f,  1.f, -1.f}, -Z_DEFAULT, {0.f, 1.f}},
	{ {-1.f, -1.f, -1.f}, -Z_DEFAULT, {1.f, 0.f}},

	{ {-1.f, -1.f, -1.f}, -Z_DEFAULT, {1.f, 0.f}},
	{ { 1.f,  1.f, -1.f}, -Z_DEFAULT, {0.f, 1.f}},
	{ { 1.f, -1.f, -1.f}, -Z_DEFAULT, {0.f, 0.f}},
	/* -Y */
	{ { 1.f, -1.f, -1.f}, -Y_DEFAULT, {0.f, 1.f}},
	{ { 1.f, -1.f,  1.f}, -Y_DEFAULT, {1.f, 1.f}},
	{ {-1.f, -1.f,  1.f}, -Y_DEFAULT, {1.f, 0.f}},

	{ {-1.f, -1.f,  1.f}, -Y_DEFAULT, {1.f, 0.f}},
	{ {-1.f, -1.f, -1.f}, -Y_DEFAULT, {0.f, 0.f}},
	{ { 1.f, -1.f, -1.f}, -Y_DEFAULT, {0.f, 1.f}}
};



