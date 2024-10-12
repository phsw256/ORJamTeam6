#pragma once
#include "ORComponent.h"
#include "ORBackType.h"
#include "ORGameMap.h"
#include "RoundData.h"

class ORStage_MusicPlayer;
class Stage_MainMenu;
class Stage_ShellSelect;
class Stage_Setting;
class Stage_InGameOptions;
class Stage_TechTree;
class Stage_MainGame;

const int InitialVolume = 800;


class ORStage_MusicPlayer : public ORStage
{
private:
    static void DrawMenuElem(std::string&, int RelIdx, int AbsIdx, DWORD pList);
    ORAsyncPlayList PlayList;
    ORListMenu<std::string> Menu;
    int VolumeSlider{ InitialVolume };
    int TimeSlider{ 0 };
    std::vector<DWORD>ResultBuffer;
    RateClass RefreshRate;
    ORAnim Blink{ NoInit{} };
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
    inline ORAsyncPlayList& GetList() { return PlayList; }
};

class Stage_MainMenu : public ORStage
{
public:
    virtual ~Stage_MainMenu() = default;
    virtual void DrawUI();
    virtual void EventLoop();
    virtual void OnSwitched();
    Stage_MainMenu(const _UTF8 std::string_view StageName);
};


struct ShellSetting
{
    bool MMPMode{ false };
};

struct 教程关Rules {};

struct RulesClass
{
    ShellSetting Setting;
    bool ITeachYouHowToPlayThisFuckingGame;
    RulesClass(const ShellSetting& setting):Setting(setting)
    {
        ITeachYouHowToPlayThisFuckingGame = false;
    }
    RulesClass(教程关Rules)
    {
        Setting.MMPMode = false;
        ITeachYouHowToPlayThisFuckingGame = true;
    }
};

class Stage_ShellSelect : public ORStage
{
    ShellSetting Setting;
public:
    virtual ~Stage_ShellSelect() = default;
    virtual void DrawUI();
    virtual void EventLoop();
    virtual void OnSwitched();
    Stage_ShellSelect(const _UTF8 std::string_view StageName);
};

class Stage_Setting : public ORStage
{
    bool ToMainMenu{ true };
public:
    virtual ~Stage_Setting() = default;
    virtual void DrawUI();
    virtual void EventLoop();
    virtual void OnSwitched();
    Stage_Setting(const _UTF8 std::string_view StageName);
    void SwitchInGame();
    void SwitchInMenu();
};

class Stage_InGameOptions : public ORStage
{
    Stage_MainGame* Main{ nullptr };
public:
    virtual ~Stage_InGameOptions() = default;
    virtual void DrawUI();
    virtual void EventLoop();
    virtual void OnSwitched();
    Stage_InGameOptions(const _UTF8 std::string_view StageName);
};

class Stage_TechTree : public ORStage
{
    struct Opt
    {
        TechTree* pTree;
        std::string Name;
        ImColor BgCol;
    };
    Stage_MainGame* Main{ nullptr };
    TechTree BodyTechTree, CulTechTree, SciTechTree;
    Opt Opts[3] = { {&BodyTechTree, u8"体质"}, {&CulTechTree, u8"文化"}, {&SciTechTree, u8"科技"} };
    int CurOpt{ 0 };
public:
    void Init();

    virtual ~Stage_TechTree() = default;
    virtual void DrawUI();
    virtual void EventLoop();
    virtual void OnSwitched();
    Stage_TechTree(const _UTF8 std::string_view StageName);
};

class Stage_MainGame : public ORStage
{
    std::unique_ptr<RulesClass> Rules;
    ORIsoTileMap TileMap;
    Stage_TechTree* Tree{ nullptr };

    void InitTileMap();
    void InitTechTree();
public:
    virtual ~Stage_MainGame() = default;
    virtual void DrawUI();
    virtual void EventLoop();
    virtual void OnSwitched();
    inline void InitRules(const ShellSetting& Setting)
    {
        Rules.reset(new RulesClass(Setting));
        InitTileMap(); InitTechTree();
    }
    inline void InitRules(教程关Rules _)
    {
        Rules.reset(new RulesClass(_));
        InitTileMap(); InitTechTree();
    }
    Stage_MainGame(const _UTF8 std::string_view StageName);
    void Pause();
    void Resume();
    void ExitGame();
};

class PosSetHelper
{
    ImVec2 Pos,Tar;
public:
    PosSetHelper(ImVec2 pos) :Tar(pos)
    {
        Pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(pos);
    }
    void SetX() { ImGui::SetCursorPosX(Tar.x); }
    ~PosSetHelper()
    {
        ImGui::SetCursorPos(Pos);
    }
};

namespace ORTest
{
    void Init();
    void Loop();
    void CleanUp();
}
