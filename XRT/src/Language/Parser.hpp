#pragma once
#include "_ParserTypeBase.hpp"
#include "Lexer.hpp"

namespace XRT {

    class Parser : public _ParserTypeBase {
    public:
        Parser();
        ~Parser();

        void Parse(Lexer* lex);

        const TokenStorage& GetTokens() const {
            return m_Tokens;
        }

    private:
        TokenStorage m_Tokens;
    };

} // namespace XRT