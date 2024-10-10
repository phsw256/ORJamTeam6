/***********************************************************************************************
 ***                                 OR_JAM_6——源代码                                      ***
 ***********************************************************************************************
 *                                                                                             *
 *                       文件名 : ORComponent.h                                                *
 *                                                                                             *
 *                     编写名单 : IronHammer_Std                                               *
 *                                                                                             *
 *                     新建日期 : 2024/9/30                                                    *
 *                                                                                             *
 *                     最近编辑 : 2024/10/5 IronHammer_Std                                     *
 *                                                                                             *
 *                     单元测试 : 2022/10/4 IronHammer_Std                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*/
/* 注：类型标记
 *     T：类型       TP：模板参数类型   V：值          TV：模板参数值
 *     F：函数       SF：静态函数       VF：虚函数     PVF：纯虚函数
 */
#pragma once
#include "Include.h"
#include "ORImage.h"
#include "Minimal.h"
#include "ORTimer.h"
#include "ORBackType.h"

 /***********************************************************************************************
  * WindowRelPos -- 从相对窗口的尺寸到一般尺寸                   
  *                                                              
  * 执行行为：                                                   
  *    - 将一个从相对窗口的尺寸换算到实际的尺寸。                                        
  *                                                                                                           
  * 输入参数：Rel -- 相对窗口的尺寸       ImVec2                                                                                            
  *                                                              
  *   返回值：实际的尺寸                  ImVec2                
  *                                                              
  * 注意事项：实际是窗口尺寸×相对尺寸                                                 
  *                                                              
  *=============================================================================================*/
ImVec2 WindowRelPos(ImVec2 Rel);

/***********************************************************************************************
 * FontRelPos -- 从相对字体的尺寸到一般尺寸
 *
 * 执行行为：
 *    - 将一个从相对字体的尺寸换算到实际的尺寸。
 *
 * 输入参数：Rel -- 相对字体的尺寸       ImVec2
 *
 *   返回值：实际的尺寸                  ImVec2
 *
 * 注意事项：实际是字体尺寸×相对尺寸
 *
 *=============================================================================================*/
ImVec2 FontRelPos(ImVec2 Rel);

/***********************************************************************************************
 * GetIDByObject -- 按照地址生成对象的ImGuiID
 *
 * 执行行为：
 *    - 将一个从相对字体的尺寸换算到实际的尺寸。
 *
 * 输入参数：This -- 相对字体的尺寸       const T*
 *
 *   返回值：对象的ImGuiID                ImGuiID
 *
 * 注意事项：这个东西是用来配合渲染UI的，最好用于不变地址的对象
 *
 *=============================================================================================*/
template <typename T>
inline ImGuiID GetIDByObject(const T* This) { return reinterpret_cast<ImGuiID>(This); }

/***********************************************************************************************
 * ThisImGuiID -- 按照地址生成对象的ImGuiID
 *
 * 执行行为：
 *    - 按照地址生成当前对象的ImGuiID
 *
 * 注意事项：仅限于类的成员函数内部使用
 *
 *=============================================================================================*/
#define ThisImGuiID (GetIDByObject(this))

 /***********************************************************************************************
  * InvisibleArrow -- 绘制一个看不见的箭头按钮
  *
  * 执行行为：
  *    - 绘制一个看不见的箭头按钮，用来填充对齐，或者单纯拿来按
  *
  * 输入参数：Label -- 按钮标签       const char*
  *
  * 注意事项：标签如果只是占个位置，标签可以瞎填。如果想能按，请保证标签的唯一性、不变性。
  *
  *=============================================================================================*/
void InvisibleArrow(const char* Label);

/***********************************************************************************************
 *   ORDrawable             -- 可绘制对象基类     T        class
 *     ~ORDrawable          -- 析构函数           VF       DTOR
 *     DrawUI               -- 绘制函数           PVF      void ()
 *     ORDrawable           -- 构造函数           F        CTOR_DEF
 *                                                F        CTOR_COPY
 *                                                F        CTOR_MOVE
 *
 * 内容：
 *  -- 是可绘制对象类的公共基类
 *
 * 注意：
 *  -- 所有的可绘制对象都必须实现DrawUI接口。
 *=============================================================================================*/
class ORDrawable
{
public:
    virtual ~ORDrawable() = default;
    virtual void DrawUI() = 0;//统一不带窗口创建

    ORDrawable() = default;
    ORDrawable(const ORDrawable&) = default;
    ORDrawable(ORDrawable&&) = default;
};

/***********************************************************************************************
 *   继承：public ORDrawable
 *   ORComponent            -- OR绘制组件基类     T        class
 *     BackGround           -- 背景图片           V        ORResPtr<ORImage>
 *     ~ORComponent         -- 析构函数           VF       DTOR
 *     ORComponent          -- 构造函数           F        CTOR_DEF
 *                                                F        CTOR_COPY (X)
 *                                                F        CTOR_MOVE (X)
 *
 * 内容：
 *  -- 是OR绘制组件的公共基类
 *
 * 注意：
 *  -- 所有的OR绘制组件都必须实现 DrawUI 接口
 *  -- OR绘制组件是不可复制、不可移动构造的
 *  -- 放入容器时应该放入指向 ORComponent 的智能指针（如 ORResPtr、std::unique_ptr 等）
 *============================================================================================*/
class ORComponent : public ORDrawable
{
public:
    ORResPtr<ORImage> BackGround;

    virtual ~ORComponent() = default;
    ORComponent() = default;
    ORComponent(const ORComponent&) = delete;
    ORComponent(ORComponent&&) = delete;
};

/***********************************************************************************************
 *   继承：public ORComponent
 *   ORComponentFrame       -- OR定位绘制基类     T        class
 *     Position             -- 位置               V        ImVec2
 *     Size                 -- 尺寸               V        ImVec2
 *     ~ORComponentFrame    -- 析构函数           VF       DTOR
 *     ORComponentFrame     -- 构造函数           F        CTOR_DEF
 *                                                F        CTOR_COPY (X)
 *                                                F        CTOR_MOVE (X)
 *                          -- 指定位置、尺寸构造 F        CTOR (ImVec2 , ImVec2)
 *     Resize               -- 重设位置、尺寸     F        void (ImVec2, ImVec2)
 *     FitBackGroundSize    -- 按背景尺寸重设尺寸 F        void()
 *
 * 内容：
 *  -- 是OR可定位的绘制组件的公共基类
 *
 * 注意：
 *  -- 注意事项同ORComponent
 *============================================================================================*/
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
    ORStage* GetStage(const _UTF8 std::string_view Name) const;
    template<typename T>
    inline T* GetStage_Typed(const _UTF8 std::string_view Name) const
    {
       return dynamic_cast<T*>(GetStage(Name));
    }
    bool ForceChangeStage(int StageID);
    bool ForceChangeStage(const _UTF8 std::string_view Name);
    void EventLoop(bool IgnoreDisabledStages = true);

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
    ORResourcePool<ORImage> ImagePool;
    ORResPtr<ORImage> MissingImage;

    ORWorkSpace() = default;
    ORWorkSpace(const ORWorkSpace&) = delete;
    ORWorkSpace(ORWorkSpace&&) = delete;
};
