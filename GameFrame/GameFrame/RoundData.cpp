#include "RoundData.h"
#include "ORComponent.h"
#include "Global.h"

PropDelta& PropDelta::operator+=(const PropDelta& rhs)
{
    Value += rhs.Value;
    return *this;
}

PropValue& PropValue::operator+=(const PropDelta& rhs)
{
    Value += rhs.Value;
    return *this;
}

PropValue operator+(const PropValue& l, const PropDelta& r)
{
    auto p = l; p += r;
    return p;
}
PropDelta operator+(const PropDelta& l, const PropDelta& r)
{
    auto p = l; p += r;
    return p;
}

ORLoadable_DefineLoaderOuter(HintedName)
{
    Name.clear();
    Desc.clear();
    Hint.clear();
    Obj("Name", Name)("Description", Desc, Name)("Hint", Hint, LoadOrSkip{});
}

ORLoadable_DefineLoaderOuter(PropDelta)
{
    Obj("Value", Value, 0);
}

ORLoadable_DefineLoaderOuter(PropValue)
{
    Obj("Value", Value, 0);
}

ORLoadable_DefineLoaderOuter(TechTreeNode)
{
    std::string ImgName;
    Obj("Image", ImgName, LoadOrSkip{})("EffectValue", Value)("NameGroup", Name)("Position", Position);
    Image = WorkSpace.ImagePool.GetResource(ImgName, WorkSpace.MissingImage);
}
