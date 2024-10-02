#pragma once

#include"..\Minimal.h"

#include<functional>
#include<string>
#include<algorithm>


hash_t StrHash(const std::string& __Str);
hash_t StrHash(const char* __Str);

char* RemoveFrontSpace(char* src);
void RemoveBackSpace(char* src);
void RemoveBackSpace(std::string& src);

template<typename T>
T gcd(T a, T b)
{
    while (b)
    {
        a %= b;
        std::swap(a, b);
    }
    return a;
}

template<typename T>
T lcm(T a, T b)
{
    return (a * b) / gcd(a, b);
}

// ANSI字符集转换成Unicode
std::wstring MBCStoUnicode(const std::string& MBCS);
// UTF-8字符集转换成Unicode
std::wstring UTF8toUnicode(const std::string& UTF8);
// Unicode字符集转换成UTF-8
std::string UnicodetoUTF8(const std::wstring& Unicode);
// Unicode字符集转换成ANSI
std::string UnicodetoMBCS(const std::wstring& Unicode);
// ANSI字符集转换成UTF-8
std::string MBCStoUTF8(const std::string& MBCS);
// UTF-8字符集转换成ANSI
std::string UTF8toMBCS(const std::string& MBCS);

uint64_t GetSysTimeMicros();

bool RegexFull_Nothrow(const std::string& Str, const std::string& Regex) throw();
bool RegexFull_Throw(const std::string& Str, const std::string& Regex);
bool RegexNone_Nothrow(const std::string& Str, const std::string& Regex) throw();
bool RegexNone_Throw(const std::string& Str, const std::string& Regex);
bool RegexNotFull_Nothrow(const std::string& Str, const std::string& Regex) throw();
bool RegexNotFull_Throw(const std::string& Str, const std::string& Regex);
bool RegexNotNone_Nothrow(const std::string& Str, const std::string& Regex) throw();
bool RegexNotNone_Throw(const std::string& Str, const std::string& Regex);
