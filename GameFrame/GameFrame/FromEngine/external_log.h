#pragma once
#include "external_file.h"
#include <mutex>



class LogClass
{
	const char* path;
	ExtFileClass LogFile;
	std::mutex lk;
public:
	LogClass() = default;
	void SetPath(const char* _Path)
	{
		path = _Path;
	}
	LogClass(const char* _Path)
	{
		path = _Path;
	}
	bool AddLog(const char* str, bool ln = true);
	bool AddLogC(const char c, bool ln = false);
	bool AddLog(const int num, bool ln = true);
	bool AddLog(const void* ptr, bool ln = true);
	bool AddLog_Hex(const int num, bool ln = true);
	bool AddLog_FixedHex(const int num, bool ln = true);
	bool AddLog_CurTime(bool ln = true);
	bool ClearLog();
};

int Ustrlen(const char *StrDest);
int Ustrlen(const wchar_t *StrDest);
std::string TimeNow();

/*

*/
