
#include"external_file.h"
#include"Csf.h"


bool CSFClass::Load(const char* name)
{
    static char gbuf[10000];
    ExtFileClass fp;
    if (!fp.Open(name, "rb"))return Loaded = false;
    gbuf[4] = 0;
    fp.Read(gbuf, 1, 4);
    if (std::string(gbuf) != " FSC")return Loaded = false;
    fp.Read(&Version, 4, 1);
    fp.Read(&LblN, 4, 1);
    fp.Read(&StrN, 4, 1);
    fp.Read(&Language, 4, 1);
    fp.Read(&Reserved, 4, 1);
    //cout<<Version<<endl<<LblN<<endl<<StrN<<endl<<Language<<endl;
    uint32_t NLblStr, StrLen;
    std::string Key, Ext;
    std::wstring Value;
    bool HasExt;
    for (size_t i = 0; i < LblN; i++)
    {
        gbuf[4] = 0;
        fp.Read(gbuf, 1, 4);
        if (std::string(gbuf) != " LBL")return Loaded = false;
        fp.Read(&NLblStr, 4, 1);
        fp.Read(&StrLen, 4, 1);
        gbuf[StrLen] = 0;
        fp.Read(gbuf, 1, StrLen);
        Key = gbuf;
        //cout<<Key<<endl;

        gbuf[4] = 0;
        fp.Read(gbuf, 1, 4);
        if (std::string(gbuf) != " RTS" && std::string(gbuf) != "WRTS")return Loaded = false;
        HasExt = (std::string(gbuf) == "WRTS");
        fp.Read(&StrLen, 4, 1);
        gbuf[StrLen << 1] = gbuf[StrLen << 1 | 1] = 0;
        fp.Read(gbuf, 1, StrLen << 1);
        for (size_t i = 0; i < (StrLen << 1); i++)gbuf[i] = ~gbuf[i];
        Value = (const wchar_t*)gbuf;
        //cout<<UnicodetoMBCS(Value)<<endl;

        if (HasExt)
        {
            fp.Read(&StrLen, 4, 1);
            gbuf[StrLen] = 0;
            fp.Read(gbuf, 1, StrLen);
            Ext = gbuf;
            //cout<<Ext<<endl;
        }

        CsfMap[Key] = { Value,Ext };
    }
    fp.Close();
    return Loaded = false;
}


CSFClass::sdata CSFClass::GetStr(const std::string& Key)
{
    auto it = CsfMap.find(Key);
    if (it == CsfMap.end())return { L"","" };
    else return it->second;
}
