/***********************************************************************************************
 ***                                 OR_JAM_6����Դ����                                      ***
 ***********************************************************************************************
 *                                                                                             *
 *                       �ļ��� : ORComponent.h                                                *
 *                                                                                             *
 *                     ��д���� : IronHammer_Std                                               *
 *                                                                                             *
 *                     �½����� : 2024/9/30                                                    *
 *                                                                                             *
 *                     ����༭ : 2024/10/5 IronHammer_Std                                     *
 *                                                                                             *
 *                     ��Ԫ���� : 2022/10/4 IronHammer_Std                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*/
/* ע�����ͱ��
 *     T������       TP��ģ���������   V��ֵ          TV��ģ�����ֵ
 *     F������       SF����̬����       VF���麯��     PVF�����麯��
 */
#pragma once
#include "Include.h"
#include "ORImage.h"
#include "Minimal.h"
#include "ORTimer.h"
#include "ORBackType.h"

 /***********************************************************************************************
  * WindowRelPos -- ����Դ��ڵĳߴ絽һ��ߴ�                   
  *                                                              
  * ִ����Ϊ��                                                   
  *    - ��һ������Դ��ڵĳߴ绻�㵽ʵ�ʵĳߴ硣                                        
  *                                                                                                           
  * ���������Rel -- ��Դ��ڵĳߴ�       ImVec2                                                                                            
  *                                                              
  *   ����ֵ��ʵ�ʵĳߴ�                  ImVec2                
  *                                                              
  * ע�����ʵ���Ǵ��ڳߴ����Գߴ�                                                 
  *                                                              
  *=============================================================================================*/
ImVec2 WindowRelPos(ImVec2 Rel);

/***********************************************************************************************
 * FontRelPos -- ���������ĳߴ絽һ��ߴ�
 *
 * ִ����Ϊ��
 *    - ��һ�����������ĳߴ绻�㵽ʵ�ʵĳߴ硣
 *
 * ���������Rel -- �������ĳߴ�       ImVec2
 *
 *   ����ֵ��ʵ�ʵĳߴ�                  ImVec2
 *
 * ע�����ʵ��������ߴ����Գߴ�
 *
 *=============================================================================================*/
ImVec2 FontRelPos(ImVec2 Rel);

/***********************************************************************************************
 * GetIDByObject -- ���յ�ַ���ɶ����ImGuiID
 *
 * ִ����Ϊ��
 *    - ��һ�����������ĳߴ绻�㵽ʵ�ʵĳߴ硣
 *
 * ���������This -- �������ĳߴ�       const T*
 *
 *   ����ֵ�������ImGuiID                ImGuiID
 *
 * ע�����������������������ȾUI�ģ�������ڲ����ַ�Ķ���
 *
 *=============================================================================================*/
template <typename T>
inline ImGuiID GetIDByObject(const T* This) { return reinterpret_cast<ImGuiID>(This); }

/***********************************************************************************************
 * ThisImGuiID -- ���յ�ַ���ɶ����ImGuiID
 *
 * ִ����Ϊ��
 *    - ���յ�ַ���ɵ�ǰ�����ImGuiID
 *
 * ע�������������ĳ�Ա�����ڲ�ʹ��
 *
 *=============================================================================================*/
#define ThisImGuiID (GetIDByObject(this))

 /***********************************************************************************************
  * InvisibleArrow -- ����һ���������ļ�ͷ��ť
  *
  * ִ����Ϊ��
  *    - ����һ���������ļ�ͷ��ť�����������룬���ߵ���������
  *
  * ���������Label -- ��ť��ǩ       const char*
  *
  * ע�������ǩ���ֻ��ռ��λ�ã���ǩ����Ϲ�������ܰ����뱣֤��ǩ��Ψһ�ԡ������ԡ�
  *
  *=============================================================================================*/
void InvisibleArrow(const char* Label);

/***********************************************************************************************
 *   ORDrawable             -- �ɻ��ƶ������     T        class
 *     ~ORDrawable          -- ��������           VF       DTOR
 *     DrawUI               -- ���ƺ���           PVF      void ()
 *     ORDrawable           -- ���캯��           F        CTOR_DEF
 *                                                F        CTOR_COPY
 *                                                F        CTOR_MOVE
 *
 * ���ݣ�
 *  -- �ǿɻ��ƶ�����Ĺ�������
 *
 * ע�⣺
 *  -- ���еĿɻ��ƶ��󶼱���ʵ��DrawUI�ӿڡ�
 *=============================================================================================*/
class ORDrawable
{
public:
    virtual ~ORDrawable() = default;
    virtual void DrawUI() = 0;//ͳһ�������ڴ���

    ORDrawable() = default;
    ORDrawable(const ORDrawable&) = default;
    ORDrawable(ORDrawable&&) = default;
};

/***********************************************************************************************
 *   �̳У�public ORDrawable
 *   ORComponent            -- OR�����������     T        class
 *     BackGround           -- ����ͼƬ           V        ORResPtr<ORImage>
 *     ~ORComponent         -- ��������           VF       DTOR
 *     ORComponent          -- ���캯��           F        CTOR_DEF
 *                                                F        CTOR_COPY (X)
 *                                                F        CTOR_MOVE (X)
 *
 * ���ݣ�
 *  -- ��OR��������Ĺ�������
 *
 * ע�⣺
 *  -- ���е�OR�������������ʵ�� DrawUI �ӿ�
 *  -- OR��������ǲ��ɸ��ơ������ƶ������
 *  -- ��������ʱӦ�÷���ָ�� ORComponent ������ָ�루�� ORResPtr��std::unique_ptr �ȣ�
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
 *   �̳У�public ORComponent
 *   ORComponentFrame       -- OR��λ���ƻ���     T        class
 *     Position             -- λ��               V        ImVec2
 *     Size                 -- �ߴ�               V        ImVec2
 *     ~ORComponentFrame    -- ��������           VF       DTOR
 *     ORComponentFrame     -- ���캯��           F        CTOR_DEF
 *                                                F        CTOR_COPY (X)
 *                                                F        CTOR_MOVE (X)
 *                          -- ָ��λ�á��ߴ繹�� F        CTOR (ImVec2 , ImVec2)
 *     Resize               -- ����λ�á��ߴ�     F        void (ImVec2, ImVec2)
 *     FitBackGroundSize    -- �������ߴ�����ߴ� F        void()
 *
 * ���ݣ�
 *  -- ��OR�ɶ�λ�Ļ�������Ĺ�������
 *
 * ע�⣺
 *  -- ע������ͬORComponent
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
    void SetHintCustom(const std::function<bool(_UTF8 std::string&)>& Fn);//����true������falseֹͣ��Clear
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
