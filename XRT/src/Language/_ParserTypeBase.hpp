#pragma once
#include <exception>
#include <string_view>
#include <StringUtils.hpp>
#include "Language/SourceEntry.hpp"
#include "Token.hpp"

namespace XRT {

    struct _ParserTypeBase {
        using Token        = XRT::Token;
        using TokenType    = XRT::TokenType;
        using TokenStorage = std::vector<Token*>;

        class ParseException : public std::exception {
        public:
            ParseException() {
            }

            const char* what() const noexcept override {
                return "Parser::ParseException";
            }
        };

        class InvalidPunctuatorSequence : public ParseException {
        public:
            InvalidPunctuatorSequence(Token* token, std::wstring_view value) : m_Token(token) {
                m_Reason = "Invalid operator" + StringUtils::utf16_to_utf8(value);
            }

            const char* what() const noexcept override {
                return m_Reason.c_str();
            }

            size_t GetLine() const {
                return m_Token->line;
            }
            size_t GetColumn() const {
                return m_Token->column;
            }

        private:
            Token* m_Token;
            std::string m_Reason;
        };
    };

} // namespace XRT