#include "ORFile.h"

ORReadStraw::~ORReadStraw()
{
    Unlink();
}
size_t ORReadStraw::Get(void* Buf, size_t Size)
{
    if (From)return From->Get(Buf, Size);
    else return 0;
}
bool ORReadStraw::Available() const
{
    if (From)return From->Available();
    else return false;
}
void ORReadStraw::Link(ORReadStraw* from)
{
    Unlink();
    if (from)
    {
        From = from;
        To = from->To;
        if (from->To)from->To->From = this;
        from->To = this;
    }
}
void ORReadStraw::Unlink()
{
    if (To)To->From = From;
    if (From)From->To = To;
    From = To = nullptr;
}
ORReadStraw::ORReadStraw(ORReadStraw&& rhs) : From(rhs.From), To(rhs.To)
{
    if (rhs.From)rhs.From->To = this;
    if (rhs.To)rhs.To->From = this;
    rhs.From = rhs.To = nullptr;
}

BytePointerArray ORStaticStraw::ReadWhole(size_t ReservedBytes)
{
    if (!Available())return BytePointerArray{ 0,nullptr };
    BytePointerArray Result;
    Result.Alloc(GetSize() + ReservedBytes);
    Result.N = Get(Result.Data, Result.N) + ReservedBytes;
    return Result;
}

ORExtFileStraw::~ORExtFileStraw()
{
    Close();
}
size_t ORExtFileStraw::Get(void* Buf, size_t Size)
{
    if (!Available())return 0;
    return File.Read(Buf, 1, Size);
}
bool ORExtFileStraw::Available() const
{
    return File.Available();
}
bool ORExtFileStraw::Open(const std::string_view pFileName)
{
    Close();
    File.Open(pFileName.data(), "rb");
    OwningFile = Available();
    return Available();
}
void ORExtFileStraw::Close()
{
    if (OwningFile)File.Close();
    else File.Release();
    OwningFile = false;
}
size_t ORExtFileStraw::GetSize()
{
    return File.GetSize();
}
void ORExtFileStraw::Connect(const ExtFileClass& fp)
{
    Close();
    File = fp;
    OwningFile = false;
}
void ORExtFileStraw::Connect(ExtFileClass&& fp)
{
    Close();
    File = std::move(fp);
    OwningFile = true;
}
ORExtFileStraw::ORExtFileStraw(ORExtFileStraw&& rhs) :ORStaticStraw(std::move(rhs)), File(std::move(rhs.File)), OwningFile(rhs.OwningFile)
{
    rhs.File.Release();
    rhs.OwningFile = false;
}
ORExtFileStraw::ORExtFileStraw(const std::string_view pFileName)
{
    Open(pFileName);
}
ORExtFileStraw::ORExtFileStraw(const ExtFileClass& fp): ORExtFileStraw()
{
    Connect(fp);
}
ORExtFileStraw::ORExtFileStraw(ExtFileClass&& fp) : ORExtFileStraw()
{
    Connect(std::move(fp));
}

std::vector<std::string_view> GetLinesView(BytePointerArray Text, size_t ExtBytes)
{
    std::vector<std::string_view> Res;
    auto Data = (char*)Text.Data;
    memset(Data + Text.N - ExtBytes, 0, ExtBytes);
    for (size_t i = 0; i < Text.N; ++i)
    {
        if (Data[i] == '\r' || Data[i] == '\n')Data[i] = 0;
    }
    for (size_t i = 0; i < Text.N;)
    {
        auto Dt = Data + i;
        i++;
        if (!*Dt)continue;
        Res.push_back(Dt);
        i += strlen(Dt);
    }
    return Res;
}

std::vector<std::string> GetLines(BytePointerArray Text, size_t ExtBytes)
{
    std::vector<std::string> Res;
    auto Data = (char*)Text.Data;
    memset(Data + Text.N - ExtBytes, 0, ExtBytes);
    for (size_t i = 0; i < Text.N; ++i)
    {
        if (Data[i] == '\r' || Data[i] == '\n')Data[i] = 0;
    }
    for (size_t i = 0; i < Text.N;)
    {
        auto Dt = Data + i;
        i++;
        if (!*Dt)continue;
        Res.push_back(Dt);
        i += strlen(Dt);
    }
    return Res;
}

std::vector<std::string> GetLines(ORStaticStraw& Straw)
{
    BytePointerArray Arr = Straw.ReadWhole(16);
    auto Ret = GetLines(Arr, 16);
    Arr.Delete();
    return Ret; 
}
