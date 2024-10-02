#pragma once
#include "ExtJson.h"
#include "ORFile.h"
#include "Include.h"

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
        return Pool.insert({ std::string(Name), std::make_shared<T>(std::forward<TArgs>(Args)...) }).second;
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
