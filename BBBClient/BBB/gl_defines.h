#pragma once

#include "stdafx.h"

constexpr int g_WindowSizeX = 1240;
constexpr int g_WindowSizeY = 720;

const glm::vec3 V_ZERO{ glm::vec3{0} };
const glm::vec3 X_DEFAULT{ 1, 0, 0 };
const glm::vec3 Y_DEFAULT{ 0, 1, 0 };
const glm::vec3 Z_DEFAULT{ 0, 0, 1 };
const glm::vec3 V3_DEFAULT{ glm::vec3(1) };
const glm::vec4 V4_DEFAULT{ glm::vec3(0), 1 };
const glm::mat4 M_DEFAULT{ 1 };
constexpr float ROOT3{ 1.732421875 };
constexpr float HALF_ROOT3 = { 0.8662109375 };
constexpr float ellipsis = { 0.00001f };
constexpr float ellipsis2 = { 0.1f };

const glm::vec3 HEADING_DEFAULT = X_DEFAULT;
const glm::vec3 UP_DEFAULT = Y_DEFAULT;
const glm::vec3 RIGHT_DEFAULT = Z_DEFAULT;