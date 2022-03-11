#pragma once

#include "FileHelper.h"

/////////////////////////////////////////////////////

template<class T>
inline void FILE_HELPER::load_file(ifstream& file, T& var)
{
	static_assert(always_false<T>, "load_file(), come to FileHelper.hpp and specialize template for type.");
}



template<class T>
inline void FILE_HELPER::save_file(ofstream& file, T& var)
{
	static_assert(always_false<T>, "save_file(), come to FileHelper.hpp and specialize template for type.");
}

/////////////////////////////////////////////////////

#define TEMPLATE_LOAD_FILE(type) template<> inline void FILE_HELPER::load_file(ifstream& file, type& var)
#define TEMPLATE_SAVE_FILE(type) template<> inline void FILE_HELPER::save_file(ofstream& file, type& var)

/////////////////////////////////////////////////////

TEMPLATE_LOAD_FILE(bool)
{
	string tmp;
	file >> tmp >> var;
}

TEMPLATE_SAVE_FILE(bool)
{
	file << var << endl;
}

////////////////////////////////////////////////

TEMPLATE_LOAD_FILE(int)
{
	string tmp;
	file >> tmp >> var;
}

TEMPLATE_SAVE_FILE(int)
{
	file << var << endl;
}

////////////////////////////////////////////////

////////////////////////////////////////////////

TEMPLATE_LOAD_FILE(float)
{
	string tmp;
	file >> tmp >> var;
}

TEMPLATE_SAVE_FILE(float)
{
	file << var << endl;
}

////////////////////////////////////////////////

TEMPLATE_LOAD_FILE(glm::vec3)
{
	string tmp;
	file >> tmp >> var.x >> var.y >> var.z;
}

TEMPLATE_SAVE_FILE(glm::vec3)
{
	file << var.x << " " << var.y << " " << var.z << endl;
}

////////////////////////////////////////////////

TEMPLATE_LOAD_FILE(glm::quat)
{
	string tmp;
	file >> tmp >> var.x >> var.y >> var.z >> var.w;
}

TEMPLATE_SAVE_FILE(glm::quat)
{
	file << var.x << " " << var.y << " " << var.z << " " << var.w << endl;
}

/////////////////////////////////////////////////////

///////////////////////////////////////////////

TEMPLATE_LOAD_FILE(vector<int>)
{
	string tmp; file >> tmp;
	int size; file >> size;
	var.resize(size);
	for (auto& v : var)
	{
		file >> v;
	}
}

TEMPLATE_SAVE_FILE(vector<int>)
{
	file << var.size() << " ";
	for (auto& v : var)
	{
		file << v << " ";
	}
	file << endl;
}

/////////////////////////////////////////////////////

#define INIT_FILE() \
public:\
void load(string_view filename)\
{\
	filename_ = filename;\
	load_file(initpath(filename_));\
}\
\
void save()\
{\
	save_file(initpath(filename_));\
}\
\
private:\
	string_view filename_;\
\
private:\
	void load_file(string_view filename)\
	{\
		ifstream file{ filename.data(), ios::in };\
		load_file_impl(file);\
	}\
\
	void save_file(string_view filename)\
	{\
		ofstream file{ filename.data(), ios::out };\
		save_file_impl(file);\
	}\
\
private:\
	void load_file_impl(ifstream& file);\
	void save_file_impl(ofstream& file);\
