#include "ORBackType.h"
#include "Global.h"
#include "Include.h"

bool ORVariableList::Load(JsonObject FromJson)
{
    Value = FromJson.GetMapString();
    return true;
}
void ORVariableList::FillKeys(const std::vector<std::string>& List, const std::string& Val)
{
    for (const auto& s : List)Value[s] = Val;
}
void ORVariableList::Merge(const ORVariableList& Another, bool MergeUpValue)
{
    for (const auto& p : Another.Value)
        Value[p.first] = p.second;
    if (MergeUpValue && Another.UpValue && UpValue != Another.UpValue)
        Merge(*Another.UpValue, true);
}
const std::string& ORVariableList::GetVariable(const std::string& Name) const
{
    static const std::string Null = "";
    auto It = Value.find(Name);
    if (It != Value.end())return It->second;
    else if (!UpValue)return Null;
    else return UpValue->GetVariable(Name);
}
bool ORVariableList::HasValue(const std::string& Name) const
{
    if ((Value.find(Name) != Value.end()))return true;
    else if (UpValue == nullptr)return false;
    else return UpValue->HasValue(Name);
}
bool ORVariableList::CoverUpValue(const std::string& Name) const
{
    if (UpValue == nullptr)return false;
    else return (Value.find(Name) != Value.end()) && UpValue->HasValue(Name);
}
std::string ORVariableList::GetText(bool ConsiderUpValue) const
{
    if (!ConsiderUpValue)
    {
        std::string Ret;
        for (const auto& p : Value)
        {
            Ret += p.first;
            Ret.push_back('=');
            Ret += p.second;
            Ret.push_back('\n');
        }
        return Ret;
    }
    else
    {
        ORVariableList List;
        Flatten(List);
        return List.GetText(false);
    }
}
void ORVariableList::Flatten(ORVariableList& Target) const
{
    if (UpValue != nullptr)UpValue->Flatten(Target);
    for (const auto& p : Value)
        Target.Value[p.first] = p.second;
}
void ORVariableList::Read(ORReadStraw& File)
{
    std::vector<std::string> sv;
    File.ReadVector(sv);
    for (size_t i = 0; i + 1 < sv.size(); i += 2)
        Value.insert({ std::move(sv.at(i)),std::move(sv.at(i + 1)) });
}
/*
void ORVariableList::Write(const ExtFileClass& File)const
{
    std::vector<std::string> sv;
    for (const auto& p : Value)
    {
        sv.push_back(p.first);
        sv.push_back(p.second);
    }
    File.WriteVector(sv);
}
*/



class ORPlayListImpl
{
public:
    ORAsyncPlayList::Internal* Data;
    int* Current;
    ORPlayListImplParam Param;

    ORResPtr<ListedMusic> GetCurrentMusic()
    {
        //if (Data->Sequence.size() > (size_t)*Current)return Data->List.GetResource(Data->Sequence[*Current]);
        //else return ORResPtr<MusicMCI>{};
        return Data->List.GetResource(Data->Sequence[*Current]);
    }
    void Send(DWORD result)
    {
        if (!Param.IgnoreResult)Data->Result.Push(result);
    }
};

void Internal_TotalTime(ORPlayListImpl* pImpl);
void Internal_CurrentTime(ORPlayListImpl* pImpl);
void Internal_CurrentIdx(ORPlayListImpl* pImpl);
void Internal_PlayNext(ORPlayListImpl* pImpl);
void Internal_PlayPrev(ORPlayListImpl* pImpl);
void Internal_SwitchTo(ORPlayListImpl* pImpl);
void Internal_TotalTime(ORPlayListImpl* pImpl);
void Internal_SetVolume(ORPlayListImpl* pImpl);
void Internal_SetCurrentStartTime(ORPlayListImpl* pImpl);
void Internal_PlayCurrent(ORPlayListImpl* pImpl);
void Internal_StopCurrent(ORPlayListImpl* pImpl);
void Internal_PauseCurrent(ORPlayListImpl* pImpl);
void Internal_ResumeCurrent(ORPlayListImpl* pImpl);
void Internal_SetSwitchMode(ORPlayListImpl* pImpl);

void ORAsyncPlayList_ThreadLoop(ORAsyncPlayList::Internal* Data, const std::string_view StartID)
{
    int CurrentSong = 0;
    ORPlayListImpl StackParam;
    StackParam.Current = &CurrentSong;
    StackParam.Data = Data;

    Data->Playing.store(true);
    if (Data->Sequence.empty())
    {
        while (Data->Playing.load())std::this_thread::sleep_for(std::chrono::milliseconds(5));
        return;
    }
    else
    {
        if (StartID.empty())
        {
            switch (Data->Mode)
            {
            case ORAsyncPlayList::SwitchMode::Random:
                CurrentSong = rand() % Data->Sequence.size(); break;
            case ORAsyncPlayList::SwitchMode::Ordered:
            case ORAsyncPlayList::SwitchMode::Loop:
                CurrentSong = 0; break;
            }
        }
        else
        {
            for (size_t i = 0; i < Data->Sequence.size(); i++)
                if (Data->Sequence[i] == StartID)
                {
                    CurrentSong = i; break;
                }
        }
        auto pCurMusic = StackParam.GetCurrentMusic();
        if (pCurMusic)
        {
            pCurMusic->open_play();
            pCurMusic->setVolume(Data->CurVolume);
        }
    }
    while (Data->Playing.load())
    {
        for (auto& p : Data->Post.GetCopyAndClear())
        {
            StackParam.Param = p.second;
            p.first(&StackParam);
        }
        auto pCur = StackParam.GetCurrentMusic();
        if (pCur)
        {
            DWORD Cur{}, Tot{};
            pCur->getCurrentTime(Cur);
            pCur->getTotalTime(Tot);
            if (Cur >= Tot)
            {
                switch (Data->Mode)
                {
                case ORAsyncPlayList::SwitchMode::Random:
                    StackParam.Param.Param = rand() % Data->Sequence.size(); break;
                case ORAsyncPlayList::SwitchMode::Ordered:
                    StackParam.Param.Param = (CurrentSong + 1) % Data->Sequence.size(); break;
                case ORAsyncPlayList::SwitchMode::Loop:
                    StackParam.Param.Param = CurrentSong; break;
                }
                StackParam.Param.IgnoreResult = true;
                Internal_SwitchTo(&StackParam);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto pCur = Data->List.GetResource(Data->Sequence.at(CurrentSong));
    pCur->stop_close();
    return;
}

bool ORAsyncPlayList::AddToList(const std::string_view ID, LPCWSTR music, bool Replace)
{
    if (IsPlaying())return false;
    auto [it, success] = Data.List.EmplaceAndReturn(ID, Replace);
    if (!success)return false;
    if (it == Data.List.ItEnd())return false;
    /*if (!it->second->open(music))
    {
        Data.List.DeleteResource(ID); return false;
    }
    */
    it->second->SetName(music);
    Data.Sequence.emplace_back(ID);
    return true;
}
bool ORAsyncPlayList::ClearList()
{
    if (IsPlaying())return false;
    Data.List.Clear();
    Data.Sequence.clear();
    return true;
}

void ORAsyncPlayList::StartPlay(const std::string_view StartID)
{
    GlobalLog.AddLog(Player.get());
    if (Player)return;
    Player.reset(new std::thread(ORAsyncPlayList_ThreadLoop, &Data, StartID));
    while (!IsPlaying())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}
void ORAsyncPlayList::StartPlay()
{
    if (Data.Sequence.empty())StartPlay("");
    else StartPlay(Data.Sequence.at(0));
}
void ORAsyncPlayList::EndPlay()
{
    if (!Player)return;
    Data.Playing.store(false);
    Player->join();
    Data.Post.Clear();
    Data.Result.Clear();
    Player.reset();
}

DWORD ORAsyncPlayList::WaitForResult()
{
    std::vector<DWORD> vs;
    while (Data.Result.Empty())std::this_thread::sleep_for(std::chrono::microseconds(100));
    Data.Result.GetCopyAndClear(vs);
    auto Ret = vs[0];
    Data.Result.SetCont(vs.begin() + 1, vs.end());
    return Ret;
}
std::vector<DWORD> ORAsyncPlayList::WaitForResult(size_t Count)
{
    std::vector<DWORD> vs;
    while (Data.Result.Size() < Count) std::this_thread::sleep_for(std::chrono::microseconds(100));
    Data.Result.GetCopyAndClear(vs);
    Data.Result.SetCont(vs.begin() + Count, vs.end());
    return std::vector<DWORD>(vs.begin(), vs.begin() + Count);
}
size_t ORAsyncPlayList::ReceivedCount()
{
    return Data.Result.Size();
}

void ORAsyncPlayList::PlayNext(bool AllowCyclePlay, bool IgnoreResult)
{
    Data.Post.Push({ Internal_PlayNext, ORPlayListImplParam{ DWORD(AllowCyclePlay) , IgnoreResult}});
}
void ORAsyncPlayList::PlayPrev(bool AllowCyclePlay, bool IgnoreResult)
{
    Data.Post.Push({ Internal_PlayPrev, ORPlayListImplParam{ DWORD(AllowCyclePlay) , IgnoreResult} });
}
void ORAsyncPlayList::SetSwitchMode(SwitchMode Mode, bool IgnoreResult)
{
    Data.Post.Push({ Internal_SetSwitchMode, ORPlayListImplParam{ DWORD(Mode) , IgnoreResult} });
}
void ORAsyncPlayList::SetPlayFromFirst(bool IgnoreResult)
{
    SwitchTo(0, IgnoreResult);
}
void ORAsyncPlayList::SwitchTo(const std::string_view ID, bool IgnoreResult)
{
    for (size_t i = 0; i < Data.Sequence.size(); i++)
        if (Data.Sequence[i] == ID)
        {
            SwitchTo(i, IgnoreResult); break;
        }
}
void ORAsyncPlayList::SwitchTo(int Sequence, bool IgnoreResult)
{
    Data.Post.Push({ Internal_SwitchTo, ORPlayListImplParam{ DWORD(Sequence) , IgnoreResult} });
}
void ORAsyncPlayList::PlayCurrent(bool IgnoreResult)
{
    Data.Post.Push({ Internal_PlayCurrent, ORPlayListImplParam{ 0 , IgnoreResult} });
}
void ORAsyncPlayList::StopCurrent(bool IgnoreResult)
{
    Data.Post.Push({ Internal_StopCurrent, ORPlayListImplParam{ 0 , IgnoreResult} });
}
void ORAsyncPlayList::PauseCurrent(bool IgnoreResult)
{
    Data.Post.Push({ Internal_PauseCurrent, ORPlayListImplParam{ 0 , IgnoreResult} });
}
void ORAsyncPlayList::ResumeCurrent(bool IgnoreResult)
{
    Data.Post.Push({ Internal_ResumeCurrent, ORPlayListImplParam{ 0 , IgnoreResult} });
}
void ORAsyncPlayList::SetVolume(DWORD Volume, bool IgnoreResult)//0~1000
{
    Data.Post.Push({ Internal_SetVolume, ORPlayListImplParam{ Volume , IgnoreResult} });
}
void ORAsyncPlayList::SetCurrentStartTime(DWORD StartMilli, bool IgnoreResult)
{
    Data.Post.Push({ Internal_SetCurrentStartTime, ORPlayListImplParam{ StartMilli , IgnoreResult} });
}
DWORD ORAsyncPlayList::SyncTotalTime()
{
    Data.Post.Push({ Internal_TotalTime, ORPlayListImplParam{ 0 ,false } });
    return WaitForResult();
}
DWORD ORAsyncPlayList::SyncCurrentTime()
{
    Data.Post.Push({ Internal_CurrentTime, ORPlayListImplParam{ 0 ,false } });
    return WaitForResult();
}
DWORD ORAsyncPlayList::SyncCurrentMusicIdx()
{
    Data.Post.Push({ Internal_CurrentIdx, ORPlayListImplParam{ 0 ,false } });
    return WaitForResult();
}
void ORAsyncPlayList::AsyncTotalTime()
{
    Data.Post.Push({ Internal_TotalTime, ORPlayListImplParam{ 0 ,false } });
}
void ORAsyncPlayList::AsyncCurrentTime()
{
    Data.Post.Push({ Internal_CurrentTime, ORPlayListImplParam{ 0 ,false } });
}
void ORAsyncPlayList::AsyncCurrentMusicIdx()
{
    Data.Post.Push({ Internal_CurrentIdx, ORPlayListImplParam{ 0 ,false } });
}

const char* ORAsyncPlayList::GetSwitchModeStr(ORAsyncPlayList::SwitchMode Mode)
{
    static const char* str[] = { u8"随机播放",u8"顺序播放",u8"单曲循环" };
    return str[(size_t)Mode];
}

void Internal_TotalTime(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    DWORD Total{ 0 };
    if (pCurMusic)
        if (!pCurMusic->getTotalTime(Total))
            Total = 0;
    pImpl->Send(Total);
}
void Internal_CurrentTime(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    DWORD Cur{ 0 };
    if (pCurMusic)
        if (!pCurMusic->getCurrentTime(Cur))
            Cur = 0;
    pImpl->Send(Cur);
}
void Internal_CurrentIdx(ORPlayListImpl* pImpl)
{
    pImpl->Send(*pImpl->Current);
}
void Internal_PlayNext(ORPlayListImpl* pImpl)
{
    int sz = pImpl->Data->Sequence.size();
    DWORD idx = *pImpl->Current;
    if (*pImpl->Current < sz - 1 || pImpl->Param.Param)
    {
        auto pCurMusic = pImpl->GetCurrentMusic();
        if (pCurMusic)pCurMusic->stop_close();
        ++(*pImpl->Current);
        if (pImpl->Param.Param && *pImpl->Current >= sz)(*pImpl->Current) -= sz;
        idx = *pImpl->Current;
        pCurMusic = pImpl->GetCurrentMusic();
        if (pCurMusic)
        {
            pCurMusic->open_play();
            pCurMusic->setVolume(pImpl->Data->CurVolume);
        }
    }
    pImpl->Send(idx);
}
void Internal_PlayPrev(ORPlayListImpl* pImpl)
{
    auto sz = pImpl->Data->Sequence.size();
    DWORD idx = *pImpl->Current;
    if (*pImpl->Current > 0 || pImpl->Param.Param)
    {
        auto pCurMusic = pImpl->GetCurrentMusic();
        if (pCurMusic)pCurMusic->stop_close();
        if (pImpl->Param.Param && *pImpl->Current <= 0)(*pImpl->Current) += sz;
        --(*pImpl->Current);
        idx = *pImpl->Current;
        pCurMusic = pImpl->GetCurrentMusic();
        if (pCurMusic)
        {
            pCurMusic->open_play();
            pCurMusic->setVolume(pImpl->Data->CurVolume);
        }
    }
    pImpl->Send(idx);
}
void Internal_SwitchTo(ORPlayListImpl* pImpl)
{
    if ((*pImpl->Current) != pImpl->Param.Param)
    {
        auto sz = pImpl->Data->Sequence.size();
        if (pImpl->Param.Param >= 0 && pImpl->Param.Param < sz)
        {
            auto pCurMusic = pImpl->GetCurrentMusic();
            if (pCurMusic)pCurMusic->stop_close();
            (*pImpl->Current) = pImpl->Param.Param;
            pCurMusic = pImpl->GetCurrentMusic();
            if (pCurMusic)
            {
                pCurMusic->open_play();
                pCurMusic->setVolume(pImpl->Data->CurVolume);
            }
        }
    }
    else
    {
        DWORD Cur{}, Tot{};
        auto pCur = pImpl->GetCurrentMusic();
        pCur->getCurrentTime(Cur);
        pCur->getTotalTime(Tot);
        if (Cur >= Tot)
        {
            pCur->stop();
            pCur->play();
        }
    }
    pImpl->Send(*pImpl->Current);
}
void Internal_SetVolume(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    pImpl->Data->CurVolume = pImpl->Param.Param;
    pCurMusic->setVolume(pImpl->Param.Param);
    pImpl->Send(1);
}
void Internal_SetCurrentStartTime(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    DWORD Cur{ 0 };
    if (pCurMusic)
        if (pCurMusic->setStartTime(pImpl->Param.Param))
            Cur = 1;
    pImpl->Send(Cur);
}
void Internal_PlayCurrent(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    DWORD Cur{ 0 };
    if (pCurMusic)
        if (pCurMusic->play())
            Cur = 1;
    pImpl->Send(Cur);
}
void Internal_StopCurrent(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    DWORD Cur{ 0 };
    if (pCurMusic)
        if (pCurMusic->stop())
            Cur = 1;
    pImpl->Send(Cur);
}
void Internal_PauseCurrent(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    DWORD Cur{ 0 };
    if (pCurMusic)
        if (pCurMusic->pause())
            Cur = 1;
    pImpl->Send(Cur);
}
void Internal_ResumeCurrent(ORPlayListImpl* pImpl)
{
    auto pCurMusic = pImpl->GetCurrentMusic();
    DWORD Cur{ 0 };
    if (pCurMusic)
        if (pCurMusic->play())
            Cur = 1;
    pImpl->Send(Cur);
}
void Internal_SetSwitchMode(ORPlayListImpl* pImpl)
{
    pImpl->Data->Mode = ORAsyncPlayList::SwitchMode((int8_t)pImpl->Param.Param);
}

