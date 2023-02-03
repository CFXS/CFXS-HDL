#pragma once

#include <memory>
#include "Language/SourceEntry.hpp"

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
    };

    class Lexer : public _LexerTypeBase {
    public:
        Lexer(std::shared_ptr<SourceEntry> source);
        ~Lexer() = default;

        std::shared_ptr<SourceEntry> GetSource() {
            return m_Source;
        }

        TokenStorage& GetTokens() {
            return m_Tokens;
        }

        const TokenStorage& GetTokens() const {
            return m_Tokens;
        }

    private:
        void ProcessSource();

    private:
        std::shared_ptr<SourceEntry> m_Source;
        TokenStorage m_Tokens;
    };

} // namespace XRT