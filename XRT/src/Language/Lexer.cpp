#include "Lexer.hpp"
#include <memory>
#include <string_view>
#include "Language/Lexer.hpp"
#include "Log/Logger.hpp"
#include "ScopeExecTime.hpp"
#include "StringUtils.hpp"

#define REGEX_WHITESPACE        "[ \t\r\n]+"
#define REGEX_MULTILINE_COMMENT "\\/\\*[^*]*\\*+(?:[^\\/*][^*]*\\*+)*\\/"
#define REGEX_COMMENT           "\\/\\/.*[\r\n]?"
#define REGEX_PUNCTUATOR        "[!%\\^&\\*\\(\\)\\-\\+={}\\|~\\[\\];:<>\?,\\.#\\/\\\\]"
#define REGEX_IDENTIFIER        "[a-zA-Z_][a-zA-Z_0-9]*"
#define REGEX_NUMBER_LITERAL    "[0-9][a-fA-FxX_0-9]*"
#define REGEX_STRING            "\"([^\"\\\\]|\\\\.)*\"" // TODO: Modify regex to not match broken multiline strings
#define REGEX_KEYWORD \
    "all|namespace|component|abstract|registers|implementation|extern|static_assert|in|out|inout|if|else|for|template|typename|include|using|range|length"

#define CREATE_TOKEN(type, value) m_Tokens.emplace_back(new Token(type, value, 0, 0, 0));
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
            sourceContent = std::wstring_view{matchView.data() + matchView.size(), sourceContent.size() - matchView.size()}; \
            if constexpr (type == TokenType::IDENTIFIER) {                                                                   \
                if (ctre::match<REGEX_KEYWORD>(matchView)) {                                                                 \
                    CREATE_TOKEN(TokenType::KEYWORD, matchView);                                                             \
                } else {                                                                                                     \
                    CREATE_TOKEN(TokenType::IDENTIFIER, matchView);                                                          \
                }                                                                                                            \
            } else {                                                                                                         \
                CREATE_TOKEN(type, matchView);                                                                               \
            }                                                                                                                \
        }                                                                                                                    \
    }

namespace XRT {

    Lexer::Lexer(std::shared_ptr<SourceEntry> source) : m_Source(source) {
        m_Tokens.reserve(1024);
        ProcessSource();
    }

    void Lexer::ProcessSource() {
        ScopeExecTime xt("v2");

        auto sourceContent = GetSource()->GetContent();

        while (1 < 2) {
            bool found = false;

            CHECK_MATCH(TokenType::SPACE, REGEX_WHITESPACE)
            CHECK_MATCH(TokenType::IDENTIFIER, REGEX_IDENTIFIER)
            if (sourceContent.data()[0] == '/') {
                CHECK_MATCH(TokenType::COMMENT, REGEX_MULTILINE_COMMENT)
                CHECK_MATCH(TokenType::COMMENT, REGEX_COMMENT)
            }
            CHECK_MATCH(TokenType::PUNCTUATOR, REGEX_PUNCTUATOR)
            if (sourceContent.data()[0] == '"') {
                CHECK_MATCH(TokenType::STRING_LITERAL, REGEX_STRING)
            }
            CHECK_MATCH(TokenType::LITERAL, REGEX_NUMBER_LITERAL)

            if (sourceContent.size() == 0) {
                break;
            }

            if (!found) {
                LOG_ERROR("Unknown token:\n'{}'", StringUtils::utf16_to_utf8(std::wstring(sourceContent)));
                break;
            }
        }
    }

} // namespace XRT