#include "ORGameMap.h"
#include "ORBackType.h"

void ORDrawableTile::DrawUI()
{
    if(Image)Image->Draw();
}
void ORDrawableTile::DrawAt(ImDrawList& List, ImVec2 ScreenPos, const ORDrawPosition& Pos)
{
    if (Image)
    {
        ScreenPos.y += Pos.ZDrawOffset;
        List.AddImage(Image->GetID(), ScreenPos + Image->GetDelta(), ScreenPos + Image->GetSize() + Image->GetDelta());
    }
}

ImVec2 RotateToScreenPos(ImVec2 Pos)
{
    return ImVec2{ (Pos.x - Pos.y) * 0.5F, (Pos.x + Pos.y) * 0.5F };
}
ImVec2 ORTileMap::MapToScreen(ImVec2 MapPos)
{
    return Setting.CenterDrawPos + Setting.SizeRatio * RotateToScreenPos(MapPos - Setting.CenterMapPos);
}
ImVec2 ORTileMap::DrawPosToScreen(ORDrawPosition Pos)
{
    return Setting.CenterDrawPos + Setting.SizeRatio * RotateToScreenPos(*((ImVec2*)&Pos) - Setting.CenterMapPos) + ImVec2{ 0,Pos.ZDrawOffset };
}


bool ContainPart(ImRect Border, ImRect Obj)
{
    return
        Border.Contains(Obj.GetBL()) ||
        Border.Contains(Obj.GetBR()) ||
        Border.Contains(Obj.GetTL()) ||
        Border.Contains(Obj.GetTR());
}

void ORTileMap::DrawUI()
{
    if (!Setting.DrawTarget)return;
    Setting.DrawTarget->PushClipRect(Setting.DrawBorder.Min, Setting.DrawBorder.Max);
    int i = 0;
    for (auto& p : Tiles)
    {
        auto ScrPos = DrawPosToScreen(p.first);
        auto pImg = p.second->GetImage().get();
        if (!pImg)continue;
        auto Rect = ImRect(ScrPos + pImg->GetDelta(), ScrPos + pImg->GetSize() + pImg->GetDelta());
        if (ContainPart(Setting.ProcessBorder, Rect))
            p.second->UpdateTile();
        if (ContainPart(Setting.DrawBorder, Rect))
        {
            p.second->DrawAt(*Setting.DrawTarget, ScrPos, p.first);
            //Setting.DrawTarget->AddText(ScrPos, 0xFFFFFFFF, std::to_string(i++).c_str());//(int)(p.first.x + p.first.y)
        }
    }
    Setting.DrawTarget->PopClipRect();
}

void ORTileMap::RandomGenerate(int X, int Y, ORResourcePool<ORImage>& Pool, const std::vector<std::string>& SelectList)
{
    Tiles.clear();
    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Y; j++)
        {
            auto pImg = Pool.GetResource(SelectList[rand() % SelectList.size()]);
            Tiles.emplace(ORDrawPosition((float)i, (float)j, 0.0F, 0.0F), 
                std::make_shared<ORMapObject>(pImg));
        }
    }
}
