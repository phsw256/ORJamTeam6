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
        if (ImGui::Button(u8"��������"))
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
            ImGui::TextWrapped(u8"��ǰ���ţ�(0/%d)", PlayList.GetSequence().size());
            ImGui::SameLine(); InvisibleArrow("_PREV");
            Menu.DrawUI();
            ImGui::SliderInt("", &TimeSlider, 0, 0, "");
            ImGui::TextWrapped("00:00 / 00:00");
            ImGui::NewLine();
        }
        else
        {
            ImGui::TextWrapped(u8"��ǰ���ţ�(%d/%d) %s", ResultBuffer[0] + 1,
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
            if (ImGui::SmallButton(u8"��ͣ"))PlayList.PauseCurrent(); ImGui::SameLine();
            if (ImGui::SmallButton(u8"����"))PlayList.ResumeCurrent(); ImGui::SameLine();
            if (ImGui::SmallButton(u8"ֹͣ"))PlayList.StopCurrent(); ImGui::SameLine();
            if (ImGui::SmallButton(u8"����"))PlayList.PlayCurrent();
        }
        int PrevVS = VolumeSlider;
        ImGui::SliderInt(u8"����", &VolumeSlider, 0, 1000);
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
        if (ImGui::Button(u8"��ʼ����"))
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
    if (ImGui::Button(u8"�̳�"))
    {
        auto pGame = dynamic_cast<Stage_MainGame*>(WorkSpace.TopBar.GetStage(u8"��Ϸ"));
        if (pGame)pGame->InitRules(�̳̹�Rules{});
        WorkSpace.TopBar.ForceChangeStage(u8"��Ϸ");
    }
    helper.SetX();
    if (ImGui::Button(u8"��ʼ��Ϸ"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"��ʼ");
    }helper.SetX();
    if (ImGui::Button(u8"ѡ��"))
    {

    }helper.SetX();
    if (ImGui::Button(u8"�����Ŷ�"))
    {

    }helper.SetX();
    if (ImGui::Button(u8"�˳���Ϸ"))
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
    if (ImGui::RadioButton(u8"����ģʽ", Setting.MMPMode))Setting.MMPMode = !Setting.MMPMode;
    helper.SetX();
    if (ImGui::Button(u8"��ʼ��"))
    {
        auto pGame = dynamic_cast<Stage_MainGame*>(WorkSpace.TopBar.GetStage(u8"��Ϸ"));
        if (pGame)pGame->InitRules(Setting);
        WorkSpace.TopBar.ForceChangeStage(u8"��Ϸ");
    }helper.SetX();
    if (ImGui::Button(u8"����"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"���˵�");
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
        ImGui::TextWrapped(u8"������������������");
    }
    else
    {
        ImGui::TextWrapped(u8"����û������Ҳ������");
    }
    if (Rules->ITeachYouHowToPlayThisFuckingGame)
    {
        ImGui::TextWrapped(u8"���ڽ̳̹��������С���");
    }
    else
    {
        ImGui::TextWrapped(u8"�����㽶����������)�����ӽУ�");
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
    if (ImGui::Button(u8"ү��������"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"���˵�");
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
    if (ImGui::Button(u8"ү��������"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"���˵�");
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
    auto pGame = dynamic_cast<Stage_MainGame*>(WorkSpace.TopBar.GetStage(u8"��Ϸ"));
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
        auto ps = std::make_unique<ORStage_MusicPlayer>(u8"������");
        ps->AddToList("Land Of No Justice", L"lonj.mp3", false);
        ps->AddToList("One Final Chance", L"ofc.mp3", false);
        ps->AddToList("Premeditated Treachery", L"pt.mp3", false);
        ps->AddToList("It's Happening", L"ih.mp3", false);
        ps->Enable();
        WorkSpace.TopBar.AddStage(std::move(ps));

        auto p1 = std::make_unique<Stage_MainMenu>(u8"���˵�");
        p1->Enable();
        WorkSpace.TopBar.AddStage(std::move(p1));

        auto p2 = std::make_unique<Stage_ShellSelect>(u8"��ʼ");
        p2->Enable();
        WorkSpace.TopBar.AddStage(std::move(p2));

        auto p3 = std::make_unique<Stage_MainGame>(u8"��Ϸ");
        p3->Enable();
        WorkSpace.TopBar.AddStage(std::move(p3));

        WorkSpace.TopBar.ForceChangeStage(u8"���˵�");
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
