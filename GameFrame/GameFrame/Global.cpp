#include "Include.h"
LogClass GlobalLog{ "browser.log" };
BufString LogBuf;

std::string FontPath = ".\\Resources\\";//全过程不变
std::string Defaultpath{ "" };
std::string Pathbuf, Desktop;
std::wstring PathbufW;

Ini::IniFile Config;
int FontHeight = 16;
int FPSLimit = -1;
int ScrX, ScrY;
int FnInfoDelay = 1000, FnInfoEndDelay = 5000;

wchar_t CurrentDirW[5000];//调用当前目录时必须调用这两个，不要临时求！
char CurrentDirA[5000];//因为OpenFileDialog会改变当前目录，所以必须提前加载

bool EnableLog = true;
std::atomic<bool> FontLoaded = false;
std::atomic<bool> InMainLoop = false;

HWND MainWindowHandle = 0;
