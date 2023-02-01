enum TokenType {
    KEYWORD = "KEYWORD",
    IDENTIFIER = "IDENTIFIER",
    INVALID_IDENTIFIER = "INVALID_IDENTIFIER",
    LITERAL = "LITERAL",
    INVALID_LITERAL = "INVALID_LITERAL",
    OPERATOR = "OPERATOR",
    PUNCTUATOR = "PUNCTUATOR",
    COMMENT = "COMMENT"
}

interface Token {
    type: TokenType;
    value: string;
}

enum ScopeType {
    ROOT,
    NAMESPACE,
    FUNCTION,
    COMPONENT,
}

const s_Identifiers: string[] = [
    "namespace",
    "component",
    "ports",
    "registers",
    "implementation",
    "extern",
    "in",
    "out",
    "inout",
    "if",
    "else",
    "for",
    "template",
    "typename",
    "include",
].sort()

const regex_Literal = /^[a-zA-Z_$][a-zA-Z_$0-9]*$/
const regex_HexLiteral = /^0x[0-9a-fA-FxX_']+$/
const regex_BinaryLiteral = /^0b[01xX_']+$/
const regex_DecimalLiteral = /^[0-9_']+$/
const regex_RangeLiteral = /^[0-9]+\.\.[0-9]+$/
const regex_Punctuator = /[!%\^&\*\(\)\-\+={}\|~\[\];:"<>\?,\.#\/\\]/
const regex_Whitespace = /\s/

function IsKeyword(str: string): boolean {
    return s_Identifiers.includes(str)
}

export class Tokenizer {
    m_Tokens: Token[] = []
    constructor() {
    }

    Tokenize(content: string) {
        const SEGMENT_VALUE = 0
        const SEGMENT_TYPE = 1

        // end padding for single position lookahead and remove carriage returns
        content = content.replace(/\r/g, "") + " "
        var currentValue = ""
        var tokenActive = false
        var segments: Array<any> = []
        var commentActive = false
        var commentIsMultiline = false
        var stringActive = false

        const contentLen = content.length
        for (var i = 0; i < contentLen - 1; i++) {
            const c = content.charAt(i)
            const nextChar = content.charAt(i + 1)
            const prevChar = i > 0 ? content.charAt(i - 1) : null

            if (tokenActive) {
                const is_whitespace = regex_Whitespace.test(c)
                const is_punctuation = regex_Punctuator.test(c)

                if (!commentActive && !stringActive) {
                    const is_comment_start = (c == "/" && (nextChar == "/" || nextChar == "*"))
                    const is_string_start = c == '"'

                    if (is_comment_start) {
                        commentIsMultiline = nextChar == "*"
                        commentActive = true
                    } else if (is_string_start) {
                        stringActive = true
                        currentValue = c
                        continue
                    }
                }

                if (stringActive) {
                    const is_string_end = c == '"'
                    currentValue += c
                    if (is_string_end) {
                        i++
                        segments.push([currentValue, TokenType.LITERAL])
                        currentValue = ""
                        tokenActive = false
                        stringActive = false
                    }
                } else {
                    if (is_whitespace && !commentActive) {
                        segments.push([currentValue, IsKeyword(currentValue) ? TokenType.KEYWORD : TokenType.IDENTIFIER])
                        tokenActive = false
                        currentValue = ""
                    } else if (is_punctuation && !commentActive) {
                        if (currentValue !== "") {
                            segments.push([currentValue, IsKeyword(currentValue) ? TokenType.KEYWORD : TokenType.IDENTIFIER])
                            segments.push([c, TokenType.PUNCTUATOR])
                            currentValue = ""
                        } else {
                            segments.push([c, TokenType.PUNCTUATOR])
                        }
                        tokenActive = false
                    } else {
                        const is_multiline_end = commentActive && (c == "/" && prevChar == "*")
                        if (commentActive && ((!commentIsMultiline && c == "\n") || (commentActive && commentIsMultiline && is_multiline_end))) {
                            commentActive = false
                            tokenActive = false
                            segments.push([commentIsMultiline ? currentValue + "/" : currentValue, TokenType.COMMENT])
                            currentValue = ""
                        } else {
                            currentValue += c
                        }
                    }
                }
            } else {
                if (regex_Whitespace.test(c)) {
                    continue
                } else {
                    i--;
                    tokenActive = true
                }
            }
        }

        if (tokenActive) {
            if (commentActive) {
                segments.push([commentIsMultiline ? currentValue + "/" : currentValue, TokenType.COMMENT])
            } else if (stringActive) {
                if (currentValue.charAt(currentValue.length - 1) == '"') {
                    segments.push([currentValue, TokenType.LITERAL])
                } else {
                    segments.push([currentValue, TokenType.INVALID_LITERAL])
                }
            } else {
                segments.push([currentValue, IsKeyword(currentValue) ? TokenType.KEYWORD : TokenType.IDENTIFIER])
            }
        }

        for (var i = 0; i < segments.length; i++) {
            const seg = segments[i]
            const value = seg[SEGMENT_VALUE]
            if (seg[SEGMENT_TYPE] == TokenType.IDENTIFIER) {
                if (!regex_Literal.test(value)) {
                    if (regex_HexLiteral.test(value)) {
                        segments[i][SEGMENT_TYPE] = TokenType.LITERAL
                    } else if (regex_BinaryLiteral.test(value)) {
                        segments[i][SEGMENT_TYPE] = TokenType.LITERAL
                    } else if (regex_DecimalLiteral.test(value)) {
                        segments[i][SEGMENT_TYPE] = TokenType.LITERAL
                    } else if (regex_RangeLiteral.test(value)) {
                        segments[i][SEGMENT_TYPE] = TokenType.LITERAL
                    } else {
                        console.log("Invalid identifier:", value)
                        segments[i][SEGMENT_TYPE] = TokenType.INVALID_IDENTIFIER
                    }
                }
            }
        }

        console.log(segments)
    }
}
