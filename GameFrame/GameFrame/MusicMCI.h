/************************************************************************
 * ���� : ��Զ�Ĳ��													*
 * QQ   : 2237505658													*
 * ���� : 2237505658@qq.com												*
 * ����޸�  : 2020-8-17												*
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
 * ���ֲ��Ų�����								*
 * ���еײ����ֲ���								*
 * ��ͣ���رգ�������������ȡ����ʱ��Ȳ���		*
 ************************************************/

class MusicMCI
{
private:
    MCIDEVICEID nDeviceID;									// �豸ID

public:
    MusicMCI()noexcept;										// Ĭ�Ϲ��캯��
    ~MusicMCI();									        // ����������
    bool open(LPCWSTR music)noexcept;						// ������
    bool play()noexcept;									// ��������
    bool pause()noexcept;									// ��ͣ����
    bool stop()noexcept;									// ֹͣ����
    bool close()noexcept;									// �ر�����
    bool getCurrentTime(DWORD& pos)noexcept;				// ��ȡ��ǰ����ʱ��
    bool getTotalTime(DWORD& time)noexcept;					// ��ȡ������ʱ��
    bool setVolume(size_t volume)noexcept;					// ����������С
    bool setStartTime(size_t start)noexcept;				// ���ò���λ��
};
