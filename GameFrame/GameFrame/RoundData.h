#pragma once
#include "ORLoadable.h"
#include "ORImage.h"
#include "ORGameMap.h"

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

struct PropDelta
{
    int Value;
    PropDelta& operator+=(const PropDelta&);
    ORLoadable_DefineLoader;
};

struct PropValue
{
    int Value;
    PropValue& operator+=(const PropDelta&);
    ORLoadable_DefineLoader;
};

PropValue operator+(const PropValue&, const PropDelta&);
PropDelta operator+(const PropDelta&, const PropDelta&);

template<>
void BasicTypeLoad<ORDrawPosition>(ORJsonLoader& Obj, ORDrawPosition& Val)
{
    Obj("X", Val.x, 0.0F)("Y", Val.y, 0.0F)("ZOrder", Val.ZOrder, 0.0F)("ZOffset", Val.ZDrawOffset, 0.0F);
}

class TechTreeNode :public ORDrawableTile
{
private:
    PropDelta Value;
    HintedName Name;
    ORDrawPosition Position;
public:
    ORLoadable_DefineLoader;
};

class RoundCache
{
    
};
