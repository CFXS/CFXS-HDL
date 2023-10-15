#include "Token.hpp"
#include "Log/Logger.hpp"
#include "Log/ANSI.hpp"

namespace XRT {

    void XRT::Token::Print(const std::filesystem::path& path) {
        LOG_TRACE("{}[{}]{}", ANSI_BLUE, ToString(type), ANSI_RESET);
        LOG_TRACE(
            " - {}'{}'{}", ANSI_GREEN, StringUtils::utf16_to_utf8(type == TokenType::SPACE ? std::wstring_view{} : value), ANSI_RESET);
        LOG_TRACE(" - {}\"{}:{}:{}\"{}", ANSI_YELLOW, path, line, column, ANSI_RESET);
    }

    static uint8_t s_TokenMemory[1024 * 1024 * 16];
    static size_t s_TokenMemPos = 0;
    void* XRT::Token::tok_malloc(size_t size) {
        return reinterpret_cast<void*>(&s_TokenMemory[s_TokenMemPos += size]);
    }
    void XRT::Token::tok_free(void*) {
    }

} // namespace XRT