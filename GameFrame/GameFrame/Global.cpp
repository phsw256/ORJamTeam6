#include "Include.h"
LogClass GlobalLog{ "browser.log" };
BufString LogBuf;

std::string FontPath = ".\\Resources\\";//ȫ���̲���
std::string Defaultpath{ "" };
std::string Pathbuf, Desktop;
std::wstring PathbufW;

Ini::IniFile Config;
int FontHeight = 16;
int FPSLimit = -1;
int ScrX, ScrY;
int FnInfoDelay = 1000, FnInfoEndDelay = 5000;

wchar_t CurrentDirW[5000];//���õ�ǰĿ¼ʱ�����������������Ҫ��ʱ��
char CurrentDirA[5000];//��ΪOpenFileDialog��ı䵱ǰĿ¼�����Ա�����ǰ����

bool EnableLog = true;
std::atomic<bool> FontLoaded = false;
std::atomic<bool> InMainLoop = false;

HWND MainWindowHandle = 0;
