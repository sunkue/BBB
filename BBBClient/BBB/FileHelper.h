#pragma once

/////////////////////////////////
/// 선언부 :: 
/// virtual void save_file_impl(ofstream& file) final;
/// virtual void load_file_impl(ifstream& file) final;
/// 정의부 :: 
/// LOAD_FILE(file, var1); SAVE_FILE(file, var1); ...
/// 두 함수에서 로드와 세이브 순서 같게 해주야 함.
class IDataOnFile
{
public:
	void load(string_view filename = "")
	{
		datafilename_ = filename;
		load_file(initpath(datafilename_));
	}

	void save(string_view filename = "")
	{
		string_view datafilename;
		datafilename = (filename == "") ? (datafilename_) : (filename);
		save_file(initpath(datafilename));
	}

	GET(datafilename);

private:
	string_view datafilename_;

private:
	void load_file(string_view filename)
	{
		ifstream file{ filename.data(), ios::in };
		load_file_impl(file);
	}

	void save_file(string_view filename)
	{
		ofstream file{ filename.data(), ios::out };
		save_file_impl(file);
	}

private:
	virtual void load_file_impl(ifstream& file) = 0;
	virtual void save_file_impl(ofstream& file) = 0;
};

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

#define GUISAVE() \
auto savebutton = gui::Button("[SAVE]"); \
if (savebutton) { save(); }

#define GUILOAD() \
auto loadbutton = gui::Button("[LOAD]"); \
if (loadbutton) { load(get_datafilename()); }

/////////////////////////////////

#include "FileHelper.hpp"

