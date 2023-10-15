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
        // TODO: come up with better crackhead syntax for this
        // {L"<..<=", TokenType::ASSIGN_LSL_RESIZE},
        // {L">..>=", TokenType::ASSIGN_LSR_RESIZE},
        // {L"<..<", TokenType::LSL_RESIZE},
        // {L">..>", TokenType::LSR_RESIZE},
        {L"<<<=", TokenType::ASSIGN_ROL}, ////
        {L">>>=", TokenType::ASSIGN_ROR}, //
        {L"<<<", TokenType::ROL},         //
        {L">>>", TokenType::ROR},         //
        {L"<<=", TokenType::ASSIGN_LSL},  //
        {L">>=", TokenType::ASSIGN_LSR},  //
        {L"&&", TokenType::BOOL_AND},     //
        {L"||", TokenType::BOOL_OR},      //
        {L"::", TokenType::RESOLVE},      //
        {L"+=", TokenType::ASSIGN_ADD},   //
        {L"-=", TokenType::ASSIGN_SUB},   //
        {L"*=", TokenType::ASSIGN_MUL},   //
        {L"/=", TokenType::ASSIGN_DIV},   //
        {L"&=", TokenType::ASSIGN_AND},   //
        {L"|=", TokenType::ASSIGN_OR},    //
        {L"^=", TokenType::ASSIGN_XOR},   //
        {L"..", TokenType::RANGE},        //
        {L"++", TokenType::INC},          //
        {L"--", TokenType::DEC},          //
        {L"==", TokenType::EQUAL},        //
        {L"!=", TokenType::NOT_EQUAL},    //
        {L">=", TokenType::GTEQ},         //
        {L"<=", TokenType::LTEQ},         //
        {L"<<", TokenType::LSL},          //
        {L">>", TokenType::LSR},          //
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
        TokenType singlePunctuatorMap[256];
        memset(singlePunctuatorMap, 0, sizeof(singlePunctuatorMap));
        singlePunctuatorMap[static_cast<int>('.')] = TokenType::DOT;
        singlePunctuatorMap[static_cast<int>(';')] = TokenType::SEPARATOR;
        singlePunctuatorMap[static_cast<int>('<')] = TokenType::OPEN_ANGLE;
        singlePunctuatorMap[static_cast<int>('>')] = TokenType::CLOSE_ANGLE;
        singlePunctuatorMap[static_cast<int>('[')] = TokenType::OPEN_BRACKET;
        singlePunctuatorMap[static_cast<int>(']')] = TokenType::CLOSE_BRACKET;
        singlePunctuatorMap[static_cast<int>('(')] = TokenType::OPEN_PAREN;
        singlePunctuatorMap[static_cast<int>(')')] = TokenType::CLOSE_PAREN;
        singlePunctuatorMap[static_cast<int>('{')] = TokenType::OPEN_SCOPE;
        singlePunctuatorMap[static_cast<int>('}')] = TokenType::CLOSE_SCOPE;
        singlePunctuatorMap[static_cast<int>('!')] = TokenType::NOT;
        singlePunctuatorMap[static_cast<int>('+')] = TokenType::ADD;
        singlePunctuatorMap[static_cast<int>('-')] = TokenType::SUB;
        singlePunctuatorMap[static_cast<int>('*')] = TokenType::MUL;
        singlePunctuatorMap[static_cast<int>('/')] = TokenType::DIV;
        singlePunctuatorMap[static_cast<int>('^')] = TokenType::XOR;
        singlePunctuatorMap[static_cast<int>('&')] = TokenType::AND;
        singlePunctuatorMap[static_cast<int>('|')] = TokenType::OR;
        singlePunctuatorMap[static_cast<int>('=')] = TokenType::ASSIGN;
        singlePunctuatorMap[static_cast<int>('?')] = TokenType::TERNARY_IF;
        singlePunctuatorMap[static_cast<int>(':')] = TokenType::TERNARY_ELSE;

        ScopeExecTime xt("Parser::Parse");

        PARSER_LOG_TRACE("Parse \"{}\"", lex->GetSource()->GetPath());
        if (lex->GetTokens().empty()) {
            LOG_WARN("No tokens to parse");
        }

        auto& tokens        = lex->GetTokens();
        auto specPassTokens = TokenStorage{};
        specPassTokens.reserve(tokens.size());

        PARSER_LOG_TRACE(" - Punctuator specification pass");
        for (size_t i = 0; i < tokens.size(); i++) {
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
                        i += e.value.size() - 1;
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

                    tok->type = singlePunctuatorMap[static_cast<uint8_t>(tok->value.data()[0])];

                    if (tok->type == TokenType::UNKNOWN) {
                        new InvalidPunctuatorSequence(tok, tok->value);
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

        // for (auto& tok : m_Tokens) {
        //     tok->Print(lex->GetSource()->GetPath());
        // }
    }

} // namespace XRT