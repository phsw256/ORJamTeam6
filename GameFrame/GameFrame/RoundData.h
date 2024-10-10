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
