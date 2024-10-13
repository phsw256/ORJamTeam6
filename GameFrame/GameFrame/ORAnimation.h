#pragma once
#include "ORImage.h"
#include "ORTimer.h"

class ORAnimType
{
public:
    virtual ~ORAnimType(){}
    virtual bool DrawFrameAt(size_t Idx, ImDrawList& List, ImVec2 Pos) = 0;
    virtual bool DrawFrame(size_t Idx) = 0;//Idx从0到GetFrameCount()-1
    virtual size_t GetNextIdx(size_t Idx) const = 0;//返回>=GetFrameCount()的值即为停止播放
    virtual inttime_t Rate() const = 0;
    virtual size_t GetFrameCount() const = 0;
    virtual size_t GetFirstFrame() const = 0;
    virtual ORLoadable_DefineLoader = 0;
};

class ORClipAnimType: public ORAnimType
{
private:
    ORResPtr<ORImage> Image;
    ImVec2 ClipCount;
    size_t FirstFrame;
    std::vector<size_t> Sequence;
    int RateInt;

    void MoveClip(size_t Idx, ImVec2& Min, ImVec2& Max);
public:
    virtual ~ORClipAnimType() {}
    virtual bool DrawFrameAt(size_t Idx, ImDrawList& List, ImVec2 Pos);
    virtual bool DrawFrame(size_t Idx);
    virtual size_t GetNextIdx(size_t Idx) const;
    virtual inttime_t Rate() const;
    virtual size_t GetFrameCount() const;
    virtual size_t GetFirstFrame() const;
    virtual ORLoadable_DefineLoader;
    ORClipAnimType() = default;
};

class ORAnim
{
private:
    ORResPtr<ORAnimType> Type;
    StrictRateClass DrawRate;
    size_t FrameIdx;
    bool Playing;

    bool Update();
public:
    inline bool Available() const { return Type.get() != nullptr; }
    inline size_t CurrentFrame() const { return FrameIdx; }
    inline size_t TotalFrame() const { return Type ? Type->GetFrameCount() : 0u; }
    inline void SetFrame(size_t Idx) { if (Idx < TotalFrame())FrameIdx = Idx; }
    bool Draw();
    bool DrawAt(ImDrawList& List, ImVec2 Pos);
    void Play();
    void Stop();
    void Reset(const ORResPtr<ORAnimType>&);
    inline bool IsPlaying() { return Update(); }
    ORAnim() = delete;
    ORAnim(NoInit) {}
    ORAnim(const ORAnim&) = delete;
    ORAnim(ORAnim&&);
    ORAnim(const ORResPtr<ORAnimType>&);
};
