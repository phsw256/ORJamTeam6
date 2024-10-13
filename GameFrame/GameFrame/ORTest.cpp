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
    if (ImGui::Button(u8"返回"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"选项");
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
        auto pSet = WorkSpace.TopBar.GetStage_Typed<Stage_Setting>(u8"选项");
        if (pSet)pSet->SwitchInMenu();
        WorkSpace.TopBar.ForceChangeStage(u8"选项");
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
    auto pPlayer = WorkSpace.TopBar.GetStage_Typed<ORStage_MusicPlayer>(u8"播放器");
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
    if (ImGui::RadioButton(u8"天灾模式", Setting.MMPMode))Setting.MMPMode = !Setting.MMPMode;
    helper.SetX();
    if (ImGui::Button(u8"开始！"))
    {
        auto pGame = WorkSpace.TopBar.GetStage_Typed<Stage_MainGame>(u8"游戏");
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

void Stage_Setting::DrawUI()
{
    ImGui::Text(u8"设置！");
    if (ImGui::Button(u8"返回"))
    {
        if(ToMainMenu)
            WorkSpace.TopBar.ForceChangeStage(u8"主菜单");
        else
            WorkSpace.TopBar.ForceChangeStage(u8"游戏选项");
    }
    if (ImGui::Button(u8"背景音乐"))
    {
        WorkSpace.TopBar.ForceChangeStage(u8"播放器");
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
    if (ImGui::Button(u8"科技树"))
    {
        Pause();
        WorkSpace.TopBar.ForceChangeStage(u8"科技树");
    }
    if (ImGui::Button(u8"选项"))
    {
        Pause();
        WorkSpace.TopBar.ForceChangeStage(u8"游戏选项");
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
    Tree = WorkSpace.TopBar.GetStage_Typed<Stage_TechTree>(u8"科技树");
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
    if (ImGui::Button(u8"爷不玩辣！"))
    {
        Main->ExitGame();
        WorkSpace.TopBar.ForceChangeStage(u8"主菜单");
    }
    if (ImGui::Button(u8"选项"))
    {
        auto pSet = WorkSpace.TopBar.GetStage_Typed<Stage_Setting>(u8"选项");
        if (pSet)pSet->SwitchInGame();
        WorkSpace.TopBar.ForceChangeStage(u8"选项");
    }
    if (ImGui::Button(u8"接着玩！"))
    {
        Main->Resume();
        WorkSpace.TopBar.ForceChangeStage(u8"游戏");
    }
}
void Stage_InGameOptions::EventLoop()
{
    if (!Main)
    {
        Main = WorkSpace.TopBar.GetStage_Typed<Stage_MainGame>(u8"游戏");
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
    if (ImGui::Button(u8"返回"))
    {
        Main->Resume();
        WorkSpace.TopBar.ForceChangeStage(u8"游戏");
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
        if (!ImageSrc.LoadObject(u8"Body_TechTree", BodyTechTree))throw ORException("体质树载入失败！");
        if (!ImageSrc.LoadObject(u8"Culture_TechTree", CulTechTree))throw ORException("文化树载入失败！");
        if (!ImageSrc.LoadObject(u8"Science_TechTree", SciTechTree))throw ORException("科技树载入失败！");
        std::vector<ImColor> Colors;
        if (ImageSrc.LoadObject(u8"Colors", Colors))
            for (size_t i = 0; i < std::min(Colors.size(),std::size(Opts)); i++)
                Opts[i].BgCol = Colors[i];
        else throw ORException("背景色载入失败！");
    }
    catch (ORException& e)
    {
        MessageBoxW(MainWindowHandle, UTF8toUnicode(e.what()).c_str(), L"树载入错误", MB_OK);
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog(u8"树载入错误");
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
        Main = WorkSpace.TopBar.GetStage_Typed<Stage_MainGame>(u8"游戏");
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
        auto ps = std::make_unique<ORStage_MusicPlayer>(u8"播放器");
        ps->AddToList("Land Of No Justice", (MusicPathW + L"\\lonj.mp3").c_str(), false);
        ps->AddToList("One Final Chance", (MusicPathW + L"\\ofc.mp3").c_str(), false);
        ps->AddToList("Premeditated Treachery", (MusicPathW + L"\\pt.mp3").c_str(), false);
        ps->AddToList("It's Happening", (MusicPathW + L"\\ih.mp3").c_str(), false);
        ps->Enable();
        WorkSpace.TopBar.AddStage(std::move(ps));

        AddEnabledStage<Stage_MainMenu>(u8"主菜单");
        AddEnabledStage<Stage_Setting>(u8"选项");
        AddEnabledStage<Stage_ShellSelect>(u8"开始");
        AddEnabledStage<Stage_MainGame>(u8"游戏");
        AddEnabledStage<Stage_InGameOptions>(u8"游戏选项");
        AddEnabledStage<Stage_TechTree>(u8"科技树");

        WorkSpace.TopBar.ForceChangeStage(u8"主菜单");
        try{
            ORJsonSource ImageSrc;
            ImageSrc.ParseFromFile(".\\Resources\\Image.json");
            if (!ImageSrc.LoadObject(WorkSpace.ImagePool))throw ORException("图片池载入失败！");
        }
        catch (ORException& e)
        {
            MessageBoxW(MainWindowHandle, UTF8toUnicode(e.what()).c_str(), L"图片池载入错误", MB_OK);
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(u8"图片池载入错误");
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(e.what());
            glfwSetWindowShouldClose(PreLink::window, 1);
        }
        WorkSpace.MissingImage = WorkSpace.ImagePool.GetResource("MISSING");
        try {
            ORJsonSource ImageSrc;
            ImageSrc.ParseFromFile(".\\Resources\\Anim.json");
            ORResourcePool<ORClipAnimType> ClipPool;
            if (!ImageSrc.LoadObject("ClipAnims", ClipPool))throw ORException("切片动画池载入失败！");
            WorkSpace.AnimPool.Merge(ClipPool);
        }
        catch (ORException& e)
        {
            MessageBoxW(MainWindowHandle, UTF8toUnicode(e.what()).c_str(), L"动画池载入错误", MB_OK);
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(u8"动画池载入错误");
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
