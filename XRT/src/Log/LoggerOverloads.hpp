#pragma once
#include <filesystem>
#include <string_view>
// #pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
// #pragma warning(pop)
#include <StringUtils.hpp>

template<>
struct fmt::formatter<std::vector<std::string>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const std::vector<std::string>& input, FormatContext& ctx) -> decltype(ctx.out()) {
        std::string str = "";
        for (auto& s : input) {
            str += "    \"" + s + "\",\n";
        }
        str += "}";
        return format_to(ctx.out(), "std::vector<std::string> ({}) {{{}{}", input.size(), input.size() ? "\n" : "", str);
    }
};

template<>
struct fmt::formatter<std::vector<std::filesystem::path>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const std::vector<std::filesystem::path>& input, FormatContext& ctx) -> decltype(ctx.out()) {
        std::string str = "";
        for (auto& s : input) {
            str += "    " + s.string() + ",\n";
        }
        str += "}";
        return format_to(ctx.out(), "std::vector<std::filesystem::path> ({}) {{{}{}", input.size(), input.size() ? "\n" : "", str);
    }
};

template<>
struct fmt::formatter<std::filesystem::path> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const std::filesystem::path& input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "{}", input.string());
    }
};

template<>
struct fmt::formatter<std::wstring> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const std::wstring& input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "{}", StringUtils::utf16_to_utf8(input));
    }
};

template<>
struct fmt::formatter<std::wstring_view> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(std::wstring_view input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "{}", StringUtils::utf16_to_utf8(input));
    }
};
