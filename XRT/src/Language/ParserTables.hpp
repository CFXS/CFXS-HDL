#pragma once
#include "_ParserTypeBase.hpp"

/// Single punctuator spec map
static const auto s_SinglePunctuatorMap = []() {
    using TokenType = XRT::_ParserTypeBase::TokenType;

    std::array<XRT::_ParserTypeBase::TokenType, 256> table = {};

    table['.'] = TokenType::DOT;
    table[','] = TokenType::COMMA;
    table[';'] = TokenType::SEPARATOR;
    table['<'] = TokenType::OPEN_ANGLE;
    table['>'] = TokenType::CLOSE_ANGLE;
    table['['] = TokenType::OPEN_BRACKET;
    table[']'] = TokenType::CLOSE_BRACKET;
    table['('] = TokenType::OPEN_PAREN;
    table[')'] = TokenType::CLOSE_PAREN;
    table['{'] = TokenType::OPEN_SCOPE;
    table['}'] = TokenType::CLOSE_SCOPE;
    table['!'] = TokenType::NOT;
    table['+'] = TokenType::ADD;
    table['-'] = TokenType::SUB;
    table['*'] = TokenType::MUL;
    table['/'] = TokenType::DIV;
    table['^'] = TokenType::XOR;
    table['&'] = TokenType::AND;
    table['|'] = TokenType::OR;
    table['='] = TokenType::ASSIGN;
    table['?'] = TokenType::TERNARY_IF;
    table[':'] = TokenType::TERNARY_ELSE;
    table['#'] = TokenType::PREPROCESSOR;
    return table;
}();
