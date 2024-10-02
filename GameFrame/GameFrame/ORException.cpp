#include "ORException.h"

ORException::ORException(const _UTF8 std::string_view info):Info(info)
{

}
const _UTF8 char* ORException::what() const noexcept
{
    return Info.c_str();
}
