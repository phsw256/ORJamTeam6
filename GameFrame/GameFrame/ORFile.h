#pragma once
#include <cstdio>
#include <string_view>
#include <string>
#include <vector>
#include "FromEngine\External_File.h"

class ORReadStraw
{
    ORReadStraw* From{ nullptr };
    ORReadStraw* To{ nullptr };
public:
    virtual ~ORReadStraw();
    virtual size_t Get(void* Buf, size_t Size);
    virtual bool Available() const;
    void Link(ORReadStraw* From);
    inline void Link(ORReadStraw& From) { Link(&From); }
    void Unlink();
    ORReadStraw() = default;
    ORReadStraw(const ORReadStraw&) = delete;
    ORReadStraw(ORReadStraw&&);
};

class ORStaticStraw : public ORReadStraw
{
public:
    virtual ~ORStaticStraw() = default;
    virtual bool Open(const std::string_view pFileName) = 0;
    virtual void Close() = 0;
    virtual size_t GetSize() = 0;
    BytePointerArray ReadWhole(size_t ReservedBytes);
    ORStaticStraw() = default;
    ORStaticStraw(const ORStaticStraw&) = delete;
    ORStaticStraw(ORStaticStraw&&) = default;
};

class ORExtFileStraw : public ORStaticStraw
{
    ExtFileClass File;
    bool OwningFile{ false };
public:
    virtual ~ORExtFileStraw();
    virtual size_t Get(void* Buf, size_t Size);
    virtual bool Available() const;
    virtual bool Open(const std::string_view pFileName);
    virtual void Close();
    virtual size_t GetSize();
    void Connect(const ExtFileClass& file);//无所有权
    void Connect(ExtFileClass&& file);//有所有权
    ORExtFileStraw() = default;
    ORExtFileStraw(const ORExtFileStraw&) = delete;
    ORExtFileStraw(ORExtFileStraw&&);
    ORExtFileStraw(const std::string_view pFileName);
    ORExtFileStraw(const ExtFileClass& file);
    ORExtFileStraw(ExtFileClass&& file);
};

std::vector<std::string_view> GetLinesView(BytePointerArray Text, size_t ExtBytes);
std::vector<std::string> GetLines(BytePointerArray Text, size_t ExtBytes);
std::vector<std::string> GetLines(ORStaticStraw& Straw);
