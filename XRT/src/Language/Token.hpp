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
        STRING_LITERAL,  // "string"
        DECIMAL_LITERAL, // 1234
        HEX_LITERAL,     // 0x1234
        FLOAT_LITERAL,   // 1.234
        TIME_LITERAL,    // 12ns
        OPEN_ANGLE,      // <
        CLOSE_ANGLE,     // >
        OPEN_BRACKET,    // [
        CLOSE_BRACKET,   // ]
        OPEN_PAREN,      // (
        CLOSE_PAREN,     // )
        OPEN_SCOPE,      // {
        CLOSE_SCOPE,     // }
        __OPERATOR_START__,
        OP_RESOLVE,           // ::
        OP_NOT,               // !
        OP_ADD,               // +
        OP_SUB,               // -
        OP_MUL,               // *
        OP_DIV,               // /
        OP_XOR,               // ^
        OP_AND,               // &
        OP_OR,                // |
        OP_BOOL_AND,          // &&
        OP_BOOL_OR,           // ||
        OP_ASSIGN_ADD,        // +=
        OP_ASSIGN_SUB,        // -=
        OP_ASSIGN_MUL,        // *=
        OP_ASSIGN_DIV,        // /=
        OP_ASSIGN_AND,        // &=
        OP_ASSIGN_OR,         // |=
        OP_ASSIGN_XOR,        // ^=
        OP_RANGE,             // ..
        OP_INC,               // ++
        OP_DEC,               // --
        OP_EQUAL,             // ==
        OP_NOT_EQUAL,         // !=
        OP_GTEQ,              // >=
        OP_LTEQ,              // <=
        OP_LSL,               // <<
        OP_LSR,               // >>
        OP_ROL,               // <<<
        OP_ROR,               // >>>
        OP_LSL_RESIZE,        // <..< //? Extends resulting type on overflow
        OP_LSR_RESIZE,        // >..> //? Shrinks resulting type on underflow
        OP_ASSIGN_LSL,        // <<=
        OP_ASSIGN_LSR,        // >>=
        OP_ASSIGN_ROL,        // <<<=
        OP_ASSIGN_ROR,        // >>>=
        OP_ASSIGN_LSL_RESIZE, // <..<= //? Extends resulting type on overflow
        OP_ASSIGN_LSR_RESIZE, // >..>= //? Shrinks resulting type on underflow
        __OPERATOR_END__,
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
        case XRT::TokenType::CLOSE_ANGLE: return "CLOSE_ANGLE";
        case XRT::TokenType::OPEN_BRACKET: return "OPEN_BRACKET";
        case XRT::TokenType::CLOSE_BRACKET: return "CLOSE_BRACKET";
        case XRT::TokenType::OPEN_PAREN: return "OPEN_PAREN";
        case XRT::TokenType::CLOSE_PAREN: return "CLOSE_PAREN";
        case XRT::TokenType::OPEN_SCOPE: return "OPEN_SCOPE";
        case XRT::TokenType::CLOSE_SCOPE: return "CLOSE_SCOPE";
        case XRT::TokenType::DECIMAL_LITERAL: return "DECIMAL_LITERAL";
        case XRT::TokenType::HEX_LITERAL: return "HEX_LITERAL";
        case XRT::TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case XRT::TokenType::TIME_LITERAL: return "TIME_LITERAL";
        case XRT::TokenType::OP_RESOLVE: return "OP_RESOLVE";
        case XRT::TokenType::OP_NOT: return "OP_NOT";
        case XRT::TokenType::OP_ADD: return "OP_ADD";
        case XRT::TokenType::OP_SUB: return "OP_SUB";
        case XRT::TokenType::OP_MUL: return "OP_MUL";
        case XRT::TokenType::OP_DIV: return "OP_DIV";
        case XRT::TokenType::OP_XOR: return "OP_XOR";
        case XRT::TokenType::OP_AND: return "OP_AND";
        case XRT::TokenType::OP_OR: return "OP_OR";
        case XRT::TokenType::OP_BOOL_AND: return "OP_BOOL_AND";
        case XRT::TokenType::OP_BOOL_OR: return "OP_BOOL_OR";
        case XRT::TokenType::OP_ASSIGN_ADD: return "OP_ASSIGN_ADD";
        case XRT::TokenType::OP_ASSIGN_SUB: return "OP_ASSIGN_SUB";
        case XRT::TokenType::OP_ASSIGN_MUL: return "OP_ASSIGN_MUL";
        case XRT::TokenType::OP_ASSIGN_DIV: return "OP_ASSIGN_DIV";
        case XRT::TokenType::OP_ASSIGN_AND: return "OP_ASSIGN_AND";
        case XRT::TokenType::OP_ASSIGN_OR: return "OP_ASSIGN_OR";
        case XRT::TokenType::OP_ASSIGN_XOR: return "OP_ASSIGN_XOR";
        case XRT::TokenType::OP_RANGE: return "OP_RANGE";
        case XRT::TokenType::OP_INC: return "OP_INC";
        case XRT::TokenType::OP_DEC: return "OP_DEC";
        case XRT::TokenType::OP_EQUAL: return "OP_EQUAL";
        case XRT::TokenType::OP_NOT_EQUAL: return "OP_NOT_EQUAL";
        case XRT::TokenType::OP_GTEQ: return "OP_GTEQ";
        case XRT::TokenType::OP_LTEQ: return "OP_LTEQ";
        case XRT::TokenType::OP_LSL: return "OP_LSL";
        case XRT::TokenType::OP_LSR: return "OP_LSR";
        case XRT::TokenType::OP_ROL: return "OP_ROL";
        case XRT::TokenType::OP_ROR: return "OP_ROR";
        case XRT::TokenType::OP_LSL_RESIZE: return "OP_LSL_RESIZE";
        case XRT::TokenType::OP_LSR_RESIZE: return "OP_LSR_RESIZE";
        case XRT::TokenType::OP_ASSIGN_LSL: return "OP_ASSIGN_LSL";
        case XRT::TokenType::OP_ASSIGN_LSR: return "OP_ASSIGN_LSR";
        case XRT::TokenType::OP_ASSIGN_ROL: return "OP_ASSIGN_ROL";
        case XRT::TokenType::OP_ASSIGN_ROR: return "OP_ASSIGN_ROR";
        case XRT::TokenType::OP_ASSIGN_LSL_RESIZE: return "OP_ASSIGN_LSL_RESIZE";
        case XRT::TokenType::OP_ASSIGN_LSR_RESIZE: return "OP_ASSIGN_LSR_RESIZE";
        case XRT::TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "???";
    }
}