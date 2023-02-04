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

} // namespace XRT