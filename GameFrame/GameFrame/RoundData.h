#pragma once
#include "ORLoadable.h"
#include "ORAnimation.h"
#include "ORGameMap.h"
#include <valarray>

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
    ORLoadable_DefineLoader;
};

double Actual(int sup/*����*/, double bas, double cor, double envcor/*������������*/, double spi, double mul);

struct RewardValue
{
    //����ֵ������ֵ���ǰٷֱ�
    //Bas=����ֵ��Qua=���ʣ�Tec=�Ƽ���Cul=�Ļ���Att=������Str=�ж��� ��Spi=���� ��Mon=�غϽ� Lik=�øж�
    double BasQua, BasTec, BasCul, BasAtt, BasStr, BasSpi;
    double CorQua, CorTec, CorCul, CorAtt, CorStr, CorSpi;
    double SupQua, SupTec, SupCul, SupAtt, SupStr, SupSpi;
    double EncQua, EncTec, EncCul, EncAtt, EncStr, EncSpi;
    double IncStr, IncSpi, IncMon, AddMon, AddLik;
    ORLoadable_DefineLoader;
    RewardValue& operator+=(const RewardValue&);

    double& operator[](size_t Idx);
    double operator[](size_t Idx) const;
    double ActQua() const;
    double ActTec() const;
    double ActCul() const;
    double ActAtt() const;
    double ActStr() const;
    double ActSpi() const;
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

class TechTree;
class EraData;
struct RulesClass;
class Stage_TechTree;
class TechTreeNode :public ORDrawableTile
{
private:
    HintedName Name;
    PropDelta Value;
    double Progress{ 0 };
public:
    std::string EraName;
    EraData* pEra{ nullptr };
    TechTree* pTree{ nullptr };
    ORDrawPosition Position;
    std::vector<std::string> UnlockProgram;
    std::vector<std::string> Prerequisite;

    virtual void DrawAt(ImDrawList& List, ImVec2 ScreenPos, const ORDrawPosition& Pos);
    virtual void OnHover();
    virtual void OnClick();
    TechTreeNode() :ORDrawableTile(NoInit{}) {}
    ORLoadable_DefineLoader;
    const std::string& GetName() const { return Name.Name; }

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
    std::unordered_map<std::string, ORResPtr<TechTreeNode>> Nodes;
public:
    ImColor BgCol, TextCol, HintBgCol, HintTextCol;

    ORLoadable_DefineLoader;
    void DrawUI();
    ORClickablePlainTileMap& GetMap() { return NodeMap; }
    EraData* AtEra(int Round);
};

struct NodeCache
{
    
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

    RoundCache() :BeginAnim(NoInit{}),CurrentStage(Stage::First),RoundCount(0){}

    std::unordered_map <std::string, NodeCache> Nodes;

    void Init(RulesClass&);
    void NextRound(Stage_TechTree&);
    void EnterEventStage(Stage_TechTree&);
    void CalcResult(Stage_TechTree&);
};

/*
+50�ж���Base
�����ж���
+10����ֵ
*/
