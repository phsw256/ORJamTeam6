#pragma once
#include <exception>
#include <string>
#include <string_view>
#include "Minimal.h"

class ORException : public std::exception
{
    std::string Info;
public:
    ORException(const _UTF8 std::string_view info);
    const _UTF8 char* what() const noexcept override;
};
