#pragma once
#include "_ParserTypeBase.hpp"
#include "Lexer.hpp"
#include <Utils.hpp>
#include "AST.hpp"

namespace XRT {

    class Parser : public _ParserTypeBase {
        using LexData = const Scope<Lexer>&;

    public:
        Parser();
        ~Parser();

        void Parse(LexData lex);

        const TokenStorage& GetTokens() const {
            return m_Tokens;
        }

        void PrintAST() const {
            m_AST->Print();
        }

    private:
        /// Create operator specific tokens from regular base token sequences
        TokenStorage PreProcessOperators(LexData lex);

    private:
        TokenStorage m_Tokens;
        Ref<AST> m_AST;
    };

} // namespace XRT