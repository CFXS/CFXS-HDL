#include "StringUtils.hpp"
#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <locale>
    #include <codecvt>
#endif

std::string StringUtils::utf16_to_utf8(std::wstring_view wstrv) {
    std::wstring wstr{wstrv};
    std::string retStr;
#ifdef _WIN32
    if (!wstr.empty()) {
        int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, NULL, 0, NULL, NULL);

        if (sizeRequired > 0) {
            std::vector<char> utf8String(sizeRequired);
            int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, &utf8String[0], (int)utf8String.size(), NULL, NULL);
            if (bytesConverted != 0) {
                retStr = &utf8String[0];
            } else {
                throw std::runtime_error("utf16 to utf8 conversion failed");
            }
        }
    }
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    retStr = converter.to_bytes(wstr);
#endif
    return retStr;
}

std::wstring StringUtils::utf8_to_utf16(std::string_view utf8) {
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size()) {
        unsigned long uni;
        size_t todo;
        unsigned char ch = static_cast<unsigned char>(utf8[i++]);
        if (ch <= 0x7F) {
            uni  = ch;
            todo = 0;
        } else if (ch <= 0xBF) {
            throw std::logic_error("not a UTF-8 string");
        } else if (ch <= 0xDF) {
            uni  = ch & 0x1F;
            todo = 1;
        } else if (ch <= 0xEF) {
            uni  = ch & 0x0F;
            todo = 2;
        } else if (ch <= 0xF7) {
            uni  = ch & 0x07;
            todo = 3;
        } else {
            throw std::logic_error("not a UTF-8 string");
        }
        for (size_t j = 0; j < todo; ++j) {
            if (i == utf8.size())
                throw std::logic_error("not a UTF-8 string");
            unsigned char ch2 = static_cast<unsigned char>(utf8[i++]);
            if (ch2 < 0x80 || ch2 > 0xBF)
                throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch2 & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t j = 0; j < unicode.size(); ++j) {
        unsigned long uni = unicode[j];
        if (uni <= 0xFFFF) {
            utf16 += static_cast<wchar_t>(uni);
        } else {
            uni -= 0x10000;
            utf16 += static_cast<wchar_t>(((uni >> 10) + 0xD800));
            utf16 += static_cast<wchar_t>(((uni & 0x3FF) + 0xDC00));
        }
    }
    return utf16;
}