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
    inline bool operator<(const ORDrawPosition& b)const
    {
        auto L = x + y + ZOrder, R = b.x + b.y + b.ZOrder;
        if (L == R)
        {
            if (y != b.y)return y  > b.y;
            else return x < b.x;
        }
        return L < R;
    }
    ORDrawPosition() = default;
    ORDrawPosition(const ORDrawPosition&) = default;
    ORDrawPosition(ORDrawPosition&&) = default;
    ORDrawPosition(float X, float Y, float Zorder, float Zdofs) :ImVec2(X, Y), ZOrder(Zorder), ZDrawOffset(Zdofs) {}
    inline float  operator[] (size_t idx) const { IM_ASSERT(idx <= 3); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
    inline float& operator[] (size_t idx) { IM_ASSERT(idx <= 3); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
};

class ORDrawableTile
{
    ORResPtr<ORImage> Image;
public:
    inline ORResPtr<ORImage>& GetImage() { return Image; }
    ORDrawableTile() = delete;
    ORDrawableTile(ORResPtr<ORImage> Img) :Image(Img) {}
    virtual ~ORDrawableTile() = default;
    virtual void DrawUI();
    virtual void DrawAt(ImDrawList& List, ImVec2 ScreenPos, const ORDrawPosition& Pos);
    virtual void UpdateTile() {}
};

class ORMapObject :public ORDrawableTile
{
public:
    ORMapObject(ORResPtr<ORImage> Img) :ORDrawableTile(Img) {}
};

class ORTileMap
{
public:
    struct DrawSetting
    {
        ImVec2 SizeRatio;
        ImVec2 CenterMapPos;//Map
        ImVec2 CenterDrawPos;//Screen
        ImRect DrawBorder;//Screen
        ImRect ProcessBorder;//Screen
        ImDrawList* DrawTarget;
    };
private:
    DrawSetting Setting;
    ImVec2 MapSize;
    std::multimap<ORDrawPosition, ORResPtr<ORDrawableTile>>Tiles;
public:
    inline void SetDrawSetting(const DrawSetting& s) { Setting = s; }
    inline const DrawSetting& GetDrawSetting()const { return Setting; }
    inline DrawSetting& GetDrawSetting() { return Setting; }
    void DrawUI();//With Update Tiles
    inline void SetView(ImVec2 NewMapPos) { Setting.CenterMapPos = NewMapPos; }
    inline void MoveView(ImVec2 NewMapPos) { Setting.CenterMapPos += NewMapPos; }
    inline void MoveViewX(float XDelta) { Setting.CenterMapPos.x += XDelta; }
    inline void MoveViewY(float YDelta) { Setting.CenterMapPos.y += YDelta; }
    void RandomGenerate(int X,int Y, ORResourcePool<ORImage>& Pool, const std::vector<std::string>& SelectList);
private:
    ImVec2 MapToScreen(ImVec2 MapPos);
    ImVec2 DrawPosToScreen(ORDrawPosition Pos);
};
