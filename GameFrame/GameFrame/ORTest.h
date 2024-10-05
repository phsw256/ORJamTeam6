#pragma once
#include "ORComponent.h"
#include "ORBackType.h"

class ORStage_MusicPlayer : public ORStage
{
private:
    static void DrawMenuElem(std::string&, int RelIdx, int AbsIdx, DWORD pList);
    ORAsyncPlayList PlayList;
    ORListMenu<std::string> Menu;
    int VolumeSlider{ 800 };
    int TimeSlider{ 0 };
    std::vector<DWORD>ResultBuffer;
    RateClass RefreshRate;
public:
    virtual ~ORStage_MusicPlayer() = default;
    virtual void DrawUI();
    virtual void EventLoop();
    virtual void OnSwitched();
    ORStage_MusicPlayer(const _UTF8 std::string_view StageName);

    inline bool AddToList(const std::string_view ID, LPCWSTR music, bool Replace)
    {
        return PlayList.AddToList(ID, music, Replace);
    }
};

namespace ORTest
{
    void Init();
    void Loop();
    void CleanUp();
}
