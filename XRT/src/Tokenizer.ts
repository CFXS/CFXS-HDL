import { SourceLocation } from "./SourceLocation"
import chalk from 'chalk'

enum TokenType {
    KEYWORD,
    IDENTIFIER,
    INVALID_IDENTIFIER,
    LITERAL,
    INVALID_LITERAL,
    OPERATOR,
    PUNCTUATOR,
    COMMENT
}

export interface Token extends SourceLocation {
    type: TokenType;
    value: string;
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
    "using",
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

export class TokenizerError extends Error {
    constructor(msg: string, public location: SourceLocation, cause: string | null = null) {
        super(msg)
        this.name = "TokenizerError"
        if (cause)
            this.cause = cause
    }
}

const _TOKENIZER_LOG_ENABLED = true
function TOKENIZER_LOG(...args: any) {
    if (!_TOKENIZER_LOG_ENABLED)
        return
    console.log("[Tokenizer] ", ...args)
}

export class Tokenizer {
    m_Tokens: Token[] = []
    constructor() {
    }

    Tokenize(content: string, sourceLocation: SourceLocation) {
        TOKENIZER_LOG(`Process source "${sourceLocation.file_name}" (${sourceLocation.file_path})`)

        const SEGMENT_VALUE = 0
        const SEGMENT_TYPE = 1
        const SEGMENT_LOCATION = 2

        // end padding for single position lookahead and remove carriage returns
        content = content.replace(/\r/g, "") + " "
        const contentLen = content.length

        var currentValue = ""
        var tokenActive = false
        var tokenStartIndex = 0
        var tokenStartLine = 0
        var tokenStartColumn = 0
        var segments: Array<Token> = []
        var commentActive = false
        var commentIsMultiline = false
        var stringActive = false

        var tokenPos = 0
        var lineNumber = 1
        var linePos = 0
        var lastLinePos = 0

        if (!regex_Whitespace.test(content.charAt(0))) {
            tokenActive = true
            tokenStartIndex = 0
            tokenStartLine = 1
            tokenStartColumn = 0
            linePos = 1
            tokenPos = 1
        }

        for (var i = 0; i < contentLen - 1; i++) {
            const c = content.charAt(i)

            tokenPos++
            if (c === "\n") {
                lastLinePos = linePos
                linePos = 1
                lineNumber++
            } else {
                linePos++
            }

            if (tokenActive) {
                if (!commentActive && !stringActive) {
                    const nextChar = content.charAt(i + 1)
                    const is_comment_start = (c == "/" && (nextChar == "/" || nextChar == "*"))
                    const is_string_start = c == '"'

                    if (is_comment_start) {
                        commentIsMultiline = nextChar == "*"
                        commentActive = true
                        tokenStartIndex = tokenPos - 1
                        tokenStartLine = lineNumber
                        tokenStartColumn = linePos - 1
                    } else if (is_string_start) {
                        stringActive = true
                        currentValue = c
                        tokenStartIndex = tokenPos - 1
                        tokenStartLine = lineNumber
                        tokenStartColumn = linePos - 1
                        continue
                    }
                }

                if (stringActive) {
                    const is_string_end = c == '"'
                    currentValue += c
                    if (is_string_end) {
                        segments.push(<Token>{
                            value: currentValue,
                            type: TokenType.LITERAL,
                            entry_size: tokenPos - tokenStartIndex - 1,
                            line: tokenStartLine,
                            column: tokenStartColumn
                        })
                        currentValue = ""
                        tokenActive = false
                        stringActive = false
                    }
                } else {
                    if (regex_Whitespace.test(c) && !commentActive) {
                        segments.push(<Token>{
                            value: currentValue,
                            type: IsKeyword(currentValue) ? TokenType.KEYWORD : TokenType.IDENTIFIER,
                            entry_size: tokenPos - tokenStartIndex - 1,
                            line: tokenStartLine,
                            column: tokenStartColumn
                        })
                        tokenActive = false
                        currentValue = ""
                    } else if (regex_Punctuator.test(c) && !commentActive) {
                        if (currentValue !== "") {
                            segments.push(<Token>{
                                value: currentValue,
                                type: IsKeyword(currentValue) ? TokenType.KEYWORD : TokenType.IDENTIFIER,
                                entry_size: tokenPos - (tokenStartIndex - 1),
                                line: tokenStartLine,
                                column: tokenStartColumn - 1
                            })
                            segments.push(<Token>{
                                value: c,
                                type: TokenType.PUNCTUATOR,
                                entry_size: 1,
                                line: lineNumber,
                                column: linePos - 1
                            })
                            currentValue = ""
                        } else {
                            segments.push(<Token>{
                                value: c, type: TokenType.PUNCTUATOR,
                                entry_size: 1,
                                line: lineNumber,
                                column: linePos - 1
                            })
                        }
                        tokenActive = false
                    } else {
                        const is_multiline_end = commentActive && (c == "/" && content.charAt(i - 1) == "*")
                        if (commentActive && ((!commentIsMultiline && c == "\n") || (commentActive && commentIsMultiline && is_multiline_end))) {
                            commentActive = false
                            tokenActive = false
                            segments.push(<Token>{
                                value: commentIsMultiline ? currentValue + "/" : currentValue,
                                type: TokenType.COMMENT,
                                entry_size: tokenPos - tokenStartIndex - 1,
                                line: tokenStartLine,
                                column: tokenStartColumn
                            })
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
                    tokenActive = true
                    tokenStartIndex = tokenPos
                    tokenStartLine = lineNumber
                    tokenStartColumn = linePos

                    i--
                    tokenPos--
                    if (c === "\n") {
                        linePos--
                        lineNumber--
                    } else {
                        linePos--
                    }
                }
            }
        }

        if (tokenActive) {
            if (commentActive) {
                segments.push(<Token>{
                    value: commentIsMultiline ? currentValue + "/" : currentValue,
                    type: TokenType.COMMENT,
                    entry_size: tokenPos - tokenStartIndex,
                    line: tokenStartLine,
                    column: tokenStartColumn
                })
            } else if (stringActive) {
                if (currentValue.charAt(currentValue.length - 1) == '"') {
                    segments.push(<Token>{
                        value: currentValue,
                        type: TokenType.LITERAL,
                        entry_size: tokenPos - tokenStartIndex,
                        line: tokenStartLine,
                        column: tokenStartColumn
                    })
                } else {
                    segments.push(<Token>{
                        value: currentValue,
                        type: TokenType.INVALID_LITERAL,
                        entry_size: tokenPos - tokenStartIndex,
                        line: tokenStartLine,
                        column: tokenStartColumn
                    })
                }
            } else {
                segments.push(<Token>{
                    value: currentValue,
                    type: IsKeyword(currentValue) ? TokenType.KEYWORD : TokenType.IDENTIFIER,
                    entry_size: tokenPos - tokenStartIndex,
                    line: tokenStartLine,
                    column: tokenStartColumn
                })
            }
        }

        for (var i = 0; i < segments.length; i++) {
            const seg = segments[i]
            if (seg.type == TokenType.IDENTIFIER) {
                if (!regex_Literal.test(seg.value)) {
                    if (regex_HexLiteral.test(seg.value)) {
                        segments[i].type = TokenType.LITERAL
                    } else if (regex_BinaryLiteral.test(seg.value)) {
                        segments[i].type = TokenType.LITERAL
                    } else if (regex_DecimalLiteral.test(seg.value)) {
                        segments[i].type = TokenType.LITERAL
                    } else if (regex_RangeLiteral.test(seg.value)) {
                        segments[i].type = TokenType.LITERAL
                    } else {
                        segments[i].type = TokenType.INVALID_IDENTIFIER
                        throw new TokenizerError(`Invalid identifier: '${seg.value}'`, new SourceLocation({ file_name: sourceLocation.file_name, file_path: sourceLocation.file_path, line: seg.line, column: seg.column, entry_size: seg.entry_size }))
                    }
                }
            }
        }

        var segIdx = 1
        for (const s of segments) {
            console.log(chalk.blueBright(`[Segment] (${segIdx++}/${segments.length}):`) +
                `\n` + chalk.blueBright("    |") + chalk.gray(` '${s.value}'`) +
                `\n` + chalk.blueBright("    |") + chalk.greenBright(` '${TokenType[s.type]}'`) +
                `\n` + chalk.blueBright("    |") + chalk.greenBright(` '${s.entry_size}'`) +
                `\n` + chalk.blueBright("    |") + chalk.yellowBright(` "${sourceLocation.file_path}:${s.line}:${s.column}"`))
        }
    }
}
