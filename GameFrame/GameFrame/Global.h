#pragma once
#include <Windows.h>
#include <string>
#include <atomic>
#include "Minimal.h"

class LogClass;
class ORWorkSpace;
class ORImage;
struct GLFWwindow;
struct ImFont;
namespace Ini{ struct IniFile; }
template<typename T>
class ORResourcePool;

extern LogClass GlobalLog;
extern BufString LogBuf;

extern std::string FontPath;//全过程不变
extern std::string Defaultpath;
extern std::string Pathbuf, Desktop;
extern std::wstring PathbufW;
extern std::string CurrentPathA, MusicPathA;
extern std::wstring CurrentPathW, MusicPathW;

extern Ini::IniFile Config;
extern int FontHeight;
extern int FPSLimit;
extern int ScrX, ScrY;
extern bool FullScreen;
extern int FnInfoDelay;
extern int FnInfoEndDelay;

extern wchar_t CurrentDirW[5000];//调用当前目录时必须调用这两个，不要临时求！
extern char CurrentDirA[5000];//因为OpenFileDialog会改变当前目录，所以必须提前加载

extern bool EnableLog;
extern std::atomic<bool> FontLoaded;
extern std::atomic<bool> InMainLoop;

extern HWND MainWindowHandle;

extern ORWorkSpace WorkSpace;
extern ORResourcePool<ORImage> ImagePool;

namespace PreLink
{
    extern GLFWwindow* window;
    extern HINSTANCE hInst;
    extern ImFont* font;
    void CleanUp();
}
