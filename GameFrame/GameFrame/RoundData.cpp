#include "RoundData.h"
#include "ORComponent.h"
#include "Global.h"
#include "ORTest.h"

RewardValue Standard_Toturial
{
    5,5,5,20,300,100,
    100,100,100,100,100,100,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    50,10,3000,3000
};
RewardValue Standard
{
    5,5,5,20,3000,100,
    100,100,100,100,100,100,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    50,10,3000,3000
};


double Actual(double sup/*补充*/, double bas, double cor, double envcor/*环境修正因子*/, double spi, double mul)
{
    return bas * (cor + envcor) * spi * mul / 10000 + sup;//统一的计算公式
}

RewardValue& RewardValue::operator+=(const RewardValue& r)
{
    for (double* i = (double*)this, *j = (double*)&r; i < ((double*)this) + (sizeof(RewardValue) / sizeof(double)); i++, j++)
    {
        *i += *j;
    }
    return *this;
}
double& RewardValue::operator[](size_t Idx)
{
    return ((double*)this)[Idx];
}
double RewardValue::operator[](size_t Idx) const
{
    return ((double*)this)[Idx];
}
double RewardValue::ActQua() const
{
    return Actual(SupQua, BasQua, CorQua, EncQua, ActSpi(), 1);
}
double RewardValue::ActTec() const
{
    return Actual(SupTec, BasTec, CorTec, EncTec, ActSpi(), 1);
}
double RewardValue::ActCul() const
{
    return Actual(SupCul, BasCul, CorCul, EncCul, ActSpi(), 1);
}
double RewardValue::ActAtt() const
{
    return Actual(SupAtt, BasAtt, CorAtt, EncAtt, ActSpi(), 1);
}
double RewardValue::ActStr() const
{
    return Actual(SupStr, BasStr, CorStr, EncStr, ActSpi(), 1);
}
double RewardValue::ActSpi() const
{
    return Actual(SupSpi, BasSpi, CorSpi, EncSpi, 1, 1);
}
std::valarray<double> RewardValue::GetAct() const
{
    auto Spi = std::valarray<double>(ActSpi(), 6);
    Spi[5] = 1;
    return GetBas() * (GetCor() + GetEnc()) * Spi / 10000 + GetSup();
}
std::valarray<double> RewardValue::GetBas() const
{
    return std::valarray<double>(&BasQua, 6);
}
std::valarray<double> RewardValue::GetCor() const
{
    return std::valarray<double>(&CorQua, 6);
}
std::valarray<double> RewardValue::GetSup() const
{
    return std::valarray<double>(&SupQua, 6);
}
std::valarray<double> RewardValue::GetEnc() const
{
    return std::valarray<double>(&EncQua, 6);
}

PropDelta& PropDelta::operator+=(const PropDelta& rhs)
{
    Need += rhs.Need;
    Input += rhs.Input;
    Reward += rhs.Reward;
    return *this;
}

PropValue& PropValue::operator+=(const PropDelta& rhs)
{
    Reward += rhs.Reward;
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

ORLoadable_DefineLoaderOuter(RewardValue)
{
    Obj
    ("BasQua", BasQua, 0.0)("BasTec", BasTec, 0.0)("BasCul", BasCul, 0.0)("BasAtt", BasAtt, 0.0)("BasStr", BasStr, 0.0)("BasSpi", BasSpi, 0.0)
    ("CorQua", CorQua, 0.0)("CorTec", CorTec, 0.0)("CorCul", CorCul, 0.0)("CorAtt", CorAtt, 0.0)("CorStr", CorStr, 0.0)("CorSpi", CorSpi, 0.0)
    ("SupQua", SupQua, 0.0)("SupTec", SupTec, 0.0)("SupCul", SupCul, 0.0)("SupAtt", SupAtt, 0.0)("SupStr", SupStr, 0.0)("SupSpi", SupSpi, 0.0)
    ("EncQua", EncQua, 0.0)("EncTec", EncTec, 0.0)("EncCul", EncCul, 0.0)("EncAtt", EncAtt, 0.0)("EncStr", EncStr, 0.0)("EncSpi", EncSpi, 0.0)
    ("IncStr", IncStr, 0.0)("IncSpi", EncTec, 0.0)("IncMon", IncMon, 0.0)("AddMon", AddMon, 0.0)("AddLik", AddLik, 0.0);
}

ORLoadable_DefineLoaderOuter(PropDelta)
{
    Obj("Need", Need, 0.0)("Input", Input, 0.0)("Reward",Reward);
}

ORLoadable_DefineLoaderOuter(PropValue)
{
    Obj("Reward", Reward);
}

void TechTreeNode::DrawAt(ImDrawList& List, ImVec2 ScreenPos, const ORDrawPosition& Pos)
{
    if (Image)
    {
        ScreenPos.y += Pos.ZDrawOffset;
        Image->DrawAt(List, ScreenPos);
        ImGui::PushStyleColor(ImGuiCol_Text, pTree->TextCol.Value);
        List.AddText(Image->GetDeltaRect().GetBL() + ScreenPos + ImVec2{ FontHeight * 1.5F, FontHeight * 0.25F }, pTree->TextCol, Name.Name.c_str());
        ImGui::PopStyleColor();
    }
}

void TechTreeNode::OnHover()
{
    ImGui::PushStyleColor(ImGuiCol_PopupBg, pTree->HintBgCol.Value);
    ImGui::PushStyleColor(ImGuiCol_Text, pTree->HintTextCol.Value);
    ImGui::SetTooltip(Name.Hint.c_str());
    ImGui::PopStyleColor(2);
}

void TechTreeNode::OnClick()
{

}

void subreplace(std::string& dst_str, const std::string& sub_str, const std::string& new_str)
{
    std::string::size_type pos = 0;
    while ((pos = dst_str.find(sub_str)) != std::string::npos)   //替换所有指定子串
    {
        dst_str.replace(pos, sub_str.length(), new_str);
    }
}

ORLoadable_DefineLoaderOuter(TechTreeNode)
{
    std::string ImgName;
    Obj("Image", ImgName, LoadOrSkip{})("Era", EraName, LoadOrSkip{})("EffectValue", Value)("NameGroup", Name)
        ("RelativePos", Position)("Unlock", UnlockProgram, LoadOrSkip{})("Prereq", Prerequisite, LoadOrSkip{});
    subreplace(Name.Hint, "{Need}", std::to_string((int)Value.Need));
    subreplace(Name.Hint, "{Input}", std::to_string((int)Value.Input));
    Image = WorkSpace.ImagePool.GetResource(ImgName, WorkSpace.MissingImage);
}

ORLoadable_DefineLoaderOuter(EraData)
{
    Obj("NameGroup", Name)("Position", MapOffset)("RoundRange", RoundRange);
}

ORLoadable_DefineLoaderOuter(TechTree)
{
    Obj("Eras", Eras)("Nodes", Nodes)("InitialPos", NodeMap.GetDrawSetting().CenterMapPos)("SizeRatio", NodeMap.GetDrawSetting().SizeRatio)
        ("BgColor", BgCol)("TextColor", TextCol)("HintBgColor", HintBgCol)("HintTextColor", HintTextCol);
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
        p.second->pTree = this;
        NodeMap.Insert(p.second->Position, p.second);
    }
}

void TechTree::DrawUI()
{
    if (BgCol.Value.w > 0.001)NodeMap.FillDrawArea(BgCol);
    NodeMap.DrawUI();
}

EraData* TechTree::AtEra(int Round)
{
    for (auto& p : Eras)
    {
        if (Round >= p.second.RoundRange.x && Round < p.second.RoundRange.y)
        {
            return &p.second;
        }
    }
    return nullptr;
}

void RoundCache::Init(RulesClass& Rule)
{
    CurrentStage = Stage::Begin;
    if (Rule.ITeachYouHowToPlayThisFuckingGame)
    {
        Value.Reward = Standard_Toturial;
    }
    else
    {
        Value.Reward = Standard;
    }
    RestStr = Value.Reward.ActStr();
    RoundCount = 1;
    BeginAnim.Reset(WorkSpace.AnimPool.GetResource("Blink2"));
    BeginAnim.Play();
}
void RoundCache::NextRound(Stage_TechTree& Stage)
{
    ++RoundCount;
    auto pEra = Stage.AtEra(RoundCount);
    if (!pEra)
    {
        CurrentStage = Stage::Last;
    }
    else
    {
        CurrentStage = Stage::Begin;
    }

    Value.Reward.BasStr += Value.Reward.IncStr;
    Value.Reward.BasSpi += Value.Reward.IncSpi;
    if (RestStr >= 0)Value.Reward.BasSpi += std::min(10.0, RestStr / 100);
    else Value.Reward.BasSpi -= RestStr / 20;
    Value.Reward.AddMon += Value.Reward.IncMon;
    RestStr = Value.Reward.ActStr();
    BeginAnim.Play();
}
void RoundCache::EnterEventStage(Stage_TechTree&)
{
    CurrentStage = Stage::Event;
}
void RoundCache::CalcResult(Stage_TechTree&)
{
    CurrentStage = Stage::Result;
}
