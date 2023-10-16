#include "Parser.hpp"
#include <string_view>
#include "Language/AST.hpp"
#include "Language/Token.hpp"
#include "ScopeExecTime.hpp"

#include "ParserTables.hpp"
#include "StringUtils.hpp"
#include "Utils.hpp"

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
            specPassTokens.emplace_back(tok);             \
            break;                                        \
        }                                                 \
    }                                                     \
    new InvalidPunctuatorSequence(tok)

    Parser::TokenStorage Parser::PreProcessOperators(LexData lex) {
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
                        specPassTokens.emplace_back(tok);
                        i += e.value.size() - 1;
                        added = true;
                        break;
                    }
                }

                if (!added) {
                    tok->type = s_SinglePunctuatorMap[static_cast<uint8_t>(tok->value.data()[0])];

                    if (tok->type == TokenType::UNKNOWN) {
                        new InvalidPunctuatorSequence(tok);
                    }

                    specPassTokens.emplace_back(tok);
                }
            } else {
                specPassTokens.emplace_back(tok);
                if (tok->type == TokenType::END_OF_FILE) {
                    break;
                }
            }
        }

        return specPassTokens;
    }

    void Parser::Parse(LexData lex) {
        using TT = TokenType;

        const auto source_path = lex->GetSource()->GetPath();

        m_AST    = CreateRef<AST>();
        m_Tokens = PreProcessOperators(lex);

        size_t token_index = 0;
        auto current_token = m_Tokens[token_index];

#define LOOKAHEAD(l_idx, expected_type)                                                                                        \
    [&]() constexpr -> const Token* {                                                                                          \
        size_t idx = l_idx;                                                                                                    \
        if ((idx + token_index) >= m_Tokens.size())                                                                            \
            throw ParseOverflow(std::string("Lookahead [+") + std::to_string(idx) + std::string("] overflow"), current_token); \
        const auto tok = m_Tokens[idx + token_index];                                                                          \
        if (expected_type != TokenType::UNKNOWN && tok->type != expected_type)                                                 \
            throw ExpectationError(fmt::format("Expected {} got {}", ToString(expected_type), ToString(tok->type)), tok);      \
        return tok;                                                                                                            \
    }()

        auto FORCE_END = Token(TT::END_OF_FILE, {}, 0, 0, 0);

        while (current_token->type != TT::END_OF_FILE) {
            size_t inc_tokens = 1;

            switch (current_token->type) {
                case TT::COMMENT: break;
                case TT::PREPROCESSOR: {
                    const auto action = LOOKAHEAD(1, TT::IDENTIFIER);
                    inc_tokens++;

                    if (action->value == L"include") {
                        const auto path = LOOKAHEAD(2, TT::STRING_LITERAL);
                        inc_tokens++;

                        m_AST->Append(CreateScope<AST_Element::SourceLink>(path->value));

                    } else if (action->value == L"define") {
                        throw NotImplemented("#define", action);
                    } else {
                        throw InvalidPreprocessorDirective("Unknown preprocessing directive", action);
                    }

                    break;
                }

                case TT::KEYWORD: {
                    if (current_token->value == L"namespace") {
                        const auto first = LOOKAHEAD(1, TT::UNKNOWN);
                        inc_tokens++;
                        if (first->type != TT::IDENTIFIER) {
                            throw ExpectationError(fmt::format("Expected IDENTFIER got {}", ToString(first->type)), first);
                        }

                        std::wstring ns_name{first->value};
                        size_t lookahead_index = 2;
                        bool accept_resolve    = true;

                        while (1 < 2) {
                            const auto next = LOOKAHEAD(lookahead_index++, TT::UNKNOWN);
                            inc_tokens++;
                            if (next->type == TT::OPEN_SCOPE) {
                                lookahead_index--;
                                inc_tokens--;
                                if (accept_resolve == false) {
                                    // namespace ends with "::"
                                    throw ExpectationError(fmt::format("Expected IDENTIFIER got OPEN_SCOPE"), next);
                                }
                                break;
                            }
                            if (accept_resolve && next->type != TT::RESOLVE) {
                                throw ExpectationError(fmt::format("Expected RESOLVE got {}", ToString(next->type)), next);
                            }
                            if (!accept_resolve && next->type != TT::IDENTIFIER) {
                                throw ExpectationError(fmt::format("Expected IDENTIFIER got {}", ToString(next->type)), next);
                            }

                            ns_name += next->value;
                            accept_resolve = !accept_resolve;
                        }

                        LOOKAHEAD(lookahead_index, TT::OPEN_SCOPE);
                        inc_tokens++;

                        m_AST->EnterScope();
                        m_AST->Append(CreateScope<AST_Element::Namespace>(ns_name));

                    } else {
                        throw NotImplemented("keyword [" + StringUtils::utf16_to_utf8(current_token->value) + "]", current_token);
                    }
                    break;
                }

                case TT::CLOSE_SCOPE: {
                    m_AST->ExitScope();
                    break;
                }

                default: {
                    throw UnknownTokenException(current_token);
                    current_token = &FORCE_END;
                }
            }

            token_index += inc_tokens;
            current_token = m_Tokens[token_index];
        }
    }

} // namespace XRT