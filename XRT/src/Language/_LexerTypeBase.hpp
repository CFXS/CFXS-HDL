#pragma once
#include <exception>
#include <string_view>
#include <StringUtils.hpp>
#include "Token.hpp"

namespace XRT {

    struct _LexerTypeBase {
        using Token        = XRT::Token;
        using TokenType    = XRT::TokenType;
        using TokenStorage = std::vector<Token*>;

        class UnknownTokenException : public std::exception {
        public:
            UnknownTokenException(std::wstring_view source) : m_Source(source) {
            }

            const char* what() const noexcept override {
                return "Lexer::UnknownTokenException";
            }

            std::string GetSource() const {
                return StringUtils::utf16_to_utf8(m_Source);
            }

        private:
            std::wstring_view m_Source;
        };
    };

} // namespace XRT