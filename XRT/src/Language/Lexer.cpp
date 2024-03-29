#include "Lexer.hpp"
#include <memory>
#include <string_view>
#include "Language/Lexer.hpp"
#include "Language/Token.hpp"
#include "Language/_LexerTypeBase.hpp"
#include "Log/Logger.hpp"
#include "ScopeExecTime.hpp"
#include "StringUtils.hpp"
#include <Log/ANSI.hpp>
#include <regex/ctre.hpp>

#define REGEX_WHITESPACE        "[ \t\r\n]+"
#define REGEX_MULTILINE_COMMENT "\\/\\*[^*]*\\*+(?:[^\\/*][^*]*\\*+)*\\/"
#define REGEX_COMMENT           "\\/\\/.*[\r\n]?"
#define REGEX_PUNCTUATOR        "[!%\\^&\\*\\(\\)\\-\\+={}\\|~\\[\\];:<>\?,\\.#\\/\\\\]"
#define REGEX_IDENTIFIER        "[a-zA-Z_][a-zA-Z_0-9]*"
#define REGEX_NUMBER_LITERAL    "[0-9][a-fA-FxX_0-9]*"
#define REGEX_STRING            "\"([^\"\\\\]|\\\\.)*\"" // TODO: Modify regex to not match broken multiline strings
#define REGEX_KEYWORD \
    "auto|namespace|component|abstract|registers|implementation|extern|static_assert|in|out|inout|if|else|for|template|typename|using|range|length"

// do not add whitespace to token vector
#define CREATE_TOKEN(type, value)                                           \
    if (type != TokenType::SPACE) {                                         \
        m_Tokens.emplace_back(new Token(type, value, currentOffset, 0, 0)); \
    }

// regex match type/regex and create token
#define CHECK_MATCH(type, rx)                                                                                                \
    for (auto match : ctre::multiline_tokenize<rx>(sourceContent)) {                                                         \
        if (match) {                                                                                                         \
            found          = true;                                                                                           \
            auto matchView = match.to_view();                                                                                \
            if constexpr (type == TokenType::COMMENT) {                                                                      \
                if ((*matchView.rbegin() == '\n' && *(matchView.rbegin() + 1) == '\r') ||                                    \
                    (*matchView.rbegin() == '\r' && *(matchView.rbegin() + 1) == '\n')) {                                    \
                    matchView = std::wstring_view{matchView.data(), matchView.size() - 2};                                   \
                } else if (*matchView.rbegin() == '\n') {                                                                    \
                    matchView = std::wstring_view{matchView.data(), matchView.size() - 1};                                   \
                }                                                                                                            \
            }                                                                                                                \
            if constexpr (type == TokenType::IDENTIFIER) {                                                                   \
                if (ctre::match<REGEX_KEYWORD>(matchView)) {                                                                 \
                    CREATE_TOKEN(TokenType::KEYWORD, matchView);                                                             \
                } else {                                                                                                     \
                    CREATE_TOKEN(TokenType::IDENTIFIER, matchView);                                                          \
                }                                                                                                            \
            } else {                                                                                                         \
                CREATE_TOKEN(type, matchView);                                                                               \
            }                                                                                                                \
            currentOffset += matchView.size();                                                                               \
            sourceContent = std::wstring_view{matchView.data() + matchView.size(), sourceContent.size() - matchView.size()}; \
        }                                                                                                                    \
    }

namespace XRT {

    Lexer::Lexer(std::shared_ptr<SourceEntry> source) : m_Source(source) {
        m_Tokens.reserve(512);
    }

    void Lexer::ProcessSource() {
        if (m_Tokens.size()) {
            LOG_WARN("Lexer source already processed \"{}\"", m_Source->GetPath());
            return;
        }

        ScopeExecTime xt("Lexer::ProcessSource");

        auto sourceContent   = GetSource()->GetContent();
        size_t currentOffset = 0;

        while (1 < 2) {
            bool found = false;

            CHECK_MATCH(TokenType::SPACE, REGEX_WHITESPACE)
            CHECK_MATCH(TokenType::IDENTIFIER, REGEX_IDENTIFIER)
            if (sourceContent.data()[0] == '/') {
                CHECK_MATCH(TokenType::COMMENT, REGEX_MULTILINE_COMMENT)
                CHECK_MATCH(TokenType::COMMENT, REGEX_COMMENT)
            }
            CHECK_MATCH(TokenType::PUNCTUATOR, REGEX_PUNCTUATOR)
            CHECK_MATCH(TokenType::STRING_LITERAL, REGEX_STRING)
            CHECK_MATCH(TokenType::LITERAL, REGEX_NUMBER_LITERAL)

            if (sourceContent.size() == 0) {
                break;
            }

            if (!found) {
                LOG_ERROR("Unknown token:\n'{}'", StringUtils::utf16_to_utf8(std::wstring(sourceContent)));
                throw UnknownTokenException(sourceContent);
                break;
            }
        }

        // Calculate lines and columns
        if (!m_Tokens.empty()) {
            auto it_token = m_Tokens.begin();
            auto content  = GetSource()->GetContent();
            size_t line   = 1;
            size_t column = 1;
            for (size_t i = 0; i < content.size(); i++) {
                auto c = content[i];

                if (it_token != m_Tokens.end()) {
                    auto tok = *it_token;
                    if (tok->offset == i) {
                        tok->line   = line;
                        tok->column = column;
                        it_token++;
                    }
                } else {
                    break;
                }

                if (c == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
            }

            // Later lookahead padding
            for (int i = 0; i < 16; i++)
                m_Tokens.emplace_back(new Token(TokenType::END_OF_FILE, {}, content.size(), line, column));
        }
    }

    void Lexer::PrintTokens() {
        if (m_Tokens.empty()) {
            LOG_WARN("No tokens to print :(");
            return;
        }

        for (auto tok : m_Tokens) {
            tok->Print(GetSource()->GetPath());
        }
    }

} // namespace XRT