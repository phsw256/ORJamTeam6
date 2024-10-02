#include "ORBackType.h"

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
    if (MergeUpValue && UpValue != nullptr && Another.UpValue != nullptr && UpValue != Another.UpValue)
        UpValue->Merge(*Another.UpValue, true);
}
const std::string& ORVariableList::GetVariable(const std::string& Name) const
{
    static std::string Null = "";
    auto It = Value.find(Name);
    if (It != Value.end())return It->second;
    else if (UpValue == nullptr)return Null;
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
