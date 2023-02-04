#pragma once
#include <Assert.hpp>
#include <Log/Logger.hpp>

namespace XRT {

    enum class TokenType {
        UNKNOWN,
        SPACE,
        KEYWORD,
        IDENTIFIER,
        LITERAL,
        PUNCTUATOR,
        COMMENT,
        __SPECIFIED__,
        STRING_LITERAL,    // "string"
        DECIMAL_LITERAL,   // 1234
        HEX_LITERAL,       // 0x1234
        FLOAT_LITERAL,     // 1.234
        TIME_LITERAL,      // 12ns
        ASSIGN,            // =
        DOT,               // .
        SEPARATOR,         // ;
        OPEN_ANGLE,        // <
        CLOSE_ANGLE,       // >
        LT = OPEN_ANGLE,   // <
        GT = CLOSE_ANGLE,  // >
        OPEN_BRACKET,      // [
        CLOSE_BRACKET,     // ]
        OPEN_PAREN,        // (
        CLOSE_PAREN,       // )
        OPEN_SCOPE,        // {
        CLOSE_SCOPE,       // }
        NOT,               // !
        ADD,               // +
        SUB,               // -
        MUL,               // *
        DIV,               // /
        XOR,               // ^
        AND,               // &
        OR,                // |
        RESOLVE,           // ::
        BOOL_AND,          // &&
        BOOL_OR,           // ||
        ASSIGN_ADD,        // +=
        ASSIGN_SUB,        // -=
        ASSIGN_MUL,        // *=
        ASSIGN_DIV,        // /=
        ASSIGN_AND,        // &=
        ASSIGN_OR,         // |=
        ASSIGN_XOR,        // ^=
        RANGE,             // ..
        INC,               // ++
        DEC,               // --
        EQUAL,             // ==
        NOT_EQUAL,         // !=
        GTEQ,              // >=
        LTEQ,              // <=
        LSL,               // <<
        LSR,               // >>
        ROL,               // <<<
        ROR,               // >>>
        LSL_RESIZE,        // <..< //? Extends resulting type on overflow
        LSR_RESIZE,        // >..> //? Shrinks resulting type on underflow
        ASSIGN_LSL,        // <<=
        ASSIGN_LSR,        // >>=
        ASSIGN_ROL,        // <<<=
        ASSIGN_ROR,        // >>>=
        ASSIGN_LSL_RESIZE, // <..<= //? Extends resulting type on overflow
        ASSIGN_LSR_RESIZE, // >..>= //? Shrinks resulting type on underflow
        END_OF_FILE,
    };

    struct Token {
        Token(TokenType _type, std::wstring_view _value, size_t _offset, size_t _line, size_t _column) :
            type(_type), value(_value), offset(_offset), line(_line), column(_column) {
            CFXS_ASSERT(!(type == TokenType::PUNCTUATOR && value.size() != 1), "Invalid punctuator");
        }
        TokenType type;
        std::wstring_view value;
        size_t offset;
        size_t line;
        size_t column;

        void Print(const std::filesystem::path& path);
    };

} // namespace XRT

inline std::string ToString(XRT::TokenType t) {
    switch (t) {
        case XRT::TokenType::KEYWORD: return "KEYWORD";
        case XRT::TokenType::IDENTIFIER: return "IDENTIFIER";
        case XRT::TokenType::LITERAL: return "LITERAL";
        case XRT::TokenType::PUNCTUATOR: return "PUNCTUATOR";
        case XRT::TokenType::COMMENT: return "COMMENT";
        case XRT::TokenType::SPACE: return "SPACE";
        case XRT::TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case XRT::TokenType::OPEN_ANGLE: return "OPEN_ANGLE";
        case XRT::TokenType::CLOSE_ANGLE: return "CLOSE_ANGLE/GT";
        case XRT::TokenType::OPEN_BRACKET: return "OPEN_BRACKET/LT";
        case XRT::TokenType::CLOSE_BRACKET: return "CLOSE_BRACKET";
        case XRT::TokenType::OPEN_PAREN: return "OPEN_PAREN";
        case XRT::TokenType::CLOSE_PAREN: return "CLOSE_PAREN";
        case XRT::TokenType::OPEN_SCOPE: return "OPEN_SCOPE";
        case XRT::TokenType::CLOSE_SCOPE: return "CLOSE_SCOPE";
        case XRT::TokenType::DECIMAL_LITERAL: return "DECIMAL_LITERAL";
        case XRT::TokenType::HEX_LITERAL: return "HEX_LITERAL";
        case XRT::TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case XRT::TokenType::TIME_LITERAL: return "TIME_LITERAL";
        case XRT::TokenType::SEPARATOR: return "SEPARATOR";
        case XRT::TokenType::RESOLVE: return "RESOLVE";
        case XRT::TokenType::ASSIGN: return "ASSIGN";
        case XRT::TokenType::NOT: return "NOT";
        case XRT::TokenType::ADD: return "ADD";
        case XRT::TokenType::SUB: return "SUB";
        case XRT::TokenType::MUL: return "MUL";
        case XRT::TokenType::DIV: return "DIV";
        case XRT::TokenType::XOR: return "XOR";
        case XRT::TokenType::AND: return "AND";
        case XRT::TokenType::OR: return "OR";
        case XRT::TokenType::BOOL_AND: return "BOOL_AND";
        case XRT::TokenType::BOOL_OR: return "BOOL_OR";
        case XRT::TokenType::ASSIGN_ADD: return "ASSIGN_ADD";
        case XRT::TokenType::ASSIGN_SUB: return "ASSIGN_SUB";
        case XRT::TokenType::ASSIGN_MUL: return "ASSIGN_MUL";
        case XRT::TokenType::ASSIGN_DIV: return "ASSIGN_DIV";
        case XRT::TokenType::ASSIGN_AND: return "ASSIGN_AND";
        case XRT::TokenType::ASSIGN_OR: return "ASSIGN_OR";
        case XRT::TokenType::ASSIGN_XOR: return "ASSIGN_XOR";
        case XRT::TokenType::RANGE: return "RANGE";
        case XRT::TokenType::INC: return "INC";
        case XRT::TokenType::DEC: return "DEC";
        case XRT::TokenType::EQUAL: return "EQUAL";
        case XRT::TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case XRT::TokenType::GTEQ: return "GTEQ";
        case XRT::TokenType::LTEQ: return "LTEQ";
        case XRT::TokenType::LSL: return "LSL";
        case XRT::TokenType::LSR: return "LSR";
        case XRT::TokenType::ROL: return "ROL";
        case XRT::TokenType::ROR: return "ROR";
        case XRT::TokenType::LSL_RESIZE: return "LSL_RESIZE";
        case XRT::TokenType::LSR_RESIZE: return "LSR_RESIZE";
        case XRT::TokenType::ASSIGN_LSL: return "ASSIGN_LSL";
        case XRT::TokenType::ASSIGN_LSR: return "ASSIGN_LSR";
        case XRT::TokenType::ASSIGN_ROL: return "ASSIGN_ROL";
        case XRT::TokenType::ASSIGN_ROR: return "ASSIGN_ROR";
        case XRT::TokenType::ASSIGN_LSL_RESIZE: return "ASSIGN_LSL_RESIZE";
        case XRT::TokenType::ASSIGN_LSR_RESIZE: return "ASSIGN_LSR_RESIZE";
        case XRT::TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "???";
    }
}