#pragma once

#include <cstdio>
#include "..\Minimal.h"

class ExtFileClass
{
    FILE* fp{ nullptr };
	bool IsOpen{ false };
public:
    ExtFileClass() = default;
    ExtFileClass(const ExtFileClass&) = default;
    ExtFileClass(ExtFileClass&& rhs) : fp(rhs.fp), IsOpen(rhs.IsOpen)
    {
        rhs.Release();
    }
    ExtFileClass& operator=(const ExtFileClass&) = default;
    ExtFileClass& operator=(ExtFileClass&& rhs)
    {
        fp = rhs.fp;
        IsOpen = rhs.IsOpen;
        rhs.Release();
    }
	FILE* GetPlain() const
	{
        if (!IsOpen)return nullptr;
        return fp;
	}
    FILE* Release()
    {
        IsOpen = false;
        return fp;
    }
	bool Open(const char* path, const char* mode, void* Reserved = nullptr)
	{
		fp = fopen(path, mode);
		return IsOpen = (fp != NULL);
	}
	size_t Read(void* Buf, size_t Size, size_t Count)
	{
		if (!IsOpen)return 0;
		return fread(Buf, Size, Count, fp);
	}
	size_t Write(const void* Buf, size_t Size, size_t Count)
	{
		if (!IsOpen)return 0;
		return fwrite(Buf, Size, Count, fp);
	}
	char GetChr()
	{
		if (!IsOpen)return 0;
		return fgetc(fp);
	}
	bool PutChr(const char Byte)
	{
		if (!IsOpen)return 0;
		return fputc(Byte, fp)!=EOF;
	}
	bool Ln()
	{
		return PutChr('\n');
	}
	char* GetStr(char* Buf, size_t Size)
	{
		if (!IsOpen)return 0;
		return fgets(Buf, Size, fp);
	}
	size_t PutStr(const char* Buf)
	{
		if (!IsOpen)return 0;
		return fputs(Buf, fp);
	}
	int Close()
	{
		if (!IsOpen)return 0;
		IsOpen = false;
		return fclose(fp);
	}
	int Seek(int Offset,int Base)
	{
		return fseek(fp,Offset,Base);
	}
	int Position() const
	{
		return ftell(fp);
	}
	bool Eof() const
	{
		if (!IsOpen)return true;
		return feof(fp);
	}
	bool Available() const
	{
		return IsOpen;
	}
	~ExtFileClass()
	{
		if (IsOpen)Close();
	}
    size_t GetSize()
    {
        if (!IsOpen)return 0;
        int Cur = Position();
        Seek(0, SEEK_END);
        int Res = Position();
        Seek(Cur, SEEK_SET);
        return Res;
    }
};

