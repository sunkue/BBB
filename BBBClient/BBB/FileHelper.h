#pragma once

/////////////////////////////////
namespace FILE_HELPER
{
	template<class T> void load_file(ifstream& file, T& var);
	template<class T> void save_file(ofstream& file, T& var);
}

/////////////////////////////////

#define LOAD_FILE(file, var) FILE_HELPER::load_file(file, var)

#define SAVE_FILE(file, var) \
	{\
		file << #var << " ";\
		FILE_HELPER::save_file( file, var );\
	}

/////////////////////////////////

#include "FileHelper.hpp"

