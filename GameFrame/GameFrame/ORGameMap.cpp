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

/*
ImVec2 ORIsoTileMap::MapToScreen(ImVec2 MapPos)
{
    return Setting.CenterDrawPos + Setting.SizeRatio * RotateToScreenPos(MapPos - Setting.CenterMapPos);
}
*/

bool ContainPart(ImRect Border, ImRect Obj)
{
    return
        Border.Contains(Obj.GetBL()) ||
        Border.Contains(Obj.GetBR()) ||
        Border.Contains(Obj.GetTL()) ||
        Border.Contains(Obj.GetTR());
}

ORResPtr<ORImage> Pool_GetResource_Helper(ORResourcePool<ORImage>& Pool, std::string_view Name)
{
    return Pool.GetResource(Name);
}
