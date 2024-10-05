#pragma once
#include "Include.h"
#include "ORImage.h"
#include "Minimal.h"
#include "ORTimer.h"
#include "ORBackType.h"

ImVec2 WindowRelPos(ImVec2 Rel);
ImVec2 FontRelPos(ImVec2 Rel);
template <typename T>
inline ImGuiID GetIDByObject(const T* This) { return reinterpret_cast<ImGuiID>(This); }
#define ThisImGuiID (GetIDByObject(this))
void InvisibleArrow(const char* Label);

class ORDrawable
{
public:
    virtual ~ORDrawable() = default;
    virtual void DrawUI() = 0;//统一不带窗口创建

    ORDrawable() = default;
    ORDrawable(const ORDrawable&) = default;
    ORDrawable(ORDrawable&&) = default;
};

class ORComponent : public ORDrawable
{
public:
    ORResPtr<ORImage> BackGround;

    virtual ~ORComponent() = default;
    ORComponent() = default;
    ORComponent(const ORComponent&) = delete;
    ORComponent(ORComponent&&) = delete;
};

class ORComponentFrame : public ORComponent
{
public:
    ImVec2 Position;
    ImVec2 Size;

    void Resize(ImVec2 Position, ImVec2 Size);
    inline void FitBackGroundSize() { if (BackGround)Size = BackGround->GetSize(); }
    virtual ~ORComponentFrame() = default;
    ORComponentFrame(ImVec2 Position, ImVec2 Size);
    ORComponentFrame() = default;
    ORComponentFrame(const ORComponentFrame&) = delete;
    ORComponentFrame(ORComponentFrame&&) = delete;
};

class ORDescManager : public ORComponentFrame
{
    _UTF8 std::string Desc;
public:
    ImGuiWindowFlags flags{ ImGuiWindowFlags_None };

    virtual ~ORDescManager() = default;
    virtual void DrawUI();

    ORDescManager() = default;
    ORDescManager(const ORDescManager&) = delete;
    ORDescManager(ORDescManager&&) = delete;
    ORDescManager(ImVec2 Position, ImVec2 Size);
    
    void Clear();
    void SetDesc(const _UTF8 std::string_view Desc);
};

class ORStage : public ORComponent
{
    _UTF8 std::string Name;
    bool Enabled;
public:
    virtual ~ORStage() = default;
    virtual void DrawUI() = 0;
    virtual bool DrawButton();
    virtual void EventLoop() = 0;
    virtual void OnSwitched() = 0;
    ORStage() = delete;
    ORStage(const ORStage&) = delete;
    ORStage(ORStage&&) = delete;
    ORStage(const _UTF8 std::string_view StageName);
    inline void Enable() { Enabled = true; }
    inline void Disable() { Enabled = false; }
    inline bool IsEnabled() const { return Enabled; }
    inline const _UTF8 std::string& GetName() const { return Name; }
};

class ORTopBar : public ORComponent
{
    std::vector<std::unique_ptr<ORStage>> Stages;
    int CurrentStage{ 0 };
public:
    ORResPtr<ORFilter<ORStage>> DrawUIExtFilter;
    bool ShowDisabledButtons{ true };

    virtual ~ORTopBar() = default;
    virtual void DrawUI();

    void AddStage(std::unique_ptr<ORStage>&& pStage);
    ORStage* GetCurrentStage() const;
    bool ForceChangeStage(int StageID);
    bool ForceChangeStage(const _UTF8 std::string_view Name);

    ORTopBar() = default;
    ORTopBar(const ORTopBar&) = delete;
    ORTopBar(ORTopBar&&) = delete;
};

class ORHintManager : public ORComponentFrame
{
private:
    std::vector<_UTF8 std::string> Hint{ "" };
    int HintChangeMillis = 5000;
    int UseHint = 0;
    TimerClass HintTimer;
    _UTF8 std::string CustomHint;
    bool HasCustom = false, HasSet = false, IsCountDown = false;
    std::function<bool(_UTF8 std::string&)> CustomFn;
    TimerClass CountDownTimer;
    int CountDownMillis;
public:
    bool FrameOnBottom{ false };
    float BorderRounding{ 0.0F };
    float BorderThickness{ 1.0F };

    virtual ~ORHintManager() = default;
    virtual void DrawUI();
    void Clear();
    void SetHint(const _UTF8 std::string& Str, int TimeLimitMillis = -1);
    void SetHintCustom(const std::function<bool(_UTF8 std::string&)>& Fn);//返回true继续，false停止并Clear
    const std::string& GetHint() const;
    bool LoadHintFile(ORStaticStraw& Source);
    bool LoadHintFile(const std::string_view FileName);
};

typedef std::function<void()> StdMessage;

struct ORPopUp : public ORComponentFrame
{

    bool CanClose{ false };//Only when Modal==true
    bool Modal{ false };
    _UTF8 std::string Title{ "" };
    ImGuiWindowFlags Flag{ ImGuiWindowFlags_None };
    StdMessage Show{ []() {} };
    StdMessage Close{ []() {} };//run when it can be closed and it is closed

    virtual ~ORPopUp();
    virtual void DrawUI();
    ORPopUp& operator=(const ORPopUp&) = default;

    ORPopUp& Create(const _UTF8 std::string& title);
    ORPopUp& CreateModal(const _UTF8 std::string& title, bool canclose, StdMessage close = []() {});
    ORPopUp& SetTitle(const _UTF8 std::string& title);
    ORPopUp& SetFlag(ImGuiWindowFlags flag);
    ORPopUp& UnsetFlag(ImGuiWindowFlags flag);
    ORPopUp& ClearFlag();
    ORPopUp& SetSize(ImVec2 NewSize = { 0,0 });//{0,0}=default/auto
    ORPopUp& PushTextPrev(const _UTF8 std::string& Text);
    ORPopUp& PushTextBack(const _UTF8 std::string& Text);
    ORPopUp& PushMsgPrev(StdMessage Msg);
    ORPopUp& PushMsgBack(StdMessage Msg);

    ORPopUp() = default;
    ORPopUp(const ORPopUp&) = delete;
    ORPopUp(ORPopUp&&) = delete;
};

class ORPopUpManager : public ORDrawable
{
private:
    std::unique_ptr<ORPopUp> CurrentPopup;
    bool HasPopup;
public:
    static std::unique_ptr<ORPopUp> SingleText(const _UTF8 std::string& StrId, const _UTF8 std::string& Text, bool Modal);
    static std::unique_ptr<ORPopUp> BasicPopUp();

    inline void SetCurrentPopup(std::unique_ptr<ORPopUp>&& sc) { HasPopup = true; CurrentPopup = std::move(sc); }
    inline void ClearCurrentPopup() { HasPopup = false; }

    virtual ~ORPopUpManager() = default;
    virtual void DrawUI();
};

template<typename Cont>
class ORListMenu : public ORComponent
{
    std::vector<Cont>& List;
    int Page{ 0 };
    int KeyPerPage;
    DWORD Reserved;
public:
    typedef Cont Type;
    typedef std::function<void(Cont&, int, int, DWORD)> ActionType;
    std::string Tag;
    ActionType Action;

    ORListMenu() = delete;
    ORListMenu(std::vector<Cont>& L, const std::string& t, const ActionType& a, DWORD _Reserved = 0, int _KeyPerPage = 10) :
        List(L), Page(0), Tag(t), Action(a), KeyPerPage(_KeyPerPage), Reserved(_Reserved) {}
    inline int& PageLength() { return KeyPerPage; }
    inline int PageLength() const { return KeyPerPage; }


    virtual ~ORListMenu() = default;
    virtual void DrawUI();
};

void Browse_ShowList_Impl(const std::string& suffix, int Sz, int* Page, int KeyPerPage);
template<typename Cont>
void Browse_ShowList(const std::string& suffix, std::vector<Cont>& Ser, int* Page, const std::function<void(Cont&, int, int, DWORD)>& Callback, int KeyPerPage, DWORD Reserved)
{
    int RenderF = (*Page) * KeyPerPage;
    int RenderN = (1 + (*Page)) * KeyPerPage;
    int RealRF = std::max(RenderF, 0);
    int RealNF = std::min(RenderN, (int)Ser.size());
    for (int On = RealRF; On < RealNF; On++)
    {
        Callback(Ser.at(On), On - RealRF + 1, On, Reserved);
    }
    Browse_ShowList_Impl(suffix, Ser.size(), Page, KeyPerPage);
}

template<typename Cont>
void ORListMenu<Cont>::DrawUI()
{
    Browse_ShowList(Tag, List, &Page, Action, KeyPerPage, Reserved);
}

class ORUndoStack
{
public:
    struct _Item
    {
        std::string Id;
        std::function<void()> UndoAction, RedoAction;
        std::function<std::any()> Extra;
    };
private:
    std::vector<_Item> Stack;
    int Cursor{ -1 };
public:
    bool Undo();
    bool Redo();
    bool CanUndo() const;
    bool CanRedo() const;
    void Release();
    void Push(const _Item& a);
    void Clear();
    _Item* Top();
};

class ORWorkSpace
{
public:
    ORTopBar TopBar;
    ORHintManager BottomBar;
    ORPopUpManager PopUpManager;
    ORUndoStack UndoStack;

    ORWorkSpace() = default;
    ORWorkSpace(const ORWorkSpace&) = delete;
    ORWorkSpace(ORWorkSpace&&) = delete;
};
