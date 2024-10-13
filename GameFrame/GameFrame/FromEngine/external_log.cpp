#pragma once
#include "external_log.h"


#define __Open(_Mode) lk.lock();if(!LogFile.Open(path, #_Mode))return false
#define __Exit LogFile.Close();lk.unlock();return true

bool LogClass::AddLog(const char* str, bool ln)
{
	__Open(a);
	LogFile.PutStr(str);
	if(ln)LogFile.PutChr('\n');
	__Exit;
}

bool LogClass::AddLogC(const char c, bool ln)
{
	static char Ln = '\n';
	__Open(ab);
	LogFile.Write(&c, 1, 1);
	LogFile.Write(&Ln, 1, 1);
	__Exit;
}

bool LogClass::AddLog(const int num, bool ln)
{
	__Open(a);
	static char LogBuffer[3000];
	sprintf(LogBuffer, "%d", num);
	LogFile.PutStr(LogBuffer);
	if (ln)LogFile.PutChr('\n');
	__Exit;
}

bool LogClass::AddLog(const void* ptr, bool ln)
{
	__Open(a);
	static char LogBuffer[3000];
	sprintf(LogBuffer, "%p", ptr);
	LogFile.PutStr(LogBuffer);
	if (ln)LogFile.PutChr('\n');
	__Exit;
}

bool LogClass::AddLog_Hex(const int num, bool ln)
{
	__Open(a);
	static char LogBuffer[3000];
	sprintf(LogBuffer, "%.2X", num);
	LogFile.PutStr(LogBuffer);
	if (ln)LogFile.PutChr('\n');
	__Exit;
}

bool LogClass::AddLog_FixedHex(const int num, bool ln)
{
	__Open(a);
	static char LogBuffer[3000];
	sprintf(LogBuffer, "%.8X", num);
	LogFile.PutStr(LogBuffer);
	if (ln)LogFile.PutChr('\n');
	__Exit;
}

bool LogClass::ClearLog()
{
	__Open(w);
	__Exit;
}

bool LogClass::AddLog_CurTime(bool ln)
{
	__Open(a);
	LogFile.PutStr(("["  + TimeNow() + "]").c_str());
	if (ln)LogFile.PutChr('\n');
	__Exit;
}

#include <ctime>
const static char 星期[7][20] = 
	{ u8"星期日", u8"星期一", u8"星期二", u8"星期三", u8"星期四", u8"星期五", u8"星期六" };
//const static char 月份[12][20] = 
//{ "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月", };
//{ "1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月", };
std::string TimeNow()
{
	char* TBuf = new char[5000]();
	std::tm stm;
	std::time_t tt = std::time(0);
	localtime_s(&stm, &tt);
	sprintf(TBuf, u8"%04d年%02d月%02d日 %s %02d:%02d:%02d", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, 星期[stm.tm_wday], stm.tm_hour, stm.tm_min, stm.tm_sec);
	std::string rt = TBuf;
	delete[]TBuf;
	return rt;
}


int Ustrlen(const char* StrDest)
{
	int i = 0;
	while ((*StrDest) != '\0' || (*(StrDest + 1)) != '\0')
	{
		StrDest++;
		i++;
	}
	return i + 3;
}

int Ustrlen(const wchar_t* StrDest)
{
	return Ustrlen((char*)StrDest);
}
