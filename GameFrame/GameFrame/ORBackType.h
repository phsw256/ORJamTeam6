/***********************************************************************************************
 ***                                 OR_JAM_6����Դ����                                      ***
 ***********************************************************************************************
 *                                                                                             *
 *                       �ļ��� : ORBackType.h                                                 *
 *                                                                                             *
 *                     ��д���� : IronHammer_Std                                               *
 *                                                                                             *
 *                     �½����� : 2024/10/1                                                    *
 *                                                                                             *
 *                     ����༭ : 2024/10/1 IronHammer_Std                                     *
 *                                                                                             *
 *                     ��Ԫ���� : 2022/10/5 IronHammer_Std                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*/
/* ע�����ͱ��
 *     T������       TP��ģ���������   V��ֵ          TV��ģ�����ֵ
 *     F������       SF����̬����       VF���麯��     PVF�����麯��
 */
#pragma once
#include "ExtJson.h"
#include "ORFile.h"
#include "Include.h"
#include "MusicMCI.h"

 /***********************************************************************************************
  *   HintedName -- ID+���������    T    struct
  *     Name     -- һ��ID����       V    _UTF8 std::string           
  *     Desc     -- ��������         V    _UTF8 std::string
  *     Hint     -- ������ʾ         V    _UTF8 std::string
  *                                                                                             
  * ���ݣ�                                                                                      
  *  -- �����˴�����ʾ��������ID��һ������                                                      
  *                                                                                             
  * ע�⣺                                                                                      
  *  -- ��                     
  *=============================================================================================*/
struct HintedName
{
    _UTF8 std::string Name, Desc, Hint;

};

/***********************************************************************************************
 *   <T>                    -- T �Ǵ�ɸѡ������   TP
 *   ORFilter               -- ɸѡ������         T        class
 *     ~ORFilter            -- ��������           VF       DTOR
 *     IsSelected           -- �Ƿ�ɸѡ��       PVF      bool (const T&)
 *     operator()           -- =IsSelected        F        bool (const T&)
 *
 * ���ݣ�
 *  -- ��ɸѡ����Ĺ�������
 *  -- ��������ɸѡ����ORTopBar�����л������˵���
 *
 * ע�⣺
 *  -- ����ʵ����operator()��ɸѡ�������ʹ��std::function����
 *=============================================================================================*/
template<typename T>
class ORFilter
{
public:
    virtual ~ORFilter() {}
    virtual bool IsSelected(const T& Obj) = 0;
    inline bool operator()(const T& Obj)
    {
        return IsSelected(Obj);
    }
};

/***********************************************************************************************
 *   <T>                    -- T �Ǵ�ɸѡ������    TP
 *   ORSetFilter            -- ����ʽɸѡ��        T       class
 *     Range                -- ��ѡ���ķ�Χ        V       std::set<T>
 *     ~ORSetFilter         -- ��������            VF      DTOR
 *     IsSelected           -- �Ƿ�ɸѡ��        VF      bool (const T&)
 *
 * ���ݣ�
 *  -- ��ɸѡRange���������Ԫ�ص�ɸѡ��
 *  -- ��������ɸѡ����ORTopBar�����л������˵���
 *
 * ע�⣺
 *  -- ����ʵ����operator()��ɸѡ�������ʹ�� std::function ����
 *=============================================================================================*/
template<typename T>
class ORSetFilter :public ORFilter<T>
{
public:
    std::set<T> Range;

    virtual ~ORSetFilter() {}
    virtual bool IsSelected(const T& Obj)
    {
        return Range.count(Obj) != 0;
    }
};

/***********************************************************************************************
 *   <T,Pred>            -- T �Ǵ�ɸѡ������
 *                          Pred �ǱȽ�ν��     TP
 *   ORCompareFilter     -- �Ƚ�ʽɸѡ��        T     class
 *   ˽�г�Ա��
 *     value             -- ɸѡ����ֵ          V     T
 *   ������Ա��
 *     ~ORCompareFilter  -- ��������            VF      DTOR
 *     IsSelected        -- �Ƿ�ɸѡ��        VF     bool (const T&)
 *
 * ���ݣ�
 *  -- �ǰ�������ֵ�ıȽ���ɸѡ��ɸѡ��
 *  -- ��������ɸѡ����ORTopBar�����л������˵���
 *
 * ע�⣺
 *  -- ����ʵ����operator()��ɸѡ�������ʹ�� std::function ����
 *  -- Pred�����˱ȽϷ�ʽ����ȡ std::less<T> ʱ��ʾɸѡС����ֵ����
 *=============================================================================================*/
template<typename T,typename Pred>
class ORCompareFilter :public ORFilter<T>
{
private:
    T value;
public:
    virtual ~ORCompareFilter() {}
    virtual bool IsSelected(const T& Obj)
    {
        return (Pred{})(Obj, value);
    }

    ORCompareFilter() : T() {}
    ORCompareFilter(const ORCompareFilter&) = default;
    ORCompareFilter(ORCompareFilter&&) = default;
    ORCompareFilter(const T& v) : value(v) {}
    ORCompareFilter(T&& v) : value(std::move(v)) {}
};


/***********************************************************************************************
 *   <T>                 -- T ����Դ�ش洢������     TP
 *   ORResourcePool      -- ��������Դ��             T    class
 *   ˽�г�Ա��
 *     Pool              -- ��Դ�ص� Map ����        V    std::unordered_map<std::string, ORResPtr<T>>
 *   ������Ա��
 *     Insert            -- ������Դ                 F    bool (const std::string_view, bool, T*)
 *     Emplace           -- ������Դ                 F    bool (const std::string_view, bool, TArgs&&...)
 *     EmplaceAndReturn  -- ������Դ�����ؾ��       F    std::pair<typename std::unordered_map<std::string, ORResPtr<T>>::iterator,bool> (const std::string_view, bool, TArgs&&...)
 *     ItEnd             -- ��EmplaceAndReturn���   F    typename std::unordered_map<std::string, ORResPtr<T>>::iterator ()
 *     GetResource       -- ��ȡ��Դ                 F    ORResPtr<T> (const std::string_view)
 *                       -- ��ȡ����Դ�򷵻�Ĭ��ֵ   F    ORResPtr<T> (const std::string_view, const ORResPtr<T>&)
 *     DeleteResource    -- �ͷ�ָ����Դ             F    bool (const std::string_view)
 *     Clear             -- �ͷ�ȫ����Դ             F    void ()
 * 
 * ���ݣ�
 *  -- ��Ϊ��Դ�ض���������ǰ���ز���������
 *
 * ע�⣺
 *  -- �洢���� ORResPtr �����������ü���Ϊ0ʱ���Զ��ͷ�
 *  -- ��� DeleteResource(��Clear) �ͷŵ���Դ���ڱ�ʹ�ã���ô�����ӳٵ���Դʹ�ý��������˳��к��ͷ�
 *  -- ORResourcePool �����̰߳�ȫ��
 *=============================================================================================*/
template<typename T>
class ORResourcePool
{
private:
    std::unordered_map<std::string, ORResPtr<T>> Pool;
public:
    bool Insert(const std::string_view Name, bool Replace, T* pResource)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())
        {
            if (!Replace)return false;
            else Pool.erase(Name);
        }
        return Pool.insert({ Name, ORResPtr<T>(pResource) }).second;
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
        return Pool.insert({ std::string(Name), std::make_shared<T>(std::forward<TArgs>(Args)...) }).second;
    }
    template<typename... TArgs>
    std::pair<typename std::unordered_map<std::string, ORResPtr<T>>::iterator,bool> EmplaceAndReturn(const std::string_view Name, bool Replace, TArgs&&... Args)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())
        {
            if (!Replace)return { Pool.end(),false };
            else Pool.erase(std::string(Name));
        }
        return Pool.insert({ std::string(Name), std::make_shared<T>(std::forward<TArgs>(Args)...) });
    }
    typename std::unordered_map<std::string, ORResPtr<T>>::iterator ItEnd() { return Pool.end(); }
    ORResPtr<T> GetResource(const std::string_view Name)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())return it->second;
        else return nullptr;
    }
    ORResPtr<T> GetResource(const std::string_view Name, const ORResPtr<T>& Default)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())return it->second;
        else return Default;
    }
    bool DeleteResource(const std::string_view Name)
    {
        auto it = Pool.find(std::string(Name));
        if (it != Pool.end())
        {
            Pool.erase(std::string(Name)); return true;
        }
        else return false;
    }
    void Clear()
    {
        Pool.clear();
    }
};

/***********************************************************************************************
 *   <T>                 -- T ��ջ�洢������       TP
 *   ORInfoStack         -- �첽��Ϣջ             T     class
 *     Cont              -- ��������               T     std::vector<T>
 *   ˽�г�Ա��
 *     Store             -- ��Դ�ص� Map ����      V     Cont
 *     Lock              -- ������                 V     std::mutex
 *   ������Ա�� 
 *     Empty             -- �Ƿ�Ϊ��               F     bool ()
 *     Size              -- ������С               F     size_t ()
 *     GetStoreRaw       -- ��ȡԭʼ����           F     Cont& ()
 *     GetCopy           -- ��ȡ���ݵĸ���         F     Cont ()
 *                       -- ��Ŀ�긴������         F     void (Cont&)
 *     SetCont           -- ��������ֵ             F     void (const Cont&)
 *                       -- �ӵ������Ը�ֵ         F     void (std::vector<T>::iterator, std::vector<T>::iterator)
 *     Clear             -- �������               F     void ()
 *     GetCopyAndClear   -- �������Ƴ���������     F     Cont ()
 *                       -- �������Ƶ�Ŀ�괦��     F     void (Cont&)
 *     Push              -- ��������               F     void (const T& Ct)
 * 
 * ���ݣ�
 *  -- �첽��Ϣջ�����ڼ򵥵��̼߳�ͨѶ������
 *
 * ע�⣺
 *  -- ����Ҫ�̰߳�ȫ�� vector ʱ�����������
 *  -- GetStoreRaw �����ƻ��̰߳�ȫ���õ�ԭʼ������ӦС�Ĵ���
 *=============================================================================================*/
template<class T>
class ORInfoStack
{
public:
    typedef std::vector<T> Cont;
private:
    Cont Store;
    std::mutex Lock;
public:
    bool Empty()
    {
        Lock.lock();
        bool Ret = Store.empty();
        Lock.unlock();
        return Ret;
    }
    size_t Size()
    {
        Lock.lock();
        size_t Ret = Store.size();
        Lock.unlock();
        return Ret;
    }
    Cont& GetStoreRaw()
    {
        return Store;
    }
    Cont GetCopy()
    {
        Lock.lock();
        Cont Ret = Store;
        Lock.unlock();
        return Ret;
    }
    void GetCopy(Cont& Target)
    {
        Lock.lock();
        Target = Store;
        Lock.unlock();
    }
    void SetCont(const Cont& Source)
    {
        Lock.lock();
        Store = Source;
        Lock.unlock();
    }
    void SetCont(std::vector<T>::iterator Begin, std::vector<T>::iterator End)
    {
        Lock.lock();
        Store.assign(Begin, End);
        Lock.unlock();
    }
    void Clear()
    {
        Lock.lock();
        Store.clear();
        Lock.unlock();
    }
    Cont GetCopyAndClear()
    {
        Lock.lock();
        Cont Ret;
        Ret.swap(Store);
        Lock.unlock();
        return Ret;
    }
    void GetCopyAndClear(Cont& Target)
    {
        Lock.lock();
        Target.swap(Store);
        Store.clear();
        Lock.unlock();
    }
    void Push(const T& Ct)
    {
        Lock.lock();
        Store.push_back(Ct);
        Lock.unlock();
    }
};

/***********************************************************************************************
 *   ORVariableList      -- �ɼ̳еĶ�̬����       T     class
 *     UpValue           -- ��ʽ�̳е�ֵ           V     std::shared_ptr<ORVariableList>
 *     Value             -- ����������ֵ           V     std::unordered_map<std::string, std::string>
 *     GetPtr            -- ��ȡָ���Լ���ָ��     F     std::shared_ptr<ORVariableList> ()
 *     Load              -- ��Json��ȡ���Ա�       F     bool (JsonObject)
 *     FillKeys          -- �������ֵ             F     void (const std::vector<std::string>&, const std::string&)
 *     Merge             -- �ϲ��������е�ֵ       F     void (const ORVariableList&, bool)
 *     GetVariable       -- ��ȡ����ֵ             F     const std::string& (const std::string&) const
 *     CoverUpValue      -- ֵ�Ƿ�����ڱ��̳еı� F     void (Cont&)
 *     HasValue          -- ֵ�Ƿ����             F     bool (const std::string& Name) const
 *     GetText           -- ��ȡ�ı���ʽ           F     std::string (bool) const
 *     Flatten           -- ����Ӹ���ʼ��ȫ����Ϣ F     void (ORVariableList&) const
 *     Read              -- ���ļ���ȡ���Ա�       F     void (ORReadStraw&)
 * 
 * ���ݣ�
 *  -- �ɼ̳еĶ�̬���Ա���������ʱ�޸�/����
 *
 * ע�⣺
 *  -- һ�����޸�ֵ���������������ı��������Ӧֵ������Ӱ��
 *=============================================================================================*/
struct ORVariableList : std::enable_shared_from_this<ORVariableList>
{
    std::shared_ptr<ORVariableList> UpValue{ nullptr };
    std::unordered_map<std::string, std::string>Value;

    inline std::shared_ptr<ORVariableList> GetPtr() { return shared_from_this(); }
    bool Load(JsonObject FromJson);
    void FillKeys(const std::vector<std::string>& List, const std::string& Val);
    void Merge(const ORVariableList& Another, bool MergeUpValue);

    const std::string& GetVariable(const std::string& Name) const;
    bool CoverUpValue(const std::string& Name) const;
    bool HasValue(const std::string& Name) const;
    std::string GetText(bool ConsiderUpValue) const;
    void Flatten(ORVariableList& Target) const;

    void Read(ORReadStraw& File);//����UpValue
    //void Write(const ExtFileClass& File)const;//����UpValue
};


/***********************************************************************************************
 *   ORPlayListImplParam       -- �ڲ���                 T     struct
 *   ORPlayListImpl            -- �ڲ���                 T     class
 *   ORAsyncPlayList           -- ���ֲ��ſ�����         T     class
 *     SwitchMode              -- �и�ģʽ               T     enum class : int8_t
 *     Internal                -- �ڲ���                 T     struct
 *       List                  -- ���BGM����Դ��        V     ORResourcePool<ListedMusic>
 *       Sequence              -- �赥˳��               V     std::vector<std::string>
 *       Playing               -- �Ƿ����ڲ���           V     std::atomic<bool>
 *       Mode                  -- ��ǰ�и�ģʽ           V     SwitchMode
 *       Post                  -- �첽��������ջ         V     ORInfoStack<std::pair<void(*)(ORPlayListImpl*), ORPlayListImplParam>> 
 *       Result                -- ����������ջ         V     ORInfoStack<DWORD> 
 *       CurVolume             -- ��ǰ����               V     DWORD 
 *   ˽�г�Ա��
 *     Data                    -- �ڲ�����               V     Internal
 *     Player                  -- �������߳�             V     std::unique_ptr<std::thread> 
 *   ������Ա��
 *     AddToList               -- ��BGM����赥          F     bool (const std::string_view, LPCWSTR, bool)
 *     ClearList               -- ��ո赥               F     bool ()
 *     StartPlay               -- ��ָ����Ŀ�𲥷Ÿ赥   F     void (const std::string_view)
 *                             -- ��ͷ���Ÿ赥           F     void ()
 *     EndPlay                 -- ֹͣ���Ÿ赥           F     void ()
 *     IsPlaying               -- �Ƿ��ڲ��Ÿ赥         F     bool ()
 *     PlayNext                -- ��һ��                 F     void (bool, bool)
 *     PlayPrev                -- ��һ��                 F     void (bool, bool)
 *     SetSwitchMode           -- �л��и�ģʽ           F     void (SwitchMode, bool)
 *     SetPlayFromFirst        -- ��ͷ����               F     void (bool)
 *     SwitchTo                -- ���������л�BGM        F     void (const std::string_view, bool)
 *     SwitchTo                -- ���������л�BGM        F     void (int, bool)
 *     PlayCurrent             -- ���ŵ�ǰBGM            F     void (bool)
 *     StopCurrent             -- ֹͣ��ǰBGM            F     void (bool)
 *     PauseCurrent            -- ��ͣ��ǰBGM            F     void (bool)
 *     ResumeCurrent           -- �ָ���ǰBGM            F     void (bool)
 *     SetVolume               -- ���ò�������           F     void (DWORD Volume, bool)
 *     SetCurrentStartTime     -- ���ò��Ž���           F     void (DWORD , bool)
 *     SyncTotalTime           -- ͬ����ȡBGMʱ��        F     DWORD ()
 *     SyncCurrentTime         -- ͬ����ȡ���Ž���       F     DWORD ()
 *     SyncCurrentMusicIdx     -- ͬ����ȡ��ǰBGM����    F     DWORD ()
 *     AsyncTotalTime          -- �첽��ȡBGMʱ��        F     void ()
 *     AsyncCurrentTime        -- �첽��ȡ���Ž���       F     void ()
 *     AsyncCurrentMusicIdx    -- �첽��ȡ��ǰBGM����    F     void ()
 *     WaitForResult           -- ��ȡ�첽�����ĵ������ F     DWORD ()
 *     WaitForResult           -- ��ȡ�첽�����Ķ����� F     std::vector<DWORD> (size_t)
 *     ReceivedCount           -- ��ȡ�ɽ��յĽ������   F     size_t ()
 *     GetSwitchModeStr        -- ���и�ģʽ��ȡ����     SF    const _UTF8 char* (SwitchMode Mode)
 *     GetMusicName            -- ��������ȡBGM����      F     std::string_view (size_t Idx)
 *     SyncCurrentMusicName    -- ͬ����ȡ��ǰBGM����    F     std::string_view ()
 *     GetSequence             -- ��ȡ��ǰ�赥           F     std::vector<std::string>& ()
 *     GetCurrentSwitchMode    -- ��ǰ�и�ģʽ           F     SwitchMode ()
 *     GetCurrentSwitchModeStr -- ��ǰ�и�ģʽ����       F     const _UTF8 char* ()
 *
 * ���ݣ�
 *  -- ���ֲ��ſ����������ڿ���һ���첽��BGM������
 *
 * ע�⣺
 *  -- ����������첽���Ƶģ���ר�ŵĲ������߳���������
 *  -- �������������BGM��
 *  -- AddToList �� ClearList ���ڲ�����û���ڲ���ʱ����
 *  -- ���Ƽ��޸�ͨ��GetSequence��ȡ�ĸ赥
 *  -- SyncXX �� WaitForResult ��ͬ����ȡ����Ϣ�����Բ�ҪƵ����ȡ�������� RateClass/StrictRateClass �����ƻ�ȡƵ��
 *=============================================================================================*/
struct ORPlayListImplParam
{
    DWORD Param;
    bool IgnoreResult;
};
class ORPlayListImpl;

class ORAsyncPlayList
{
public:
    enum class SwitchMode : int8_t
    {
        Random,
        Ordered,
        Loop
    };
    struct Internal
    {
        ORResourcePool<ListedMusic> List;
        std::vector<std::string> Sequence;
        std::atomic<bool> Playing;
        SwitchMode Mode{ SwitchMode::Random };
        ORInfoStack<std::pair<void(*)(ORPlayListImpl*), ORPlayListImplParam>> Post;
        ORInfoStack<DWORD> Result;
        DWORD CurVolume{ 0 };
    };
private:
    Internal Data;
    std::unique_ptr<std::thread> Player;
public:
    //����δ�ڲ���ʱ������������
    bool AddToList(const std::string_view ID, LPCWSTR music, bool Replace);
    bool ClearList();

    void StartPlay(const std::string_view StartID);
    void StartPlay();
    void EndPlay();

    inline bool IsPlaying() { return Data.Playing.load(); }
    void PlayNext(bool AllowCyclePlay, bool IgnoreResult = true);
    void PlayPrev(bool AllowCyclePlay, bool IgnoreResult = true);

    void SetSwitchMode(SwitchMode Mode, bool IgnoreResult = true);
    void SetPlayFromFirst(bool IgnoreResult = true);
    void SwitchTo(const std::string_view ID, bool IgnoreResult = true);
    void SwitchTo(int Sequence, bool IgnoreResult = true);
    void PlayCurrent(bool IgnoreResult = true);
    void StopCurrent(bool IgnoreResult = true);
    void PauseCurrent(bool IgnoreResult = true);
    void ResumeCurrent(bool IgnoreResult = true);
    void SetVolume(DWORD Volume, bool IgnoreResult = true);//0~1000
    void SetCurrentStartTime(DWORD StartMilli, bool IgnoreResult = true);

    DWORD SyncTotalTime();
    DWORD SyncCurrentTime();
    DWORD SyncCurrentMusicIdx();
    void AsyncTotalTime();
    void AsyncCurrentTime();
    void AsyncCurrentMusicIdx();
    DWORD WaitForResult();
    std::vector<DWORD> WaitForResult(size_t Count);
    size_t ReceivedCount();

    static const _UTF8 char* GetSwitchModeStr(SwitchMode Mode);
    inline std::string_view GetMusicName(size_t Idx) { return (Idx >= 0 && Idx < Data.Sequence.size()) ? Data.Sequence.at(Idx) : ""; }
    inline std::string_view SyncCurrentMusicName() { return GetMusicName(SyncCurrentMusicIdx()); }
    inline std::vector<std::string>& GetSequence() { return Data.Sequence; }
    inline SwitchMode GetCurrentSwitchMode() { return Data.Mode; }
    inline const _UTF8 char* GetCurrentSwitchModeStr() { return GetSwitchModeStr(Data.Mode); }
};
