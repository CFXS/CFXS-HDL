#pragma once
#include <exception>
#include <string_view>
#include <StringUtils.hpp>
#include "Language/SourceEntry.hpp"
#include "Token.hpp"

namespace XRT {

    struct _ParserTypeBase {
        using Token        = XRT::Token;
        using TokenType    = XRT::TokenType;
        using TokenStorage = std::vector<Token*>;

        class ParseException : public std::exception {
        public:
            ParseException() {
            }

            const char* what() const noexcept override {
                return "Parser::ParseException";
            }

            virtual size_t GetLine() const {
                return 0;
            }
            virtual size_t GetColumn() const {
                return 0;
            }
        };

        class ParseOverflow : public ParseException {
        public:
            ParseOverflow(const std::string& reason, const Token* current_token) :
                m_Token(current_token), m_Reason(reason + " (" + StringUtils::utf16_to_utf8(current_token->value) + ")") {
            }

            const char* what() const noexcept override {
                return m_Reason.c_str();
            }

            size_t GetLine() const override {
                return m_Token->line;
            }
            size_t GetColumn() const override {
                return m_Token->column;
            }

        private:
            const Token* m_Token;
            std::string m_Reason;
        };

        class NotImplemented : public ParseException {
        public:
            NotImplemented(const std::string& reason, const Token* current_token) :
                m_Token(current_token), m_Reason("Not Implemented: " + reason) {
            }

            const char* what() const noexcept override {
                return m_Reason.c_str();
            }

            size_t GetLine() const override {
                return m_Token->line;
            }
            size_t GetColumn() const override {
                return m_Token->column;
            }

        private:
            const Token* m_Token;
            std::string m_Reason;
        };

        class InvalidPreprocessorDirective : public ParseException {
        public:
            InvalidPreprocessorDirective(const std::string& reason, const Token* token) :
                m_Token(token), m_Reason(reason + " (" + StringUtils::utf16_to_utf8(token->value) + ")") {
            }

            const char* what() const noexcept override {
                return m_Reason.c_str();
            }

            size_t GetLine() const override {
                return m_Token->line;
            }
            size_t GetColumn() const override {
                return m_Token->column;
            }

        private:
            const Token* m_Token;
            std::string m_Reason;
        };

        class ExpectationError : public ParseException {
        public:
            ExpectationError(const std::string& reason, const Token* token) :
                m_Token(token), m_Reason(reason + " (" + StringUtils::utf16_to_utf8(token->value) + ")") {
            }

            const char* what() const noexcept override {
                return m_Reason.c_str();
            }

            size_t GetLine() const override {
                return m_Token->line;
            }
            size_t GetColumn() const override {
                return m_Token->column;
            }

        private:
            const Token* m_Token;
            std::string m_Reason;
        };

        class InvalidPunctuatorSequence : public ParseException {
        public:
            InvalidPunctuatorSequence(const Token* token) : m_Token(token) {
                m_Reason = "Invalid operator: " + StringUtils::utf16_to_utf8(token->value);
            }

            const char* what() const noexcept override {
                return m_Reason.c_str();
            }

            size_t GetLine() const override {
                return m_Token->line;
            }
            size_t GetColumn() const override {
                return m_Token->column;
            }

        private:
            const Token* m_Token;
            std::string m_Reason;
        };

        class UnknownTokenException : public ParseException {
        public:
            UnknownTokenException(const Token* token) : m_Token(token) {
                m_Reason = "Unknown token: " + StringUtils::utf16_to_utf8(token->value);
            }

            const char* what() const noexcept override {
                return m_Reason.c_str();
            }

            size_t GetLine() const override {
                return m_Token->line;
            }
            size_t GetColumn() const override {
                return m_Token->column;
            }

        private:
            const Token* m_Token;
            std::string m_Reason;
        };
    };

} // namespace XRT