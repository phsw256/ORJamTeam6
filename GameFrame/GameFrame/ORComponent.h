#pragma once
#include "Include.h"
#include "ORImage.h"
#include "Minimal.h"
#include "ORTimer.h"

ImVec2 WindowRelPos(ImVec2 Rel);
ImVec2 FontRelPos(ImVec2 Rel);
template <typename T>
inline ImGuiID GetIDByObject(const T* This) { return reinterpret_cast<ImGuiID>(This); }
#define ThisImGuiID (GetIDByObject(this))

struct HintedName
{
    _UTF8 std::string Name, Desc, Hint;

};

template<typename T>
class ORResourcePool
{
private:
    std::unordered_map<std::string, std::shared_ptr<T>> Pool;
public:
    bool Insert(const std::string_view Name, bool Replace, T* pResource)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())
        {
            if (!Replace)return false;
            else Pool.erase(Name);
        }
        return Pool.insert({ Name, std::shared_ptr<T>(pResource) }).second;
    }
    template<typename... TArgs>
    bool Emplace(const std::string_view Name, bool Replace, TArgs&&... Args)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())
        {
            if (!Replace)return false;
            else Pool.erase(std::string(Name));
        }
        return Pool.insert({ std::string(Name), std::make_shared<T>(std::forward<TArgs>(Args)...)}).second;
    }
    std::shared_ptr<T> GetResource(const std::string_view Name)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())return it->second;
        else return nullptr;
    }
    std::shared_ptr<T> GetResource(const std::string_view Name, const std::shared_ptr<T>& Default)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())return it->second;
        else return Default;
    }
};

class ORComponent
{
public:
    std::shared_ptr<ORImage> BackGround;

    virtual ~ORComponent() {}
    virtual void DrawUI() = 0;//统一不带窗口创建

    ORComponent() = default;
    ORComponent(const ORComponent&) = delete;
    ORComponent(ORComponent&&) = delete;
};

class ORComponentFrame : public ORComponent
{
public:
    ImVec2 Position;
    ImVec2 Size;

    virtual ~ORComponentFrame() {}
    virtual void DrawUI() = 0;

    void Resize(ImVec2 Position, ImVec2 Size);
    inline void FitBackGroundSize() { if (BackGround)Size = BackGround->GetSize(); }
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
