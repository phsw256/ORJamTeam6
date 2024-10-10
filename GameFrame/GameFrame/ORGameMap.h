#pragma once
#include "ORImage.h"
#include <vector>
#include <string_view>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

template<typename T>
class ORResourcePool;

struct ORDrawPosition :public ImVec2
{
    float ZOrder;//MapPos 
    float ZDrawOffset;//ScreenPos
    ORDrawPosition() = default;
    ORDrawPosition(const ORDrawPosition&) = default;
    ORDrawPosition(ORDrawPosition&&) = default;
    ORDrawPosition(float X, float Y, float Zorder, float Zdofs) :ImVec2(X, Y), ZOrder(Zorder), ZDrawOffset(Zdofs) {}
    inline float  operator[] (size_t idx) const { IM_ASSERT(idx <= 3); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
    inline float& operator[] (size_t idx) { IM_ASSERT(idx <= 3); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
};

class ORDrawableTile
{
protected:
    ORResPtr<ORImage> Image;
public:
    inline ORResPtr<ORImage>& GetImage() { return Image; }
    ORDrawableTile() = delete;
    ORDrawableTile(ORResPtr<ORImage> Img) :Image(Img) {}
    virtual ~ORDrawableTile() = default;
    virtual void DrawUI();
    virtual void DrawAt(ImDrawList& List, ImVec2 ScreenPos, const ORDrawPosition& Pos);
    virtual void UpdateTile() {}
    virtual void OnClick() {}
    inline ImRect DeltaRect() { return ImRect{ Image->GetDelta(), Image->GetSize() + Image->GetDelta() }; }
    inline ImVec2 DeltaRectMin() { return Image->GetDelta(); }
    inline ImVec2 DeltaRectMax() { return Image->GetSize() + Image->GetDelta(); }
};

class ORMapObject :public ORDrawableTile
{
public:
    ORMapObject(ORResPtr<ORImage> Img) :ORDrawableTile(Img) {}
};

struct ORTileMapSetting
{
    ImVec2 SizeRatio;
    ImVec2 CenterMapPos;//Map
    ImVec2 CenterDrawPos;//Screen
    ImRect DrawBorder;//Screen
    ImRect ProcessBorder;//Screen
    ImDrawList* DrawTarget{ nullptr };
};

template<bool Clickable>
class ORIsoTileTraits
{
public:
    const static bool IsClickable = Clickable;
    struct Compare
    {
        bool operator()(const ORDrawPosition& a, const ORDrawPosition& b)const;
    };
    static ImVec2 DrawPosToScreen(ORTileMapSetting& Setting, ORDrawPosition Pos);
};

template<bool Clickable>
class ORPlainTileTraits
{
public:
    const static bool IsClickable = Clickable;
    struct Compare
    {
        bool operator()(const ORDrawPosition& a, const ORDrawPosition& b)const;
    };
    static ImVec2 DrawPosToScreen(ORTileMapSetting& Setting, ORDrawPosition Pos);
};

template<typename TileTrait>
class ORTileMap
{
    using Compare = typename TileTrait::Compare;
protected:
    ORTileMapSetting Setting;
    ImVec2 MapSize;
    std::multimap<ORDrawPosition, ORResPtr<ORDrawableTile>, Compare>Tiles;
public:
    using TileIter = typename std::multimap<ORDrawPosition, ORResPtr<ORDrawableTile>, Compare>::iterator;
    inline TileIter IterEnd() { return Tiles.end(); }

    inline void SetDrawSetting(const ORTileMapSetting& s) { Setting = s; }
    inline const ORTileMapSetting& GetDrawSetting()const { return Setting; }
    inline ORTileMapSetting& GetDrawSetting() { return Setting; }
    void DrawUI();
    inline void SetView(ImVec2 NewMapPos) { Setting.CenterMapPos = NewMapPos; }
    inline void MoveView(ImVec2 NewMapPos) { Setting.CenterMapPos += NewMapPos; }
    inline void MoveViewX(float XDelta) { Setting.CenterMapPos.x += XDelta; }
    inline void MoveViewY(float YDelta) { Setting.CenterMapPos.y += YDelta; }
    void RandomGenerate(int X,int Y, ORResourcePool<ORImage>& Pool, const std::vector<std::string>& SelectList);
};

using ORIsoTileMap = ORTileMap<ORIsoTileTraits<false>>;
using ORPlainTileMap = ORTileMap<ORPlainTileTraits<false>>;
using ORClickableIsoTileMap = ORTileMap<ORIsoTileTraits<true>>;
using ORClickablePlainTileMap = ORTileMap<ORPlainTileTraits<true>>;

bool ContainPart(ImRect Border, ImRect Obj);

template<typename TileTrait>
void ORTileMap<TileTrait>::DrawUI()//With Update Tiles
{
    if (!Setting.DrawTarget)return;
    Setting.DrawTarget->PushClipRect(Setting.DrawBorder.Min, Setting.DrawBorder.Max);
    ImVec2 Cursor = ImGui::GetCursorPos();
    int i = 0;
    for (auto& p : Tiles)
    {
        auto ScrPos = TileTrait::DrawPosToScreen(Setting, p.first);
        auto pImg = p.second->GetImage().get();
        if (!pImg)continue;
        auto Rect = ImRect(ScrPos + pImg->GetDelta(), ScrPos + pImg->GetSize() + pImg->GetDelta());
        if (ContainPart(Setting.ProcessBorder, Rect))
            p.second->UpdateTile();
        if (ContainPart(Setting.DrawBorder, Rect))
        {
            p.second->DrawAt(*Setting.DrawTarget, ScrPos, p.first);
            if constexpr (TileTrait::IsClickable)
            {
                ImGui::SetCursorPos(Rect.Min);
                ImGui::Dummy(pImg->GetSize());
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                {
                    p.second->OnClick();
                }
            }
        }
            
    }
    Setting.DrawTarget->PopClipRect();
    ImGui::SetCursorPos(Cursor);
}

template<typename TileTrait>
void ORTileMap<TileTrait>::RandomGenerate(int X, int Y, ORResourcePool<ORImage>& Pool, const std::vector<std::string>& SelectList)
{
    inline ORResPtr<ORImage> Pool_GetResource_Helper(ORResourcePool<ORImage>&Pool, std::string_view Name);
    Tiles.clear();
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++)
        {
            auto pImg = Pool_GetResource_Helper(Pool, SelectList[rand() % SelectList.size()]);
            Tiles.emplace(ORDrawPosition((float)i, (float)j, 0.0F, 0.0F),
                std::make_shared<ORMapObject>(pImg));
        }
}

template<bool Clickable>
bool ORIsoTileTraits<Clickable>::Compare::operator()(const ORDrawPosition& a, const ORDrawPosition& b)const
{
    auto L = a.x + a.y + a.ZOrder, R = b.x + b.y + b.ZOrder;
    if (L == R)
    {
        if (a.y != b.y)return a.y > b.y;
        else return a.x < b.x;
    }
    return L < R;
}

ImVec2 RotateToScreenPos(ImVec2 Pos);

template<bool Clickable>
ImVec2 ORIsoTileTraits<Clickable>::DrawPosToScreen(ORTileMapSetting& Setting, ORDrawPosition Pos)
{
    return Setting.CenterDrawPos + Setting.SizeRatio * RotateToScreenPos(*((ImVec2*)&Pos) - Setting.CenterMapPos) + ImVec2{ 0,Pos.ZDrawOffset };
}
template<bool Clickable>
bool ORPlainTileTraits<Clickable>::Compare::operator()(const ORDrawPosition& a, const ORDrawPosition& b)const
{
    return a.y + a.ZOrder < b.y + b.ZOrder;
}
template<bool Clickable>
ImVec2 ORPlainTileTraits<Clickable>::DrawPosToScreen(ORTileMapSetting& Setting, ORDrawPosition Pos)
{
    return Setting.CenterDrawPos + Setting.SizeRatio * (*((ImVec2*)&Pos) - Setting.CenterMapPos) + ImVec2{ 0,Pos.ZDrawOffset };
}
