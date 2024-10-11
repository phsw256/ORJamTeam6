#pragma once
#include <exception>
#include <string>
#include <string_view>
#include "Minimal.h"

class ORException : public std::exception
{
    std::string Info;
public:
    explicit ORException(_UTF8 std::string&& info);
    explicit ORException(const _UTF8 std::string_view info);
    explicit ORException(const _UTF8 char* info);
    const _UTF8 char* what() const noexcept override;
};
