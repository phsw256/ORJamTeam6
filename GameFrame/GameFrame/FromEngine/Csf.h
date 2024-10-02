#pragma once

#include <string>
#include <unordered_map>

class CSFClass
{
    bool Loaded{ false };
    struct sdata
    {
        std::wstring str;
        std::string ext;
    };
    std::unordered_map<std::string, sdata> CsfMap;
    std::string Name;
    uint32_t Version, LblN, StrN, Language, Reserved;
public:
    
    bool Load(const char* name);
    sdata GetStr(const std::string& Key);

    bool Available() { return Loaded; }
    void Clear()
    {
        CsfMap.clear();
        Name = "";
        Version = LblN = StrN = Language = Reserved = 0u;
        Loaded = false;
    }
};
