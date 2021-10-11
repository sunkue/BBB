#pragma once

#include "stdafx.h"

/* max = 256 */
using INDEX = uint8_t;

struct VERTEX
{
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};

const VERTEX box[8]
{
	/* position */
	{{-1.f,  1.f, -1.f}, glm::normalize(glm::vec3{-1.f,  1.f, -1.f}), {0.f, 0.f}},
	{{ 1.f,  1.f, -1.f}, glm::normalize(glm::vec3{ 1.f,  1.f, -1.f}), {0.f, 1.f}},
	{{-1.f,  1.f,  1.f}, glm::normalize(glm::vec3{-1.f,  1.f,  1.f}), {1.f, 0.f}},
	{{ 1.f,  1.f,  1.f}, glm::normalize(glm::vec3{ 1.f,  1.f,  1.f}), {1.f, 1.f}},
	{{-1.f, -1.f, -1.f}, glm::normalize(glm::vec3{-1.f, -1.f, -1.f}), {1.f, 1.f}},
	{{ 1.f, -1.f, -1.f}, glm::normalize(glm::vec3{ 1.f, -1.f, -1.f}), {1.f, 0.f}},
	{{-1.f, -1.f,  1.f}, glm::normalize(glm::vec3{-1.f, -1.f,  1.f}), {0.f, 1.f}},
	{{ 1.f, -1.f,  1.f}, glm::normalize(glm::vec3{ 1.f, -1.f,  1.f}), {0.f, 0.f}}
};




const INDEX box_index[36]
{
	0,2,1,
	1,2,3,
	3,7,5,
	5,1,3,
	3,2,7,
	7,2,6,
	6,2,4,
	4,2,0,
	0,1,4,
	4,1,5,
	5,7,6,
	6,4,5
};

const glm::vec3 V_ZERO{ glm::vec3(0.f) };
const glm::vec3 X_DEFAULT{ 1.f,0.f,0.f };
const glm::vec3 Y_DEFAULT{ 0.f,1.f,0.f };
const glm::vec3 Z_DEFAULT{ 0.f,0.f,1.f };
const glm::vec3 V3_DEFAULT{ glm::vec3(1.f) };
const glm::vec4 V4_DEFAULT{ glm::vec3(0.f),1.f };
const glm::mat4 M_DEFAULT{ 1.f };