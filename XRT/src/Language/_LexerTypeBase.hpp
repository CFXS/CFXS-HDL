#pragma once
#include <exception>
#include <string_view>
#include <StringUtils.hpp>

namespace XRT {

    struct _LexerTypeBase {
        enum class TokenType {
            UNKNOWN,
            SPACE,
            KEYWORD,
            IDENTIFIER,
            STRING_LITERAL,
            LITERAL,
            OPERATOR,
            PUNCTUATOR,
            COMMENT,
        };

        struct Token {
            Token(TokenType _type, std::wstring_view _value, size_t _offset, size_t _line, size_t _column) :
                type(_type), value(_value), offset(_offset), line(_line), column(_column) {
            }
            TokenType type;
            std::wstring_view value;
            size_t offset;
            size_t line;
            size_t column;
        };

        inline std::string TokenTypeToString(TokenType t) {
            switch (t) {
                case TokenType::KEYWORD: return "KEYWORD";
                case TokenType::IDENTIFIER: return "IDENTIFIER";
                case TokenType::LITERAL: return "LITERAL";
                case TokenType::OPERATOR: return "OPERATOR";
                case TokenType::PUNCTUATOR: return "PUNCTUATOR";
                case TokenType::COMMENT: return "COMMENT";
                case TokenType::SPACE: return "SPACE";
                case TokenType::STRING_LITERAL: return "STRING_LITERAL";
                default: return "???";
            }
        }

        using TokenStorage = std::vector<Token*>;

        class UnknownTokenException : public std::exception {
        public:
            UnknownTokenException(std::wstring_view source) : m_Source(source) {
            }

            const char* what() const override {
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