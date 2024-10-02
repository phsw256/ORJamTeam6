#include"global_tool_func.h"
#include <Windows.h>
 

hash_t StrHash(const std::string& __Str)
{
	std::hash<std::string> hash_str;
	return ((hash_t)hash_str(__Str));
}
hash_t StrHash(const char* __Str)
{
	std::string ____String____(__Str);
	return StrHash(____String____);
}

char* RemoveFrontSpace(char* src)
{
	char* res = src;
	while (*res == ' ' || *res == '\t')++res;
	return res;
}
void RemoveBackSpace(char* src)
{
	char* res = src;
	--res;
	while (*(++res) != 0); --res;
	while (*res == ' ' || *res == '\t' || *res == '\r' || *res == '\n')--res;
	*(res + 1) = 0;
}
void RemoveBackSpace(std::string& src)
{
	while (1)
	{
		char i = src[src.length() - 1];
		if (i == ' ' || i == '\t' || i == '\r' || i == '\n')src.pop_back();
		else break;
	}
}


// ANSI�ַ���ת����Unicode
std::wstring MBCStoUnicode(const std::string& MBCS)
{
    int nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, NULL, NULL);   // ��ȡ���������ȣ��ٷ����ڴ�
    WCHAR* tch = new WCHAR[nLength + 4]();
    nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, tch, nLength);     // ��MBCSת����Unicode
    std::wstring ret = tch;
    delete[] tch;
    return ret;
}
// UTF-8�ַ���ת����Unicode
std::wstring UTF8toUnicode(const std::string& UTF8)
{
    int nLength = MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), -1, NULL, NULL);   // ��ȡ���������ȣ��ٷ����ڴ�
    WCHAR* tch = new WCHAR[nLength + 4]{};
    MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), -1, tch, nLength);     // ��UTF-8ת����Unicode
    std::wstring ret = tch;
    delete[] tch;
    return ret;
}
// Unicode�ַ���ת����UTF-8
std::string UnicodetoUTF8(const std::wstring& Unicode)
{
    int UTF8len = WideCharToMultiByte(CP_UTF8, 0, Unicode.c_str(), -1, 0, 0, 0, 0);// ��ȡUTF-8���볤��
    char* UTF8 = new CHAR[UTF8len + 4]{};
    WideCharToMultiByte(CP_UTF8, 0, Unicode.c_str(), -1, UTF8, UTF8len, 0, 0); //ת����UTF-8����
    std::string ret = UTF8;
    delete[] UTF8;
    return ret;
}
// Unicode�ַ���ת����ANSI
std::string UnicodetoMBCS(const std::wstring& Unicode)
{
    int ANSIlen = WideCharToMultiByte(CP_ACP, 0, Unicode.c_str(), -1, 0, 0, 0, 0);// ��ȡUTF-8���볤��
    char* ANSI = new CHAR[ANSIlen + 4]{};
    WideCharToMultiByte(CP_ACP, 0, Unicode.c_str(), -1, ANSI, ANSIlen, 0, 0); //ת����UTF-8����
    std::string ret = ANSI;
    delete[] ANSI;
    return ret;
}
// ANSI�ַ���ת����UTF-8
std::string MBCStoUTF8(const std::string& MBCS)
{
    return UnicodetoUTF8(MBCStoUnicode(MBCS));
}
// UTF-8�ַ���ת����ANSI
std::string UTF8toMBCS(const std::string& MBCS)
{
    return UnicodetoMBCS(UTF8toUnicode(MBCS));
}

uint64_t GetSysTimeMicros()
{
    const uint64_t EpochFIleTime = (116444736000000000ULL);
    FILETIME ft;
    LARGE_INTEGER li;
    uint64_t tt = 0;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    // ��1970��1��1��0:0:0:000�����ڵ�΢����(UTCʱ��)
    tt = (li.QuadPart - EpochFIleTime) / 10;
    return tt;
}
