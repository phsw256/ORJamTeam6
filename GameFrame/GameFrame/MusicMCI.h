/************************************************************************
 * 作者 : 悠远的苍穹													*
 * QQ   : 2237505658													*
 * 邮箱 : 2237505658@qq.com												*
 * 最后修改  : 2020-8-17												*
 ************************************************************************/
#pragma once

#include "Include.h"
#include <windows.h>
#include <mmsystem.h>
#include <Digitalv.h>
#pragma comment(lib, "winmm.lib")

using std::wstring;
using std::vector;
using std::array;

/************************************************
 *                class MusicMCI				*
 * 音乐播放操作层								*
 * 进行底层音乐播放								*
 * 暂停，关闭，设置音量，获取播放时间等操作		*
 ************************************************/

class MusicMCI
{
private:
    MCIDEVICEID nDeviceID;									// 设备ID

public:
    MusicMCI()noexcept;										// 默认构造函数
    virtual ~MusicMCI();									        // 虚析构函数
    bool open(LPCWSTR music)noexcept;						// 打开音乐
    bool play()noexcept;									// 播放音乐
    bool pause()noexcept;									// 暂停音乐
    bool stop()noexcept;									// 停止播放
    bool close()noexcept;									// 关闭音乐
    bool getCurrentTime(DWORD& pos)noexcept;				// 获取当前播放时间
    bool getTotalTime(DWORD& time)noexcept;					// 获取音乐总时长
    bool setVolume(size_t volume)noexcept;					// 设置音量大小
    bool setStartTime(size_t start)noexcept;				// 设置播放位置
};

class ListedMusic:public MusicMCI
{
private:						
    std::wstring Name;
public:
    ListedMusic() = default;								
    virtual ~ListedMusic() = default;
    inline void SetName(LPCWSTR music)noexcept { Name = music; }
    inline bool open()noexcept { return MusicMCI::open(Name.c_str()); }
    inline bool open_play()noexcept { return open() && play(); }
    inline bool stop_close()noexcept { return stop() && close(); }
};
