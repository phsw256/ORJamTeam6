#include "ORAnimation.h"
#include "ORComponent.h"
#include "Global.h"

void ORClipAnimType::MoveClip(size_t Idx)
{
    auto s = size_t(ClipCount.x);
    auto y = float(Idx / s);
    auto x = float(Idx % s);
    Image->ResetClip({ x / ClipCount.x , y / ClipCount.y }, { (x + 1) / ClipCount.x , (y + 1) / ClipCount.y });
}
bool ORClipAnimType::DrawFrameAt(size_t Idx, ImDrawList& List, ImVec2 Pos)
{
    if (!Image || Idx >= GetFrameCount())return false;
    MoveClip(Idx);
    return Image->DrawAt(List, Pos);
}
bool ORClipAnimType::DrawFrame(size_t Idx)
{
    if (!Image || Idx >= GetFrameCount())return false;
    MoveClip(Idx);
    return Image->Draw();
}
size_t ORClipAnimType::GetNextIdx(size_t Idx) const
{
    if (Sequence.size() <= Idx)return Idx + 1;
    else return Sequence[Idx];
}
inttime_t ORClipAnimType::Rate() const
{
    return (inttime_t)RateInt;
}
size_t ORClipAnimType::GetFrameCount() const
{
    return size_t(ClipCount.x) * size_t(ClipCount.y);
}
size_t ORClipAnimType::GetFirstFrame() const
{
    return FirstFrame;
}
ORLoadable_DefineLoaderOuter(ORClipAnimType)
{
    std::string ImgName;
    Obj("Image", ImgName)("Clip", ClipCount, { 1.0F,1.0F })("FirstFrame", FirstFrame, 0u)("Rate", RateInt, 0)("SequenceFull", Sequence, LoadOrSkip{});
    if (Sequence.empty())
    {
        std::vector<ImVec2> Seq;
        Obj("SequenceShort", Seq, LoadOrSkip{});
        if (!Seq.empty())
        {
            auto cnt = GetFrameCount();
            Sequence.reserve(cnt);
            for (size_t i = 0; i < cnt; i++)Sequence[i] = i + 1;
            for (auto& p : Seq)
                if (size_t(p.x) < cnt)
                    Sequence[size_t(p.x)] = size_t(p.y);
        }
    }
    Image = WorkSpace.ImagePool.GetResource(ImgName);
}

bool ORAnim::Update()
{
    if (Playing)
    {
        if (DrawRate.NextFrame())
        {
            FrameIdx = Type->GetNextIdx(FrameIdx);
            if (FrameIdx >= Type->GetFrameCount())Stop();
            return Playing;
        }
        return true;
    }
    return false;
}

bool ORAnim::Draw()
{
    if (Update())return Type->DrawFrame(FrameIdx);
    else return false;
}
bool ORAnim::DrawAt(ImDrawList& List, ImVec2 Pos)
{
    if (Update())return Type->DrawFrameAt(FrameIdx, List, Pos);
    else return false;
}
void ORAnim::Play()
{
    if (Type)
    {
        Playing = true;
        DrawRate.Reset(Type->Rate());
        FrameIdx = Type->GetFirstFrame();
    }
}
void ORAnim::Reset(const ORResPtr<ORAnimType>& ptr)
{
    Type = ptr;
    Playing = false;
}
void ORAnim::Stop()
{
    Playing = false;
}
ORAnim::ORAnim(ORAnim&& r)
{
    std::swap(r.Type, Type);
    std::swap(r.Playing, Playing);
    std::swap(r.DrawRate, DrawRate);
}
ORAnim::ORAnim(const ORResPtr<ORAnimType>& ptr) :Type(ptr), Playing(false)
{

}
