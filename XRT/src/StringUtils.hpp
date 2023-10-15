#pragma once
#include <string>
#include <vector>

class StringUtils {
public:
    static std::string utf16_to_utf8(std::wstring_view wstrv);
    static std::wstring utf8_to_utf16(std::string_view utf8);
};