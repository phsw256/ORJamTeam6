
#include "FromEngine/Include.h"
#include "PreLink.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// 此回调函数为全局函数或静态函数;
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    switch (uMsg)
    {
    case BFFM_INITIALIZED:
    {
        ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
    }
    break;
    default:
        break;
    }
    return 0;
}

bool SelectFilePath(HWND father, std::string& strFilePath, const std::string& FromPath)//No Unicode
{
    CHAR szPathName[MAX_PATH] = { 0 };
    BROWSEINFOA bInfo = { 0 };
    bInfo.hwndOwner = father;
    bInfo.lpszTitle = "选择目录";
    bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI
        | BIF_UAHINT | BIF_SHAREABLE;
    // 关于更多的 ulFlags 参考 http://msdn.microsoft.com/en-us/library/bb773205(v=vs.85).aspx;
    bInfo.lpfn = (BrowseCallbackProc);
    bInfo.lParam = (LPARAM)(LPCTSTR)(FromPath.c_str());// 注意路径中不要带'\..\'或'\.\'符号，否则设置默认路径失败;

    LPITEMIDLIST lpDlist;
    lpDlist = SHBrowseForFolderA(&bInfo);
    if (nullptr == lpDlist) // 单击了取消或×按钮;
    {
        strFilePath.clear();
        return false;
    }
    SHGetPathFromIDListA(lpDlist, szPathName);
    strFilePath = szPathName;
    return true;
}


#include "ORImage.h"


int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    using namespace PreLink;
    int RScrX{}, RScrY{};


    GlobalLog.ClearLog();
    GlobalLog.AddLog_CurTime(false);
    GlobalLog.AddLog((u8"INI浏览器 V" + Version).c_str());
    GlobalLog.AddLog_CurTime(false);
    GlobalLog.AddLog(u8"INI浏览器已开始运行。");

    FontLoaded.store(false);

    hInst = hInstance;
    //auto _lpwCmdLine = L"/800*0<FFFFFF>";a
    auto _lpwCmdLine = lpCmdLine;

    RScrX = GetSystemMetrics(SM_CXSCREEN);
    RScrY = GetSystemMetrics(SM_CYSCREEN);
    int llg = 0;
    swscanf(_lpwCmdLine, L"%d/%d*%d", &llg, &ScrX, &ScrY);
    if (llg)EnableLog = true;
    else EnableLog = false;

    if (ScrX == 0)ScrX = RScrX >> 1;
    if (ScrX == 1)ScrX = RScrX;
    if (!ScrY)ScrY = RScrY;

    ::srand((unsigned)::time(NULL));
    GlobalAnalyze::rnd = std::default_random_engine{ (unsigned)::time(NULL) };
    if (EnableLog)
    {
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog(u8"设置随机数引擎。");

        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog((u8"读入输入参数 X=" + std::to_string(ScrX) + u8" Y=" + std::to_string(ScrY)).c_str());
    }

    char DesktopTmp[300];
    SHGetSpecialFolderPathA(0, DesktopTmp, CSIDL_DESKTOPDIRECTORY, 0);
    GetCurrentDirectoryW(MAX_PATH, CurrentDirW);
    GetCurrentDirectoryA(MAX_PATH, CurrentDirA);
    Defaultpath = Desktop = DesktopTmp;
    CurrentPathA = CurrentDirA;
    CurrentPathW = CurrentDirW;
    MusicPathA = CurrentPathA + "\\Resources\\Music";
    MusicPathW = CurrentPathW + L"\\Resources\\Music";

    int PreLoopRet = PreLoop1();
    if (PreLoopRet)return PreLoopRet;


    if (EnableLog)
    {
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog(u8"渲染初始化完成。");
    }

    PreLoop2();

    while (!FontLoaded.load());
    glfwShowWindow(PreLink::window);
    SetClassLong(MainWindowHandle, GCL_HICON, (LONG)LoadIcon(PreLink::hInst, MAKEINTRESOURCE(IDI_ICON1)));

    ShellLoop();
    CleanUp();

    return 0;
}

