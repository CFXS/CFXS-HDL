#include "Tokenizer.hpp"
#include <Log/Logger.hpp>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <StringUtils.hpp>
#include <ScopeExecTime.hpp>
#include "Language/TokenizerException.hpp"
#include "regex/ctre.hpp"

#define REGEX_IDENTIFIER      L"^[a-zA-Z_$][a-zA-Z_$0-9]*$"
#define REGEX_HEX_LITERAL     L"^0x[0-9a-fA-FxX_']+$"
#define REGEX_BINARY_LITERAL  L"^0b[01xX_']+$"
#define REGEX_DECIMAL_LITERAL L"^[0-9_']+$"
#define REGEX_RANGE_LITERAL   L"^[0-9]+\\.\\.[0-9]+$"
#define REGEX_PUNCTUATOR      L"[!%\\^&\\*\\(\\)\\-\\+={}\\|~\\[\\];:\"<>\?,\\.#\\/\\\\]"
#define REGEX_WHITESPACE      L"\\s"
#define REGEX_KEYWORD \
    L"all|namespace|component|abstract|registers|implementation|extern|static_assert|in|out|inout|if|else|for|template|typename|include|using|constexpr"

namespace XRT {

    static bool IsKeyword(const std::wstring_view& str) {
        return ctre::match<REGEX_KEYWORD>(str.begin(), str.end());
    }

    std::vector<std::unique_ptr<Token>> Tokenizer::Tokenize(const std::string& _source, const std::filesystem::path* filePath) {
        LOG_TRACE("Tokenize {}", filePath->filename());
        ScopeExecTime xt("Tokenize");

        std::wstring source = StringUtils::utf8_to_utf16(_source + " ");
        source.erase(std::remove(source.begin(), source.end(), (wchar_t)'\r'), source.end());

        std::vector<std::unique_ptr<Token>> tokens;
        std::wstring currentValue;
        currentValue.reserve(128);

        bool tokenActive        = false;
        size_t tokenStartIndex  = 0;
        size_t tokenStartLine   = 0;
        size_t tokenStartColumn = 0;
        bool commentActive      = false;
        bool commentIsMultiline = false;
        bool stringActive       = false;
        size_t tokenPos         = 1;
        size_t lineNumber       = 1;
        size_t lineOffset       = 1;

        if (!ctre::match<REGEX_WHITESPACE>(source.begin(), source.begin() + 1)) {
            tokenActive      = true;
            tokenStartIndex  = 1;
            tokenStartLine   = 1;
            tokenStartColumn = 1;
            tokenPos         = 1;
        }

        auto addKeywordIdentifierLiteral = [&](const std::wstring& value, size_t size, size_t line, size_t column) {
            if (StringUtils::utf16_to_utf8(value) == "00") {
                for (auto c : value) {
                    LOG_INFO("- {}", (uint16_t)c);
                }
                for (auto c : std::wstring(L"00")) {
                    LOG_WARN("- {}", (uint16_t)c);
                }
            }

            bool is_keyword = IsKeyword(value);
            if (is_keyword) {
                tokens.emplace_back(new Token(value, TokenType::KEYWORD, size, line, column, filePath));
            } else {
                if (ctre::match<REGEX_IDENTIFIER>(value.begin(), value.end())) {
                    tokens.emplace_back(new Token(value, TokenType::IDENTIFIER, size, line, column, filePath));
                } else {
                    if (ctre::match<REGEX_DECIMAL_LITERAL>(value.begin(), value.end())) {
                        tokens.emplace_back(new Token(value, TokenType::LITERAL, size, line, column, filePath));
                    } else if (ctre::match<REGEX_HEX_LITERAL>(value.begin(), value.end())) {
                        tokens.emplace_back(new Token(value, TokenType::LITERAL, size, line, column, filePath));
                    } else if (ctre::match<REGEX_BINARY_LITERAL>(value.begin(), value.end())) {
                        tokens.emplace_back(new Token(value, TokenType::LITERAL, size, line, column, filePath));
                    } else if (ctre::match<REGEX_RANGE_LITERAL>(value.begin(), value.end())) {
                        tokens.emplace_back(new Token(value, TokenType::LITERAL, size, line, column, filePath));
                    } else {
                        auto token = new Token(value, TokenType::INVALID_LITERAL, size, line, column, filePath);
                        std::stringstream ss;
                        fmt::print(ss, "Invalid literal: \"{}\"\n{}", StringUtils::utf16_to_utf8(value), token->PrintToString());
                        throw TokenizerException(ss.str());
                        tokens.emplace_back(token);
                    }
                }
            }
        };

        // Source is padded with 1 space at the end
        size_t sourceLength = source.length();
        for (size_t i = 0; i < sourceLength; i++) {
            auto c       = source[i];
            auto it_char = source.begin() + i;

            if (tokenActive) {
                if (!commentActive & !stringActive) {
                    auto nextChar         = *(it_char + 1);
                    auto is_comment_start = (c == '/' && (nextChar == '/' || nextChar == '*'));
                    auto is_string_start  = c == '"';

                    if (is_comment_start) {
                        commentIsMultiline = nextChar == '*';
                        commentActive      = true;
                        tokenStartIndex    = tokenPos;
                        tokenStartLine     = lineNumber;
                        tokenStartColumn   = lineOffset;
                    } else if (is_string_start) {
                        stringActive     = true;
                        currentValue     = c;
                        tokenStartIndex  = tokenPos;
                        tokenStartLine   = lineNumber;
                        tokenStartColumn = lineOffset;

                        tokenPos++;
                        if (c == '\n') {
                            lineOffset = 1;
                            lineNumber++;
                        } else {
                            lineOffset++;
                        }
                        continue;
                    }
                }

                if (stringActive) {
                    auto is_string_end = c == '"';
                    currentValue += c;
                    if (is_string_end) {
                        tokens.emplace_back(new Token(
                            currentValue, TokenType::LITERAL, tokenPos + 1 - tokenStartIndex, tokenStartLine, tokenStartColumn, filePath));
                        currentValue.clear();
                        tokenActive  = false;
                        stringActive = false;
                    }
                } else {
                    if (ctre::match<REGEX_WHITESPACE>(it_char, it_char + 1) && !commentActive) {
                        addKeywordIdentifierLiteral(currentValue, tokenPos - tokenStartIndex, tokenStartLine, tokenStartColumn);
                        tokenActive = false;
                        currentValue.clear();
                    } else if (ctre::match<REGEX_PUNCTUATOR>(it_char, it_char + 1) && !commentActive) {
                        if (!currentValue.empty()) {
                            addKeywordIdentifierLiteral(currentValue, tokenPos - tokenStartIndex, tokenStartLine, tokenStartColumn);
                            std::wstring cc;
                            cc = c;
                            tokens.emplace_back(new Token(cc, TokenType::PUNCTUATOR, 1, lineNumber, lineOffset, filePath));
                        } else {
                            std::wstring cc;
                            cc = c;
                            tokens.emplace_back(new Token(cc, TokenType::PUNCTUATOR, 1, lineNumber, lineOffset, filePath));
                        }
                        currentValue.clear();
                        tokenActive = false;
                    } else {
                        auto is_multiline_end = commentActive && (c == '/' && (i ? *(it_char - 1) : (wchar_t)'\0') == '*');
                        if (commentActive &&
                            ((!commentIsMultiline && c == '\n') || (commentActive && commentIsMultiline && is_multiline_end))) {
                            commentActive = false;
                            tokenActive   = false;
                            tokens.emplace_back(
                                new Token(commentIsMultiline ? currentValue + L"/" : currentValue,
                                          TokenType::COMMENT,
                                          commentIsMultiline ? (tokenPos + 1 - tokenStartIndex) : (tokenPos - tokenStartIndex),
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
                if (!ctre::match<REGEX_WHITESPACE>(it_char, it_char + 1)) {
                    tokenActive      = true;
                    tokenStartIndex  = tokenPos;
                    tokenStartLine   = lineNumber;
                    tokenStartColumn = lineOffset;

                    i--;
                    tokenPos--;
                    lineOffset--;
                    if (c == '\n') {
                        lineNumber--;
                    }
                }
            }

            tokenPos++;
            if (c == '\n') {
                lineOffset = 1;
                lineNumber++;
            } else {
                lineOffset++;
            }
        }

        if (tokenActive) {
            currentValue.pop_back();

            if (commentActive) {
                tokens.emplace_back(new Token(
                    currentValue, TokenType::COMMENT, tokenPos - 1 - tokenStartIndex, tokenStartLine, tokenStartColumn, filePath));
            } else if (stringActive) {
                if (currentValue.size() && currentValue[currentValue.size() - 1] == '"') {
                    tokens.emplace_back(new Token(
                        currentValue, TokenType::LITERAL, tokenPos - tokenStartIndex, tokenStartLine, tokenStartColumn, filePath));
                } else {
                    tokens.emplace_back(new Token(
                        currentValue, TokenType::INVALID_LITERAL, tokenPos - tokenStartIndex, tokenStartLine, tokenStartColumn, filePath));
                }
            } else {
                addKeywordIdentifierLiteral(currentValue, tokenPos - tokenStartIndex, tokenStartLine, tokenStartColumn);
            }
        }

        return tokens;
    }

} // namespace XRT