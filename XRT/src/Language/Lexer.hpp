#pragma once
#include <memory>
#include "_LexerTypeBase.hpp"
#include "Language/SourceEntry.hpp"

namespace XRT {

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

        void ProcessSource();

        void PrintTokens();

    private:
        std::shared_ptr<SourceEntry> m_Source;
        TokenStorage m_Tokens;
    };

} // namespace XRT