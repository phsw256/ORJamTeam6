#pragma once
#include "ORLoadable.h"
#include "ORAnimation.h"
#include "ORGameMap.h"
#include <valarray>

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
    ORLoadable_DefineLoader;
};

double Actual(int sup/*补充*/, double bas, double cor, double envcor/*环境修正因子*/, double spi, double mul);

struct RewardValue
{
    //精神值、修正值都是百分比
    //Bas=基础值，Qua=体质，Tec=科技，Cul=文化，Att=魅力，Str=行动力 ，Spi=精神 ，Mon=回合金， Lik=好感度
    double BasQua, BasTec, BasCul, BasAtt, BasStr, BasSpi;
    double CorQua, CorTec, CorCul, CorAtt, CorStr, CorSpi;
    double SupQua, SupTec, SupCul, SupAtt, SupStr, SupSpi;
    double EncQua, EncTec, EncCul, EncAtt, EncStr, EncSpi;
    double IncStr, IncSpi, IncMon, AddMon, AddLik;
    ORLoadable_DefineLoader;
    RewardValue& operator+=(const RewardValue&);

    double& operator[](size_t Idx);
    double operator[](size_t Idx) const;
    double ActQua(double EncFix = 0) const;
    double ActTec(double EncFix = 0) const;
    double ActCul(double EncFix = 0) const;
    double ActAtt(double EncFix = 0) const;
    double ActStr(double EncFix = 0) const;
    double ActSpi(double EncFix = 0) const;
    std::valarray<double> GetAct() const;
    std::valarray<double> GetBas() const;
    std::valarray<double> GetCor() const;
    std::valarray<double> GetSup() const;
    std::valarray<double> GetEnc() const;
};

struct PropDelta
{
    double Need;
    double Input;
    RewardValue Reward;
    PropDelta& operator+=(const PropDelta&);
    ORLoadable_DefineLoader;
};

struct PropValue
{
    RewardValue Reward;
    PropValue& operator+=(const PropDelta&);
    ORLoadable_DefineLoader;
};

PropValue operator+(const PropValue&, const PropDelta&);
PropDelta operator+(const PropDelta&, const PropDelta&);

template<>
inline void BasicTypeLoad<ORDrawPosition>(ORJsonLoader& Obj, ORDrawPosition& Val)
{
    Obj("X", Val.x, 0.0F)("Y", Val.y, 0.0F)("ZOrder", Val.ZOrder, 0.0F)("ZOffset", Val.ZDrawOffset, 0.0F);
}

struct NodeCache
{
    int StrInput;
};

class TechTree;
class EraData;
struct RulesClass;
class Stage_TechTree;
class RoundCache;
class ORDescManager;
class TechTreeNode :public ORDrawableTile
{
private:
    bool Enabled{ false };
    HintedName Name;
    PropDelta Value;
    
public:
    double Progress{ 0 };
    std::string ID;
    std::string EraName;
    EraData* pEra{ nullptr };
    TechTree* pTree{ nullptr };
    ORDrawPosition Position;
    std::vector<std::string> UnlockProgram;
    std::vector<std::string> Prerequisite;

    virtual void DrawAt(ImDrawList& List, const ORDrawPosition& Pos);
    virtual void OnHover();
    virtual void OnClick();
    virtual void UpdateTileAlways(ImDrawList& List);
    TechTreeNode() :ORDrawableTile(NoInit{}) {}
    ORLoadable_DefineLoader;
    const std::string& GetName() const { return Name.Name; }
    inline NodeCache& GetCache();
    inline double StrToProgress(double ActualStr);
    inline bool Complete() { return Progress > Value.Input - 0.0001; }
};

class EraData
{
public:
    HintedName Name;
    ImVec2 MapOffset;
    ImVec2 RoundRange;
    ORLoadable_DefineLoader;
};



class TechTree
{
    std::unordered_map<std::string, EraData> Eras;
    ORClickablePlainTileMap NodeMap;
public:
    std::unordered_map<std::string, ORResPtr<TechTreeNode>> Nodes;
    ImColor BgCol, TextCol, HintBgCol, HintTextCol;
    RoundCache* pCache;
    std::string ID;

    ORLoadable_DefineLoader;
    void DrawUI();
    ORClickablePlainTileMap& GetMap() { return NodeMap; }
    EraData* AtEra(int Round);
    TechTreeNode* GetNode(const std::string_view ID);
};




class RoundCache
{
public:

    enum class Stage
    {
        First = 0,
        Begin = 1,
        Event = 2,
        Action = 3,
        Result = 4,
        End = 5,
        Last =6,
    };

    Stage CurrentStage;
    PropValue Value;
    double RestStr;
    int RoundCount;
    ORAnim BeginAnim;
    ORDescManager* pDsc;
    std::unordered_map <std::string, NodeCache> Cache;

    RoundCache() :BeginAnim(NoInit{}),CurrentStage(Stage::First),RoundCount(0){}

    void Init(RulesClass&);
    void NextRound(Stage_TechTree&);
    void EnterEventStage(Stage_TechTree&);
    void CalcResult(Stage_TechTree&);
};

/*
+50行动力Base
回满行动力
+10精神值
*/
