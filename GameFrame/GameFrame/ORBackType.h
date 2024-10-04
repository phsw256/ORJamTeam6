#pragma once
#include "ExtJson.h"
#include "ORFile.h"
#include "Include.h"
#include "MusicMCI.h"

struct HintedName
{
    _UTF8 std::string Name, Desc, Hint;

};

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


template<typename T>
class ORResourcePool
{
private:
    std::unordered_map<std::string, ORResPtr<T>> Pool;
public:
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
    typename std::unordered_map<std::string, ORResPtr<T>>::iterator ItEnd() { return Pool.end(); }
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
};

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

struct ORVariableList
{
    ORVariableList* UpValue{ nullptr };
    std::unordered_map<std::string, std::string>Value;

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

class UpdatableMusic:public MusicMCI
{
public:
    virtual  ~UpdatableMusic() = default;
    virtual bool Update() = 0; //返回false则移除
};

class ORPlayListImplParam
{
public:
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
        ORResourcePool<MusicMCI> List;
        std::vector<std::string> Sequence;
        std::atomic<bool> Playing;
        SwitchMode Mode;
        ORInfoStack<std::pair<void(*)(ORPlayListImpl*), ORPlayListImplParam>> Post;
        ORInfoStack<DWORD> Result;
        DWORD CurVolume;
    };
private:
    Internal Data;
    std::unique_ptr<std::thread> Player;
public:
    //请在未在播放时调用
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

    DWORD TotalTime();
    DWORD CurrentTime();
    DWORD CurrentMusicIdx();
    DWORD WaitForResult();

    static const _UTF8 char* GetSwitchModeStr(SwitchMode Mode);
    inline std::string_view GetName(int Idx) { return (Idx >= 0 && Idx < (int)Data.Sequence.size()) ? Data.Sequence.at(Idx) : ""; }
    inline std::string_view GetCurrentMusicName() { return GetName((int)CurrentMusicIdx()); }
    inline std::vector<std::string>& GetSequence() { return Data.Sequence; }
    inline SwitchMode GetCurrentSwitchMode() { return Data.Mode; }
    inline const _UTF8 char* GetCurrentSwitchModeStr() { return GetSwitchModeStr(Data.Mode); }
};
