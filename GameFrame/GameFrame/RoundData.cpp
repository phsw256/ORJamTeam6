#include "RoundData.h"
#include "ORComponent.h"
#include "Global.h"

PropDelta& PropDelta::operator+=(const PropDelta& rhs)
{
    Tech += rhs.Tech;
    Cult += rhs.Cult;
    Body += rhs.Body;
    return *this;
}

PropValue& PropValue::operator+=(const PropDelta& rhs)
{
    Tech += rhs.Tech;
    Cult += rhs.Cult;
    Body += rhs.Body;
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
    Obj("Tech", Tech, 0.0)("Body", Body, 0.0)("Culture", Cult, 0.0);
}

ORLoadable_DefineLoaderOuter(PropValue)
{
    Obj("Tech", Tech, 0.0)("Body", Body, 0.0)("Culture", Cult, 0.0);
}

ORLoadable_DefineLoaderOuter(TechTreeNode)
{
    std::string ImgName;
    Obj("Image", ImgName, LoadOrSkip{})("Era", EraName, LoadOrSkip{})("EffectValue", Value)("NameGroup", Name)("RelativePos", Position);
    Image = WorkSpace.ImagePool.GetResource(ImgName, WorkSpace.MissingImage);
}

ORLoadable_DefineLoaderOuter(EraData)
{
    Obj("NameGroup", Name)("Position", MapOffset);
}

ORLoadable_DefineLoaderOuter(TechTree)
{
    Obj("Eras", Eras)("Nodes", Nodes)("InitialPos", NodeMap.GetDrawSetting().CenterMapPos)("SizeRatio", NodeMap.GetDrawSetting().SizeRatio);
    for (auto& p : Nodes)
    {
        auto it=Eras.find(p.second->EraName);
        if (it == Eras.end())p.second->pEra = nullptr;
        else
        {
            p.second->pEra = &it->second;
            p.second->Position.x += p.second->pEra->MapOffset.x;
            p.second->Position.y += p.second->pEra->MapOffset.y;
        }
        NodeMap.Insert(p.second->Position, p.second);
    }
}

void TechTree::DrawUI()
{
    NodeMap.DrawUI();
}
