#pragma once
#include <cstdint>
#define _UTF8
typedef unsigned       hash_t;
const int MAX_LINE_LENGTH = 5000;
const int MAX_STRING_LENGTH = 5000;
typedef char BufString[MAX_STRING_LENGTH];
typedef int64_t inttime_t;
typedef int64_t counter_t;

template<typename T>
struct PointerArray
{
    size_t N;
    T* Data;
    void Delete() { N = 0; delete[]Data; }
    void Alloc(size_t n) { if (N)Delete(); N = n; Data = new T[n]; }
};

using BytePointerArray = PointerArray<unsigned char>;
