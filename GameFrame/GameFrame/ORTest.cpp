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


void Stage_MainMenu::DrawUI()
{
    auto helper = PosSetHelper(WindowRelPos({0.4F,0.4F}));
    if (ImGui::Button(u8"教程"))
    {
        auto pGame = dynamic_cast<Stage_MainGame*>(WorkSpace.TopBar.GetStage(u8"游戏"));
        if (pGame)pGame->InitRules(教程关Rules{});
        WorkSpace.TopBar.ForceChangeStage(u8"游戏");
    }
    helper.SetX();
    if (ImGui::Button(u8"开始游戏"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"开始");
    }helper.SetX();
    if (ImGui::Button(u8"选项"))
    {

    }helper.SetX();
    if (ImGui::Button(u8"制作团队"))
    {

    }helper.SetX();
    if (ImGui::Button(u8"退出游戏"))
    {
        glfwSetWindowShouldClose(PreLink::window, 1);
    }
}
void Stage_MainMenu::EventLoop()
{

}
void Stage_MainMenu::OnSwitched()
{

}
Stage_MainMenu::Stage_MainMenu(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

}


void Stage_ShellSelect::DrawUI()
{
    auto helper = PosSetHelper(WindowRelPos({ 0.4F,0.4F }));
    if (ImGui::RadioButton(u8"天灾模式", Setting.MMPMode))Setting.MMPMode = !Setting.MMPMode;
    helper.SetX();
    if (ImGui::Button(u8"开始！"))
    {
        auto pGame = dynamic_cast<Stage_MainGame*>(WorkSpace.TopBar.GetStage(u8"游戏"));
        if (pGame)pGame->InitRules(Setting);
        WorkSpace.TopBar.ForceChangeStage(u8"游戏");
    }helper.SetX();
    if (ImGui::Button(u8"返回"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"主菜单");
    }
}
void Stage_ShellSelect::EventLoop()
{

}
void Stage_ShellSelect::OnSwitched()
{

}
Stage_ShellSelect::Stage_ShellSelect(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

}


void Stage_MainGame::DrawUI()
{
    if (Rules->Setting.MMPMode)
    {
        ImGui::TextWrapped(u8"天灾来辣！你似辣！");
    }
    else
    {
        ImGui::TextWrapped(u8"天灾没来！你也似辣！");
    }
    if (Rules->ITeachYouHowToPlayThisFuckingGame)
    {
        ImGui::TextWrapped(u8"正在教程关里面爬行……");
    }
    else
    {
        ImGui::TextWrapped(u8"（吃香蕉）（爬树藤)（猴子叫）");
    }
}
void Stage_MainGame::EventLoop()
{

}
void Stage_MainGame::OnSwitched()
{

}
Stage_MainGame::Stage_MainGame(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

}

void Stage_InGameOptions::DrawUI()
{
    if (ImGui::Button(u8"爷不玩辣！"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"主菜单");
    }
}
void Stage_InGameOptions::EventLoop()
{

}
void Stage_InGameOptions::OnSwitched()
{

}
Stage_InGameOptions::Stage_InGameOptions(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

}

void Stage_TechTree::DrawUI()
{
    /*
    if (ImGui::Button(u8"爷不玩辣！"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"主菜单");
    }
    ImGui::Button(u8"",);
    {
        auto helper = PosSetHelper(WindowRelPos({ 0.4F,0.4F }));
    }
    ImGui::BeginListBox();
    auto pImg=WorkSpace.ImagePool.GetResource("1232131");
    pImg->Draw();
    ImGui::ImageButton(pImg->GetID(), pImg->GetSize());
    ImGui::GetBackgroundDrawList();
    auto pGame = dynamic_cast<Stage_MainGame*>(WorkSpace.TopBar.GetStage(u8"游戏"));
    if (pGame)pGame->InitRules(Setting);
    */
}
void Stage_TechTree::EventLoop()
{

}
void Stage_TechTree::OnSwitched()
{

}
Stage_TechTree::Stage_TechTree(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

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

        auto p1 = std::make_unique<Stage_MainMenu>(u8"主菜单");
        p1->Enable();
        WorkSpace.TopBar.AddStage(std::move(p1));

        auto p2 = std::make_unique<Stage_ShellSelect>(u8"开始");
        p2->Enable();
        WorkSpace.TopBar.AddStage(std::move(p2));

        auto p3 = std::make_unique<Stage_MainGame>(u8"游戏");
        p3->Enable();
        WorkSpace.TopBar.AddStage(std::move(p3));

        WorkSpace.TopBar.ForceChangeStage(u8"主菜单");
    }
    void Loop()
    {
        /*
        ImGui::SetNextWindowSize({ FontHeight * 30.0F,FontHeight * 60.0F });
        ImGui::SetNextWindowPos({ 0.0F,FontHeight * 5.0F });
        ImGui::SetNextWindowFocus();
        ImGui::Begin("ORTest", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse );
        WorkSpace.TopBar.DrawUI();
        auto pStage = WorkSpace.TopBar.GetCurrentStage();
        if (pStage)pStage->DrawUI();
        ImGui::End();
        */
    }
    void CleanUp()
    {

    }
}
