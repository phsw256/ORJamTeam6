#pragma once
#include <Windows.h>
#include <string>
#include <atomic>
#include "Minimal.h"

class LogClass;
namespace Ini{ struct IniFile; }

extern LogClass GlobalLog;
extern BufString LogBuf;

extern std::string FontPath;//全过程不变
extern std::string Defaultpath;
extern std::string Pathbuf, Desktop;
extern std::wstring PathbufW;

extern Ini::IniFile Config;
extern int FontHeight;
extern int FPSLimit;
extern int ScrX, ScrY;
extern int FnInfoDelay;
extern int FnInfoEndDelay;

extern wchar_t CurrentDirW[5000];//调用当前目录时必须调用这两个，不要临时求！
extern char CurrentDirA[5000];//因为OpenFileDialog会改变当前目录，所以必须提前加载

extern bool EnableLog;
extern std::atomic<bool> FontLoaded;
extern std::atomic<bool> InMainLoop;

extern HWND MainWindowHandle;
