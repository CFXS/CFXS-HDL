#include "Parser.hpp"
#include <string_view>
#include "Language/Token.hpp"
#include "ScopeExecTime.hpp"

#ifdef DEBUG
    #define PARSER_LOG_TRACE(...) LOG_TRACE(__VA_ARGS__)
#else
    #define PARSER_LOG_TRACE(...)
#endif

namespace XRT {

    Parser::Parser() {
    }

    Parser::~Parser() {
    }

    //////////////////////////////////////////////////////////////////////////////////////
    struct OpSequenceMatchEntry {
        std::wstring_view value;
        TokenType resulting_token;
    };
    static constexpr auto MAX_OP_SEQUENCE_LENGTH                         = 5;
    static std::vector<OpSequenceMatchEntry> s_OperatorSequenceMatchInfo = {
        {L"<..<=", TokenType::OP_ASSIGN_LSL_RESIZE},
        {L">..>=", TokenType::OP_ASSIGN_LSR_RESIZE},
        {L"<..<", TokenType::OP_LSL_RESIZE},
        {L">..>", TokenType::OP_LSR_RESIZE},
        {L"<<<=", TokenType::OP_ASSIGN_ROL},
        {L">>>=", TokenType::OP_ASSIGN_ROR},
        {L"<<<", TokenType::OP_ROL},
        {L">>>", TokenType::OP_ROR},
        {L"<<=", TokenType::OP_ASSIGN_LSL},
        {L">>=", TokenType::OP_ASSIGN_LSR},
        {L"&&", TokenType::OP_BOOL_AND},
        {L"||", TokenType::OP_BOOL_OR},
        {L"::", TokenType::OP_RESOLVE},
        {L"+=", TokenType::OP_ASSIGN_ADD},
        {L"-=", TokenType::OP_ASSIGN_SUB},
        {L"*=", TokenType::OP_ASSIGN_MUL},
        {L"/=", TokenType::OP_ASSIGN_DIV},
        {L"&=", TokenType::OP_ASSIGN_AND},
        {L"|=", TokenType::OP_ASSIGN_OR},
        {L"^=", TokenType::OP_ASSIGN_XOR},
        {L"..", TokenType::OP_RANGE},
        {L"++", TokenType::OP_INC},
        {L"--", TokenType::OP_DEC},
        {L"==", TokenType::OP_EQUAL},
        {L"!=", TokenType::OP_NOT_EQUAL},
        {L">=", TokenType::OP_GTEQ},
        {L"<=", TokenType::OP_LTEQ},
        {L"<<", TokenType::OP_LSL},
        {L">>", TokenType::OP_LSR},
    };
    //////////////////////////////////////////////////////////////////////////////////////

#define LONG_SEQUENCE_CHECK(n)                            \
    for (auto& e : s_OperatorSequenceMatchInfo_LEN_##n) { \
        if (extended_value == e.value) {                  \
            tok->type  = e.resulting_token;               \
            tok->value = extended_value;                  \
            specPassTokens.push_back(tok);                \
            break;                                        \
        }                                                 \
    }                                                     \
    new InvalidPunctuatorSequence(tok, extended_value)

    void Parser::Parse(Lexer* lex) {
        ScopeExecTime xt("Parser::Parse");

        PARSER_LOG_TRACE("Parse \"{}\"", lex->GetSource()->GetPath());
        if (lex->GetTokens().empty()) {
            LOG_WARN("No tokens to parse");
        }

        auto& tokens        = lex->GetTokens();
        auto specPassTokens = TokenStorage{};
        specPassTokens.reserve(tokens.size());

        PARSER_LOG_TRACE(" - Punctuator specification pass");
        for (int i = 0; i < (int)tokens.size(); i++) {
            auto tok = tokens[i];
            if (tok->type == TokenType::PUNCTUATOR) {
                std::wstring_view extended_value{tok->value.data(), MAX_OP_SEQUENCE_LENGTH};
                bool added = false;
                for (auto& e : s_OperatorSequenceMatchInfo) {
                    bool good = tok->type == TokenType::PUNCTUATOR && extended_value[0] == e.value[0];
                    if (good) {
                        for (size_t x = 1; x < e.value.size(); x++) {
                            if (!(tokens[i + x]->type == TokenType::PUNCTUATOR && extended_value[x] == e.value[x])) {
                                good = false;
                            }
                        }
                    }
                    if (good) {
                        tok->value = std::wstring_view{extended_value.data(), e.value.size()};
                        tok->type  = e.resulting_token;
                        specPassTokens.push_back(tok);
                        i += (int)e.value.size() - 1;
                        added = true;
                        break;
                    }
                }

                if (!added) {
#define MATCH_CHAR_CASE(ch, t) \
    case ch: {                 \
        tok->type = t;         \
        break;                 \
    }

                    switch ((char)tok->value.data()[0]) {
                        MATCH_CHAR_CASE('<', TokenType::OPEN_ANGLE)
                        MATCH_CHAR_CASE('>', TokenType::CLOSE_ANGLE)
                        MATCH_CHAR_CASE('[', TokenType::OPEN_BRACKET)
                        MATCH_CHAR_CASE(']', TokenType::CLOSE_BRACKET)
                        MATCH_CHAR_CASE('(', TokenType::OPEN_PAREN)
                        MATCH_CHAR_CASE(')', TokenType::CLOSE_PAREN)
                        MATCH_CHAR_CASE('{', TokenType::OPEN_SCOPE)
                        MATCH_CHAR_CASE('}', TokenType::CLOSE_SCOPE)
                        MATCH_CHAR_CASE('!', TokenType::OP_NOT)
                        MATCH_CHAR_CASE('+', TokenType::OP_ADD)
                        MATCH_CHAR_CASE('-', TokenType::OP_SUB)
                        MATCH_CHAR_CASE('*', TokenType::OP_MUL)
                        MATCH_CHAR_CASE('/', TokenType::OP_DIV)
                        MATCH_CHAR_CASE('^', TokenType::OP_XOR)
                        MATCH_CHAR_CASE('&', TokenType::OP_AND)
                        MATCH_CHAR_CASE('|', TokenType::OP_OR)
                        default: {
                            new InvalidPunctuatorSequence(tok, tok->value);
                        }
                    }
                    specPassTokens.push_back(tok);
                }
            } else {
                specPassTokens.push_back(tok);
                if (tok->type == TokenType::END_OF_FILE) {
                    break;
                }
            }
        }

        m_Tokens = specPassTokens;
    }

} // namespace XRT