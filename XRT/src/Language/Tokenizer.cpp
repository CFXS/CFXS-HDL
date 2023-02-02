#include "Tokenizer.hpp"
#include <Log/Logger.hpp>
#include <map>
#include <vector>
#include <algorithm>
#include <StringUtils.hpp>
#include <regex/ctre.hpp>
#include <ScopeExecTime.hpp>

#define REGEX_LITERAL         L"^[a-zA-Z_$][a-zA-Z_$0-9]*$"
#define REGEX_HEX_LITERAL     L"^0x[0-9a-fA-FxX_']+$"
#define REGEX_BINARY_LITERAL  L"^0b[01xX_']+$"
#define REGEX_DECIMAL_LITERAL L"^[0-9_']+$"
#define REGEX_RANGE_LITERAL   L"^[0-9]+\\.\\.[0-9]+$"
#define REGEX_PUNCTUATOR      L"[!%\\^&\\*\\(\\)\\-\\+={}\\|~\\[\\];:\"<>\?,\\.#\\/\\\\]"
#define REGEX_WHITESPACE      L"\\s"

namespace XRT {

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::vector<std::wstring_view> s_Identifiers = {
        L"namespace",
        L"component",
        L"ports",
        L"registers",
        L"implementation",
        L"extern",
        L"in",
        L"out",
        L"inout",
        L"if",
        L"else",
        L"for",
        L"template",
        L"typename",
        L"include",
        L"using",
        L"constexpr",
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static bool IsKeyword(const std::wstring_view& str) {
        return std::find(s_Identifiers.begin(), s_Identifiers.end(), str) != s_Identifiers.end();
    }

    void Tokenizer::Tokenize(const std::string& _source, std::filesystem::path* filePath) {
        ScopeExecTime xt("Tokenize");

        std::wstring source = StringUtils::utf8_to_utf16(_source + " ");
        source.erase(std::remove(source.begin(), source.end(), (wchar_t)'\r'), source.end());

        std::vector<Token*> tokens;
        tokens.reserve(128);
        std::wstring currentValue;
        currentValue.reserve(128);

        bool tokenActive        = false;
        size_t tokenStartIndex  = 0;
        size_t tokenStartLine   = 0;
        size_t tokenStartColumn = 0;
        bool commentActive      = false;
        bool commentIsMultiline = false;
        bool stringActive       = false;
        size_t tokenPos         = 0;
        size_t lineNumber       = 1;
        size_t lineOffset       = 0;
        size_t lastLinePos      = 0;

        if (!ctre::starts_with<REGEX_WHITESPACE>(source.begin(), source.begin() + 1)) {
            tokenActive      = true;
            tokenStartIndex  = 0;
            tokenStartLine   = 1;
            tokenStartColumn = 0;
            lineOffset       = 1;
            tokenPos         = 1;
        }

        // Source is padded with 1 space at the end
        auto sourceLength              = source.length();
        std::wstring::iterator it_char = source.begin();
        for (auto i = 0; i < sourceLength - 1; i++) {
            auto c = *it_char;

            tokenPos++;
            if (c == '\n') {
                lastLinePos = lineOffset;
                lineOffset  = 1;
                lineNumber++;
            } else {
                lineOffset++;
            }

            if (tokenActive) {
                if (!commentActive & !stringActive) {
                    auto nextChar         = *(it_char + 1);
                    auto is_comment_start = (c == '/' && (nextChar == '/' || nextChar == '*'));
                    auto is_string_start  = c == '"';

                    if (is_comment_start) {
                        commentIsMultiline = nextChar == '*';
                        commentActive      = true;
                        tokenStartIndex    = tokenPos - 1;
                        tokenStartLine     = lineNumber;
                        tokenStartColumn   = lineOffset - 1;
                    } else if (is_string_start) {
                        stringActive     = true;
                        currentValue     = c;
                        tokenStartIndex  = tokenPos - 1;
                        tokenStartLine   = lineNumber;
                        tokenStartColumn = lineOffset - 1;
                        it_char++;
                        continue;
                    }
                }

                if (stringActive) {
                    auto is_string_end = c == '"';
                    currentValue += c;
                    if (is_string_end) {
                        tokens.push_back(new Token(
                            currentValue, TokenType::LITERAL, tokenPos - tokenStartIndex - 1, tokenStartLine, tokenStartColumn, filePath));
                        currentValue.clear();
                        tokenActive  = false;
                        stringActive = false;
                    }
                } else {
                    if (ctre::starts_with<REGEX_WHITESPACE>(it_char, it_char + 1) && !commentActive) {
                        tokens.push_back(new Token(currentValue,
                                                   IsKeyword(currentValue) ? TokenType::KEYWORD : TokenType::IDENTIFIER,
                                                   tokenPos - tokenStartIndex - 1,
                                                   tokenStartLine,
                                                   tokenStartColumn,
                                                   filePath));
                        tokenActive = false;
                        currentValue.clear();
                    } else if (ctre::starts_with<REGEX_PUNCTUATOR>(it_char, it_char + 1) && !commentActive) {
                        if (!currentValue.empty()) {
                            tokens.push_back(new Token(currentValue,
                                                       IsKeyword(currentValue) ? TokenType::KEYWORD : TokenType::IDENTIFIER,
                                                       tokenPos - (tokenStartIndex - 1),
                                                       tokenStartLine,
                                                       tokenStartColumn - 1,
                                                       filePath));
                            std::wstring cc;
                            cc = c;
                            tokens.push_back(new Token(cc, TokenType::PUNCTUATOR, 1, lineNumber, lineOffset - 1, filePath));
                            currentValue.clear();
                        } else {
                            std::wstring cc;
                            cc = c;
                            tokens.push_back(new Token(cc, TokenType::PUNCTUATOR, 1, lineNumber, lineOffset - 1, filePath));
                        }
                        tokenActive = false;
                    } else {
                        auto is_multiline_end = commentActive && (c == '/' && (i ? *(it_char - 1) : (wchar_t)'\0') == '*');
                        if (commentActive &&
                            ((!commentIsMultiline && c == '\n') || (commentActive && commentIsMultiline && is_multiline_end))) {
                            commentActive = false;
                            tokenActive   = false;
                            tokens.push_back(new Token(commentIsMultiline ? currentValue + L"/" : currentValue,
                                                       TokenType::COMMENT,
                                                       tokenPos - tokenStartIndex - 1,
                                                       tokenStartLine,
                                                       tokenStartColumn,
                                                       filePath));
                            currentValue.clear();
                        } else {
                            currentValue += c;
                        }
                    }
                }
            } else {
                if (!ctre::starts_with<REGEX_WHITESPACE>(it_char, it_char + 1)) {
                    tokenActive      = true;
                    tokenStartIndex  = tokenPos;
                    tokenStartLine   = lineNumber;
                    tokenStartColumn = lineOffset;

                    i--;
                    it_char--;
                    tokenPos--;
                    lineOffset--;
                    if (c == '\n') {
                        lineNumber--;
                    }
                }
            }

            it_char++;
        }
    }

} // namespace XRT