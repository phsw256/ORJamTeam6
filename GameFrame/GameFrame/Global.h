#pragma once
#include <Windows.h>
#include <string>
#include <atomic>
#include "Minimal.h"

class LogClass;
namespace Ini{ struct IniFile; }

extern LogClass GlobalLog;
extern BufString LogBuf;

extern std::string FontPath;//ȫ���̲���
extern std::string Defaultpath;
extern std::string Pathbuf, Desktop;
extern std::wstring PathbufW;

extern Ini::IniFile Config;
extern int FontHeight;
extern int FPSLimit;
extern int ScrX, ScrY;
extern int FnInfoDelay;
extern int FnInfoEndDelay;

extern wchar_t CurrentDirW[5000];//���õ�ǰĿ¼ʱ�����������������Ҫ��ʱ��
extern char CurrentDirA[5000];//��ΪOpenFileDialog��ı䵱ǰĿ¼�����Ա�����ǰ����

extern bool EnableLog;
extern std::atomic<bool> FontLoaded;
extern std::atomic<bool> InMainLoop;

extern HWND MainWindowHandle;
