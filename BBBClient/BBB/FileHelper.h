#pragma once

/////////////////////////////////
/// 선언부 :: 
/// virtual void save_file_impl(ofstream& file) final;
/// virtual void load_file_impl(ifstream& file) final;
/// 정의부 :: 
/// LOAD_FILE(file, var1); SAVE_FILE(file, var1); ...
/// 두 함수에서 로드와 세이브 순서 같게 해주야 함.


inline string add_extension(string_view str)
{
	static constexpr auto extension = ".txt";

	string str_extension{ str };
	str_extension += extension;

	return str_extension;
}

class IDataOnFile
{
public:
	void load(string_view filename = "")
	{
		if (filename != "")
		{
			datafilename_ = add_extension(filename);
		}

		load_file(initpath(datafilename_));
	}

	void save(string_view filename = "")
	{
		auto datafilename = (filename == "") ? (datafilename_) : (filename);
		save_file(initpath(datafilename));
	}

	GET(datafilename);

private:
	string datafilename_;

private:
	void load_file(string_view filename)
	{
		ifstream file{ filename.data(), ios::in };
		if (file.fail())
		{
			cerr << "[ERR]::no file::generate::" << filename << endl;
			// ofstream ofile{ filename.data(), ios::out };
			return;
		}
		load_file_impl(file);
	}

	void save_file(string_view filename)
	{
		// 롤백용 파일 저장
		ifstream ifile{ filename.data(), ios::in };
		if (ifile)
		{
			ofstream rollbackfile{ initpath("rollback/").append(datafilename_).data(), ios::out };
			rollbackfile << ifile.rdbuf();
		}

		// 저장
		ofstream ofile{ filename.data(), ios::out };
		save_file_impl(ofile);
	}

protected:
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
if (loadbutton) { load(); }

/////////////////////////////////

#include "FileHelper.hpp"

