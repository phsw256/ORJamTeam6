#include "ORTest.h"
#include "Global.h"
#include "RoundData.h"
#include "ORException.h"

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
    if (ImGui::Button(u8"����"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"ѡ��");
    }
    for (size_t i = 0; i < std::size(Blink); i++)
    {
        if (Blink[i].Available())
        {
            //if (!Blink[i].IsPlaying())Blink[i].Play();
            Blink[i].Draw();
        }
    }
    WorkSpace.ImagePool.GetResource("MISSING")->Draw();
}
void ORStage_MusicPlayer::EventLoop()
{

}
void ORStage_MusicPlayer::OnSwitched()
{
    if (!Blink[0].Available())
    {
        for (size_t i = 0; i < std::size(Blink); i++)
        {
            Blink[i].Reset(WorkSpace.AnimPool.GetResource("Blink1"));
            Blink[i].SetFrame(i);
            Blink[i].Play();
        }
    }
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
        auto pSet = WorkSpace.TopBar.GetStage_Typed<Stage_Setting>(u8"ѡ��");
        if (pSet)pSet->SwitchInMenu();
        WorkSpace.TopBar.ForceChangeStage(u8"ѡ��");
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
    auto pPlayer = WorkSpace.TopBar.GetStage_Typed<ORStage_MusicPlayer>(u8"������");
    if (pPlayer)
    {
        auto& List = pPlayer->GetList();
        if (!List.IsPlaying())
        {
            List.StartPlay();
            List.SetPlayFromFirst();
            List.SetVolume(InitialVolume);
        }
    }
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
        auto pGame = WorkSpace.TopBar.GetStage_Typed<Stage_MainGame>(u8"��Ϸ");
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

void Stage_Setting::DrawUI()
{
    ImGui::Text(u8"���ã�");
    if (ImGui::Button(u8"����"))
    {
        if(ToMainMenu)
            WorkSpace.TopBar.ForceChangeStage(u8"���˵�");
        else
            WorkSpace.TopBar.ForceChangeStage(u8"��Ϸѡ��");
    }
    if (ImGui::Button(u8"��������"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"������");
    }
}
void Stage_Setting::EventLoop()
{

}
void Stage_Setting::OnSwitched()
{

}
Stage_Setting::Stage_Setting(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

}
void Stage_Setting::SwitchInGame()
{
    ToMainMenu = false;
}
void Stage_Setting::SwitchInMenu()
{
    ToMainMenu = true;
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
    if (ImGui::Button(u8"�Ƽ���"))
    {
        Pause();
        WorkSpace.TopBar.ForceChangeStage(u8"�Ƽ���");
    }
    if (ImGui::Button(u8"ѡ��"))
    {
        Pause();
        WorkSpace.TopBar.ForceChangeStage(u8"��Ϸѡ��");
    }
    auto& Set = TileMap.GetDrawSetting();
    Set.DrawBorder = ImRect(ImVec2{ 50.0F,150.0F }, ImGui::GetWindowSize() - ImVec2{ 50.0F, 80.0F });
    Set.ProcessBorder = ImRect({ 0.0F,0.0F }, ImGui::GetWindowSize());
    Set.DrawTarget = ImGui::GetBackgroundDrawList();
    Set.CenterDrawPos = WindowRelPos({ 0.5F,0.5F });
    InvisibleArrow("1"); ImGui::SameLine();
    if (ImGui::ArrowButton("UP", ImGuiDir_Up))TileMap.MoveView({ -0.2F,-0.2F });
    if (ImGui::ArrowButton("LEFT", ImGuiDir_Left))TileMap.MoveView({ -0.2F,0.2F });
    ImGui::SameLine(); InvisibleArrow("2"); ImGui::SameLine();
    if (ImGui::ArrowButton("RIGHT", ImGuiDir_Right))TileMap.MoveView({ 0.2F,-0.2F });
    InvisibleArrow("3"); ImGui::SameLine();
    if (ImGui::ArrowButton("DOWN", ImGuiDir_Down))TileMap.MoveView({ 0.2F,0.2F });
    TileMap.DrawUI();
}
void Stage_MainGame::InitTileMap()
{
    TileMap.RandomGenerate(60, 60, WorkSpace.ImagePool, { "Tile1","Tile2","Tile3","Tile4" });
    auto& Set = TileMap.GetDrawSetting();
    Set.CenterMapPos = { 30,30 };
    Set.SizeRatio = { 128,64 };
}
void Stage_MainGame::InitTechTree()
{
    Tree = WorkSpace.TopBar.GetStage_Typed<Stage_TechTree>(u8"�Ƽ���");
    Tree->Init();
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
void Stage_MainGame::ExitGame()
{

}
void Stage_MainGame::Pause()
{

}
void Stage_MainGame::Resume()
{
    
}

void Stage_InGameOptions::DrawUI()
{
    if (!Main)return;
    if (ImGui::Button(u8"ү��������"))
    {
        Main->ExitGame();
        WorkSpace.TopBar.ForceChangeStage(u8"���˵�");
    }
    if (ImGui::Button(u8"ѡ��"))
    {
        auto pSet = WorkSpace.TopBar.GetStage_Typed<Stage_Setting>(u8"ѡ��");
        if (pSet)pSet->SwitchInGame();
        WorkSpace.TopBar.ForceChangeStage(u8"ѡ��");
    }
    if (ImGui::Button(u8"�����棡"))
    {
        Main->Resume();
        WorkSpace.TopBar.ForceChangeStage(u8"��Ϸ");
    }
}
void Stage_InGameOptions::EventLoop()
{
    if (!Main)
    {
        Main = WorkSpace.TopBar.GetStage_Typed<Stage_MainGame>(u8"��Ϸ");
    }
}
void Stage_InGameOptions::OnSwitched()
{

}
Stage_InGameOptions::Stage_InGameOptions(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

}

void Stage_TechTree::DrawUI()
{
    if (!Main)return;
    if (ImGui::Button(u8"����"))
    {
        Main->Resume();
        WorkSpace.TopBar.ForceChangeStage(u8"��Ϸ");
    }
    int i = 0;
    for (auto& op : Opts)
    {
        if (ImGui::Button(op.Name.c_str()))
        {
            CurOpt = i;
        }
        ImGui::SameLine();
        i++;
    }
    auto pt = Opts[CurOpt].pTree;
    auto& Set = pt->GetMap().GetDrawSetting();
    Set.DrawBorder = ImRect(ImVec2{ 50.0F,150.0F }, ImGui::GetWindowSize() - ImVec2{ 50.0F, 80.0F });
    Set.ProcessBorder = ImRect({ 0.0F,0.0F }, ImGui::GetWindowSize());
    Set.DrawTarget = ImGui::GetBackgroundDrawList();
    Set.CenterDrawPos = ImVec2{ 50.0F,150.0F };
    //InvisibleArrow("1"); ImGui::SameLine();
    //if (ImGui::ArrowButton("UP", ImGuiDir_Up))pt->GetMap().MoveView({ -0.2F,-0.2F });
    if (ImGui::ArrowButton("LEFT", ImGuiDir_Left))pt->GetMap().MoveView({ -0.4F,0.0F });
    ImGui::SameLine(); InvisibleArrow("2"); ImGui::SameLine();
    if (ImGui::ArrowButton("RIGHT", ImGuiDir_Right))pt->GetMap().MoveView({ 0.4F,0.0F });
    //InvisibleArrow("3"); ImGui::SameLine();
    //if (ImGui::ArrowButton("DOWN", ImGuiDir_Down))pt->GetMap().MoveView({ 0.2F,0.2F });
    if (Opts[CurOpt].BgCol.Value.w > 0.001)pt->GetMap().FillDrawArea(Opts[CurOpt].BgCol);
    pt->DrawUI();
}
void Stage_TechTree::Init()
{
    try {
        ORJsonSource ImageSrc;
        ImageSrc.ParseFromFile(".\\Resources\\TutorialTech.json");
        if (!ImageSrc.LoadObject(u8"Body_TechTree", BodyTechTree))throw ORException("����������ʧ�ܣ�");
        if (!ImageSrc.LoadObject(u8"Culture_TechTree", CulTechTree))throw ORException("�Ļ�������ʧ�ܣ�");
        if (!ImageSrc.LoadObject(u8"Science_TechTree", SciTechTree))throw ORException("�Ƽ�������ʧ�ܣ�");
        std::vector<ImColor> Colors;
        if (ImageSrc.LoadObject(u8"Colors", Colors))
            for (size_t i = 0; i < std::min(Colors.size(),std::size(Opts)); i++)
                Opts[i].BgCol = Colors[i];
        else throw ORException("����ɫ����ʧ�ܣ�");
    }
    catch (ORException& e)
    {
        MessageBoxW(MainWindowHandle, UTF8toUnicode(e.what()).c_str(), L"���������", MB_OK);
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog(u8"���������");
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog(e.what());
        glfwSetWindowShouldClose(PreLink::window, 1);
    }
    for (auto& op : Opts)
    {
        auto& Set = op.pTree->GetMap().GetDrawSetting();
    }
}
void Stage_TechTree::EventLoop()
{
    if (!Main)
    {
        Main = WorkSpace.TopBar.GetStage_Typed<Stage_MainGame>(u8"��Ϸ");
    }
}
void Stage_TechTree::OnSwitched()
{

}
Stage_TechTree::Stage_TechTree(const _UTF8 std::string_view StageName) : ORStage(StageName)
{

}

template<typename T>
void AddEnabledStage(const char* Name)
{
    auto p1 = std::make_unique<T>(Name);
    p1->Enable();
    WorkSpace.TopBar.AddStage(std::move(p1));
}

namespace ORTest
{
    MusicMCI ms1;
    void Init()
    {
        auto ps = std::make_unique<ORStage_MusicPlayer>(u8"������");
        ps->AddToList("Land Of No Justice", (MusicPathW + L"\\lonj.mp3").c_str(), false);
        ps->AddToList("One Final Chance", (MusicPathW + L"\\ofc.mp3").c_str(), false);
        ps->AddToList("Premeditated Treachery", (MusicPathW + L"\\pt.mp3").c_str(), false);
        ps->AddToList("It's Happening", (MusicPathW + L"\\ih.mp3").c_str(), false);
        ps->Enable();
        WorkSpace.TopBar.AddStage(std::move(ps));

        AddEnabledStage<Stage_MainMenu>(u8"���˵�");
        AddEnabledStage<Stage_Setting>(u8"ѡ��");
        AddEnabledStage<Stage_ShellSelect>(u8"��ʼ");
        AddEnabledStage<Stage_MainGame>(u8"��Ϸ");
        AddEnabledStage<Stage_InGameOptions>(u8"��Ϸѡ��");
        AddEnabledStage<Stage_TechTree>(u8"�Ƽ���");

        WorkSpace.TopBar.ForceChangeStage(u8"���˵�");
        try{
            ORJsonSource ImageSrc;
            ImageSrc.ParseFromFile(".\\Resources\\Image.json");
            if (!ImageSrc.LoadObject(WorkSpace.ImagePool))throw ORException("ͼƬ������ʧ�ܣ�");
        }
        catch (ORException& e)
        {
            MessageBoxW(MainWindowHandle, UTF8toUnicode(e.what()).c_str(), L"ͼƬ���������", MB_OK);
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(u8"ͼƬ���������");
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(e.what());
            glfwSetWindowShouldClose(PreLink::window, 1);
        }
        WorkSpace.MissingImage = WorkSpace.ImagePool.GetResource("MISSING");
        try {
            ORJsonSource ImageSrc;
            ImageSrc.ParseFromFile(".\\Resources\\Anim.json");
            ORResourcePool<ORClipAnimType> ClipPool;
            if (!ImageSrc.LoadObject("ClipAnims", ClipPool))throw ORException("��Ƭ����������ʧ�ܣ�");
            WorkSpace.AnimPool.Merge(ClipPool);
        }
        catch (ORException& e)
        {
            MessageBoxW(MainWindowHandle, UTF8toUnicode(e.what()).c_str(), L"�������������", MB_OK);
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(u8"�������������");
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(e.what());
            glfwSetWindowShouldClose(PreLink::window, 1);
        }
    }
    void Loop()
    {
        WorkSpace.TopBar.EventLoop();
        //WorkSpace.TopBar.DrawUI();
        auto pStage = WorkSpace.TopBar.GetCurrentStage();
        if (pStage)pStage->DrawUI();
    }
    void CleanUp()
    {

    }
}
