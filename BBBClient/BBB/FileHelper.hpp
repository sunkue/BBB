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

TEMPLATE_LOAD_FILE(glm::quat)
{
	string tmp;
	file >> tmp >> var.x >> var.y >> var.z >> var.w;
}

TEMPLATE_SAVE_FILE(glm::quat)
{
	file << var.x << " " << var.y << " " << var.z << "" << var.z << endl;
}

TEMPLATE_LOAD_FILE(float)
{
	string tmp;
	file >> tmp >> var;
}

TEMPLATE_SAVE_FILE(float)
{
	file << var << endl;
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
