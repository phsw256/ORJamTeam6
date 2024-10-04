#include "ORTest.h"
#include "Global.h"

std::pair<DWORD, DWORD> MilliToMinSec(DWORD Milli)
{
    return { Milli / 60000,(Milli / 1000) % 60 };
}

void ORStage_MusicPlayer::DrawMenuElem(std::string& Str, int RelIdx, int AbsIdx, DWORD pList)
{
    ImGui::TextWrapped(Str.c_str());
    ImGui::SameLine();
    if (ImGui::ArrowButton(Str.c_str(), ImGuiDir_Right))
        ((ORAsyncPlayList*)pList)->SwitchTo(AbsIdx);
}
void ORStage_MusicPlayer::DrawUI()
{
    if (PlayList.IsPlaying())
    {
        ImGui::Button(u8"结束播放");
        ImGui::TextWrapped(u8"当前播放：%s", PlayList.GetCurrentMusicName().data());
        Menu.DrawUI();
        auto [A, B] = MilliToMinSec(PlayList.CurrentTime());
        auto [C, D] = MilliToMinSec(PlayList.TotalTime());
        ImGui::TextWrapped("%d:%d / %d:%d", A, B, C, D);
        if (ImGui::Button(u8"暂停"))PlayList.PauseCurrent(); ImGui::SameLine();
        if (ImGui::Button(u8"继续"))PlayList.ResumeCurrent();
        if (ImGui::Button(u8"停止"))PlayList.StopCurrent(); ImGui::SameLine();
        if (ImGui::Button(u8"播放"))PlayList.PlayCurrent();
        int PrevVS = VolumeSlider;
        ImGui::SliderInt(u8"音量", &VolumeSlider, 0, 1000);
        if (PrevVS != VolumeSlider)
        {
            PlayList.SetVolume(VolumeSlider);
        }
        if (ImGui::Button(PlayList.GetCurrentSwitchModeStr()))
        {
            auto i = (int)PlayList.GetCurrentSwitchMode();
            i = (i + 1) % 3;
            PlayList.SetSwitchMode(ORAsyncPlayList::SwitchMode(i));
        }

    }
    else
    {
        ImGui::Button(u8"开始播放");
    }
}
void ORStage_MusicPlayer::EventLoop()
{

}
void ORStage_MusicPlayer::OnSwitched()
{

}
ORStage_MusicPlayer::ORStage_MusicPlayer(const _UTF8 std::string_view StageName) :
    ORStage(StageName), Menu(PlayList.GetSequence(),std::string(StageName), DrawMenuElem, (DWORD)&PlayList) {}

namespace ORTest
{
    void Init()
    {
        auto ps = std::make_unique<ORStage_MusicPlayer>(u8"播放器");
        ps->AddToList("Land Of No Justice", L"lonj.mp3", false);
        ps->AddToList("It's Happening", L"ih.mid", false);
        ps->AddToList("One Final Chance", L"ofc.mid", false);
        WorkSpace.TopBar.AddStage(std::move(ps));
    }
    void Loop()
    {
        ImGui::SetNextWindowSize({ FontHeight * 30.0F,FontHeight * 60.0F });
        ImGui::SetNextWindowPos({ 0.0F,FontHeight * 5.0F });
        ImGui::Begin("ORTest", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse );
        WorkSpace.TopBar.DrawUI();
        auto pStage = WorkSpace.TopBar.GetCurrentStage();
        if (pStage)pStage->DrawUI();
        ImGui::End();
    }
    void CleanUp()
    {

    }
}
