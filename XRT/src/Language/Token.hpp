#pragma once
#include <filesystem>

namespace XRT {

    class Tokenizer;

    enum class TokenType {
        KEYWORD,
        IDENTIFIER,
        INVALID_IDENTIFIER,
        LITERAL,
        INVALID_LITERAL,
        OPERATOR,
        PUNCTUATOR,
        COMMENT,
    };

    class Token {
        friend class Tokenizer;

    public:
        Token(const std::wstring& currentValue,
              TokenType type,
              size_t entrySize,
              size_t line,
              size_t column,
              const std::filesystem::path* filePath);
        ~Token();

        std::string GetValue_utf8();

        inline TokenType GetType() const {
            return m_Type;
        }

        inline size_t GetEntrySize() const {
            return m_EntrySize;
        }

        inline size_t GetLine() const {
            return m_Line;
        }

        inline size_t GetColumn() const {
            return m_Column;
        }

        void Print();
        std::string PrintToString();

    private:
        const std::filesystem::path* m_FilePath;
        std::wstring m_Value;
        TokenType m_Type;
        size_t m_EntrySize;
        size_t m_Line;
        size_t m_Column;
    };

} // namespace XRT
