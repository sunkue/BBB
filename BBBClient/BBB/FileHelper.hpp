#pragma once

#include "FileHelper.h"

/////////////////////////////////////////////////////

template<class T>
inline void FILE_HELPER::load_file(ifstream& file, T& var)
{
	static_assert(false, "load_file(), come to ~~~ and add template for type.");
}

template<class T>
inline void FILE_HELPER::save_file(ofstream& file, T& var)
{
	static_assert(false, "save_file(), come to ~~~ and add template for type.");
}

/////////////////////////////////////////////////////

#define TEMPLATE_LOAD_FILE(type) template<> inline void FILE_HELPER::load_file(ifstream& file, type& var)
#define TEMPLATE_SAVE_FILE(type) template<> inline void FILE_HELPER::save_file(ofstream& file, type& var)

/////////////////////////////////////////////////////

TEMPLATE_LOAD_FILE(glm::vec3)
{
	string tmp;
	file >> tmp >> var.x >> var.y >> var.z;
}

TEMPLATE_SAVE_FILE(glm::vec3)
{
	file << var.x << " " << var.y << " " << var.z << endl;
}