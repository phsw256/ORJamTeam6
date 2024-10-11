#pragma once
#include "ORLoadable.h"
#include "ORImage.h"
#include "ORGameMap.h"

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

struct PropDelta
{
    double Tech;
    double Body;
    double Cult;
    PropDelta& operator+=(const PropDelta&);
    ORLoadable_DefineLoader;
};

struct PropValue
{
    double Tech;
    double Body;
    double Cult;
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


class EraData;
class TechTreeNode :public ORDrawableTile
{
private:
    HintedName Name;
    PropDelta Value;
public:
    std::string EraName;
    EraData* pEra;
    ORDrawPosition Position;

    TechTreeNode() :ORDrawableTile(NoInit{}) {}
    ORLoadable_DefineLoader;
    const std::string& GetName() const { return Name.Name; }
};

class EraData
{
public:
    HintedName Name;
    ImVec2 MapOffset;
    ORLoadable_DefineLoader;
};

class TechTree
{
    std::unordered_map<std::string, EraData> Eras;
    ORClickablePlainTileMap NodeMap;
    std::unordered_map<std::string, ORResPtr<TechTreeNode>> Nodes;
public:
    ORLoadable_DefineLoader;
    void DrawUI();
    ORClickablePlainTileMap& GetMap() { return NodeMap; }
};

class RoundCache
{
    
};
