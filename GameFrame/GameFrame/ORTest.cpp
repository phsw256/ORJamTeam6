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
        if (ImGui::Button(u8"结束播放"))
        {
            PlayList.EndPlay();
            ResultBuffer.clear();
            return;
        }
        if (PlayList.ReceivedCount() >= 3 && RefreshRate.NextFrame())
        {
            ResultBuffer = PlayList.WaitForResult(3);
            PlayList.AsyncCurrentMusicIdx();
            PlayList.AsyncCurrentTime();
            PlayList.AsyncTotalTime();
        }
        if (ResultBuffer.empty())
        {
            ImGui::TextWrapped(u8"当前播放：(0/%d)", PlayList.GetSequence().size());
            ImGui::SameLine(); InvisibleArrow("_PREV");
            Menu.DrawUI();
            ImGui::SliderInt("", &TimeSlider, 0, 0, "");
            ImGui::TextWrapped("00:00 / 00:00");
            ImGui::NewLine();
        }
        else
        {
            ImGui::TextWrapped(u8"当前播放：(%d/%d) %s", ResultBuffer[0] + 1,
                PlayList.GetSequence().size(), PlayList.GetSequence().at(ResultBuffer[0]).c_str());
            ImGui::SameLine();
            if (ImGui::ArrowButton("_PREV", ImGuiDir_Left))
                PlayList.PlayPrev(true);
            ImGui::SameLine();
            if (ImGui::ArrowButton("_NEXT", ImGuiDir_Right))
                PlayList.PlayNext(true);
            Menu.DrawUI();
            auto [A, B] = MilliToMinSec(ResultBuffer[1]);
            auto [C, D] = MilliToMinSec(ResultBuffer[2]);
            TimeSlider = ResultBuffer[1];
            ImGui::SliderInt("", &TimeSlider, 0, ResultBuffer[2], "");
            if (TimeSlider != ResultBuffer[1])
                PlayList.SetCurrentStartTime(TimeSlider);
            TimeSlider = ResultBuffer[1];
            ImGui::TextWrapped("%02d:%02d / %02d:%02d", A, B, C, D);
            if (ImGui::SmallButton(u8"暂停"))PlayList.PauseCurrent(); ImGui::SameLine();
            if (ImGui::SmallButton(u8"继续"))PlayList.ResumeCurrent(); ImGui::SameLine();
            if (ImGui::SmallButton(u8"停止"))PlayList.StopCurrent(); ImGui::SameLine();
            if (ImGui::SmallButton(u8"播放"))PlayList.PlayCurrent();
        }
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
        if (ImGui::Button(u8"开始播放"))
        {
            PlayList.StartPlay();
            PlayList.AsyncCurrentMusicIdx();
            PlayList.AsyncCurrentTime();
            PlayList.AsyncTotalTime();
        }
    }
}
void ORStage_MusicPlayer::EventLoop()
{

}
void ORStage_MusicPlayer::OnSwitched()
{

}
ORStage_MusicPlayer::ORStage_MusicPlayer(const _UTF8 std::string_view StageName) :
    ORStage(StageName), Menu(PlayList.GetSequence(),std::string(StageName), DrawMenuElem, (DWORD)&PlayList), RefreshRate(30)
{
    PlayList.SetVolume(VolumeSlider);
}

namespace ORTest
{
    MusicMCI ms1;
    void Init()
    {
        auto ps = std::make_unique<ORStage_MusicPlayer>(u8"播放器");
        ps->AddToList("Land Of No Justice", L"lonj.mp3", false);
        ps->AddToList("One Final Chance", L"ofc.mp3", false);
        ps->AddToList("Premeditated Treachery", L"pt.mp3", false);
        ps->AddToList("It's Happening", L"ih.mp3", false);
        ps->Enable();
        WorkSpace.TopBar.AddStage(std::move(ps));
        //ms1.play();
    }
    void Loop()
    {
        ImGui::SetNextWindowSize({ FontHeight * 30.0F,FontHeight * 60.0F });
        ImGui::SetNextWindowPos({ 0.0F,FontHeight * 5.0F });
        ImGui::SetNextWindowFocus();
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
