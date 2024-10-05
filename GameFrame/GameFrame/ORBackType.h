/***********************************************************************************************
 ***                                 OR_JAM_6——源代码                                      ***
 ***********************************************************************************************
 *                                                                                             *
 *                       文件名 : ORBackType.h                                                 *
 *                                                                                             *
 *                     编写名单 : IronHammer_Std                                               *
 *                                                                                             *
 *                     新建日期 : 2024/10/1                                                    *
 *                                                                                             *
 *                     最近编辑 : 2024/10/1 IronHammer_Std                                     *
 *                                                                                             *
 *                     单元测试 : 2022/10/5 IronHammer_Std                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*/
/* 注：类型标记
 *     T：类型       TP：模板参数类型   V：值          TV：模板参数值
 *     F：函数       SF：静态函数       VF：虚函数     PVF：纯虚函数
 */
#pragma once
#include "ExtJson.h"
#include "ORFile.h"
#include "Include.h"
#include "MusicMCI.h"

 /***********************************************************************************************
  *   HintedName -- ID+描述的组合    T    struct
  *     Name     -- 一个ID名字       V    _UTF8 std::string           
  *     Desc     -- 描述内容         V    _UTF8 std::string
  *     Hint     -- 描述提示         V    _UTF8 std::string
  *                                                                                             
  * 内容：                                                                                      
  *  -- 描述了带有提示、描述、ID的一个类型                                                      
  *                                                                                             
  * 注意：                                                                                      
  *  -- 无                     
  *=============================================================================================*/
struct HintedName
{
    _UTF8 std::string Name, Desc, Hint;

};

/***********************************************************************************************
 *   <T>                    -- T 是待筛选的类型   TP
 *   ORFilter               -- 筛选器基类         T        class
 *     ~ORFilter            -- 析构函数           VF       DTOR
 *     IsSelected           -- 是否被筛选出       PVF      bool (const T&)
 *     operator()           -- =IsSelected        F        bool (const T&)
 *
 * 内容：
 *  -- 是筛选器类的公共基类
 *  -- 用来辅助筛选（如ORTopBar里面切换顶部菜单）
 *
 * 注意：
 *  -- 由于实现了operator()，筛选规则可以使用std::function接收
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
 *   <T>                    -- T 是待筛选的类型    TP
 *   ORSetFilter            -- 交集式筛选器        T       class
 *     Range                -- 被选出的范围        V       std::set<T>
 *     ~ORSetFilter         -- 析构函数            VF      DTOR
 *     IsSelected           -- 是否被筛选出        VF      bool (const T&)
 *
 * 内容：
 *  -- 是筛选Range里面包括的元素的筛选器
 *  -- 用来辅助筛选（如ORTopBar里面切换顶部菜单）
 *
 * 注意：
 *  -- 由于实现了operator()，筛选规则可以使用 std::function 接收
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
 *   <T,Pred>            -- T 是待筛选的类型
 *                          Pred 是比较谓词     TP
 *   ORCompareFilter     -- 比较式筛选器        T     class
 *   私有成员：
 *     value             -- 筛选的阈值          V     T
 *   公开成员：
 *     ~ORCompareFilter  -- 析构函数            VF      DTOR
 *     IsSelected        -- 是否被筛选出        VF     bool (const T&)
 *
 * 内容：
 *  -- 是按照与阈值的比较来筛选的筛选器
 *  -- 用来辅助筛选（如ORTopBar里面切换顶部菜单）
 *
 * 注意：
 *  -- 由于实现了operator()，筛选规则可以使用 std::function 接收
 *  -- Pred描述了比较方式，如取 std::less<T> 时表示筛选小于阈值的量
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
 *   <T>                 -- T 是资源池存储的类型     TP
 *   ORResourcePool      -- 基本的资源池             T    class
 *   私有成员：
 *     Pool              -- 资源池的 Map 对象        V    std::unordered_map<std::string, ORResPtr<T>>
 *   公开成员：
 *     Insert            -- 加入资源                 F    bool (const std::string_view, bool, T*)
 *     Emplace           -- 构造资源                 F    bool (const std::string_view, bool, TArgs&&...)
 *     EmplaceAndReturn  -- 构造资源并返回句柄       F    std::pair<typename std::unordered_map<std::string, ORResPtr<T>>::iterator,bool> (const std::string_view, bool, TArgs&&...)
 *     ItEnd             -- 与EmplaceAndReturn配合   F    typename std::unordered_map<std::string, ORResPtr<T>>::iterator ()
 *     GetResource       -- 获取资源                 F    ORResPtr<T> (const std::string_view)
 *                       -- 获取到资源或返回默认值   F    ORResPtr<T> (const std::string_view, const ORResPtr<T>&)
 *     DeleteResource    -- 释放指定资源             F    bool (const std::string_view)
 *     Clear             -- 释放全部资源             F    void ()
 * 
 * 内容：
 *  -- 作为资源池对象，用于提前加载并反复引用
 *
 * 注意：
 *  -- 存储的是 ORResPtr 的量，在引用计数为0时会自动释放
 *  -- 如果 DeleteResource(或Clear) 释放的资源正在被使用，那么将会延迟到资源使用结束、无人持有后释放
 *  -- ORResourcePool 不是线程安全的
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
 *   <T>                 -- T 是栈存储的类型       TP
 *   ORInfoStack         -- 异步消息栈             T     class
 *     Cont              -- 容器类型               T     std::vector<T>
 *   私有成员：
 *     Store             -- 资源池的 Map 对象      V     Cont
 *     Lock              -- 互斥锁                 V     std::mutex
 *   公开成员： 
 *     Empty             -- 是否为空               F     bool ()
 *     Size              -- 容器大小               F     size_t ()
 *     GetStoreRaw       -- 获取原始容器           F     Cont& ()
 *     GetCopy           -- 获取内容的副本         F     Cont ()
 *                       -- 向目标复制内容         F     void (Cont&)
 *     SetCont           -- 从容器赋值             F     void (const Cont&)
 *                       -- 从迭代器对赋值         F     void (std::vector<T>::iterator, std::vector<T>::iterator)
 *     Clear             -- 清空容器               F     void ()
 *     GetCopyAndClear   -- 把内容移出容器处理     F     Cont ()
 *                       -- 把内容移到目标处理     F     void (Cont&)
 *     Push              -- 放入容器               F     void (const T& Ct)
 * 
 * 内容：
 *  -- 异步消息栈，用于简单的线程间通讯、传输
 *
 * 注意：
 *  -- 在需要线程安全的 vector 时可以先用这个
 *  -- GetStoreRaw 可能破坏线程安全，拿到原始容器后应小心处置
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
 *   ORVariableList      -- 可继承的动态属性       T     class
 *     UpValue           -- 链式继承的值           V     std::shared_ptr<ORVariableList>
 *     Value             -- 本级的属性值           V     std::unordered_map<std::string, std::string>
 *     GetPtr            -- 获取指向自己的指针     F     std::shared_ptr<ORVariableList> ()
 *     Load              -- 从Json读取属性表       F     bool (JsonObject)
 *     FillKeys          -- 填充属性值             F     void (const std::vector<std::string>&, const std::string&)
 *     Merge             -- 合并其他表中的值       F     void (const ORVariableList&, bool)
 *     GetVariable       -- 获取属性值             F     const std::string& (const std::string&) const
 *     CoverUpValue      -- 值是否存在于被继承的表 F     void (Cont&)
 *     HasValue          -- 值是否存在             F     bool (const std::string& Name) const
 *     GetText           -- 获取文本形式           F     std::string (bool) const
 *     Flatten           -- 输出从根开始的全部信息 F     void (ORVariableList&) const
 *     Read              -- 从文件读取属性表       F     void (ORReadStraw&)
 * 
 * 内容：
 *  -- 可继承的动态属性表，用于运行时修改/派生
 *
 * 注意：
 *  -- 一个表修改值后，所有由它派生的表里面的相应值都会受影响
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

    void Read(ORReadStraw& File);//不管UpValue
    //void Write(const ExtFileClass& File)const;//不管UpValue
};


/***********************************************************************************************
 *   ORPlayListImplParam       -- 内部类                 T     struct
 *   ORPlayListImpl            -- 内部类                 T     class
 *   ORAsyncPlayList           -- 音乐播放控制器         T     class
 *     SwitchMode              -- 切歌模式               T     enum class : int8_t
 *     Internal                -- 内部类                 T     struct
 *       List                  -- 存放BGM的资源池        V     ORResourcePool<ListedMusic>
 *       Sequence              -- 歌单顺序               V     std::vector<std::string>
 *       Playing               -- 是否正在播放           V     std::atomic<bool>
 *       Mode                  -- 当前切歌模式           V     SwitchMode
 *       Post                  -- 异步操作传输栈         V     ORInfoStack<std::pair<void(*)(ORPlayListImpl*), ORPlayListImplParam>> 
 *       Result                -- 计算结果传输栈         V     ORInfoStack<DWORD> 
 *       CurVolume             -- 当前音量               V     DWORD 
 *   私有成员：
 *     Data                    -- 内部数据               V     Internal
 *     Player                  -- 播放器线程             V     std::unique_ptr<std::thread> 
 *   公开成员：
 *     AddToList               -- 把BGM加入歌单          F     bool (const std::string_view, LPCWSTR, bool)
 *     ClearList               -- 清空歌单               F     bool ()
 *     StartPlay               -- 从指定曲目起播放歌单   F     void (const std::string_view)
 *                             -- 从头播放歌单           F     void ()
 *     EndPlay                 -- 停止播放歌单           F     void ()
 *     IsPlaying               -- 是否在播放歌单         F     bool ()
 *     PlayNext                -- 下一首                 F     void (bool, bool)
 *     PlayPrev                -- 上一首                 F     void (bool, bool)
 *     SetSwitchMode           -- 切换切歌模式           F     void (SwitchMode, bool)
 *     SetPlayFromFirst        -- 从头播放               F     void (bool)
 *     SwitchTo                -- 按照名字切换BGM        F     void (const std::string_view, bool)
 *     SwitchTo                -- 按照索引切换BGM        F     void (int, bool)
 *     PlayCurrent             -- 播放当前BGM            F     void (bool)
 *     StopCurrent             -- 停止当前BGM            F     void (bool)
 *     PauseCurrent            -- 暂停当前BGM            F     void (bool)
 *     ResumeCurrent           -- 恢复当前BGM            F     void (bool)
 *     SetVolume               -- 设置播放音量           F     void (DWORD Volume, bool)
 *     SetCurrentStartTime     -- 设置播放进度           F     void (DWORD , bool)
 *     SyncTotalTime           -- 同步获取BGM时长        F     DWORD ()
 *     SyncCurrentTime         -- 同步获取播放进度       F     DWORD ()
 *     SyncCurrentMusicIdx     -- 同步获取当前BGM索引    F     DWORD ()
 *     AsyncTotalTime          -- 异步获取BGM时长        F     void ()
 *     AsyncCurrentTime        -- 异步获取播放进度       F     void ()
 *     AsyncCurrentMusicIdx    -- 异步获取当前BGM索引    F     void ()
 *     WaitForResult           -- 获取异步操作的单个结果 F     DWORD ()
 *     WaitForResult           -- 获取异步操作的多个结果 F     std::vector<DWORD> (size_t)
 *     ReceivedCount           -- 获取可接收的结果个数   F     size_t ()
 *     GetSwitchModeStr        -- 从切歌模式获取名字     SF    const _UTF8 char* (SwitchMode Mode)
 *     GetMusicName            -- 从索引获取BGM名称      F     std::string_view (size_t Idx)
 *     SyncCurrentMusicName    -- 同步获取当前BGM名称    F     std::string_view ()
 *     GetSequence             -- 获取当前歌单           F     std::vector<std::string>& ()
 *     GetCurrentSwitchMode    -- 当前切歌模式           F     SwitchMode ()
 *     GetCurrentSwitchModeStr -- 当前切歌模式名字       F     const _UTF8 char* ()
 *
 * 内容：
 *  -- 音乐播放控制器，用于控制一个异步的BGM播放器
 *
 * 注意：
 *  -- 这个东西是异步控制的，由专门的播放器线程来播音乐
 *  -- 这个类是拿来放BGM的
 *  -- AddToList 和 ClearList 请在播放器没有在播放时调用
 *  -- 不推荐修改通过GetSequence获取的歌单
 *  -- SyncXX 和 WaitForResult 是同步获取的信息，所以不要频繁获取，可以用 RateClass/StrictRateClass 来控制获取频率
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
    //请在未在播放时调用以下两个
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
