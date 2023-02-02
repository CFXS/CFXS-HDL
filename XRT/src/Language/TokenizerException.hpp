#pragma once
#include <exception>
#include <string>
#include <Log/ANSI.hpp>

namespace XRT {

    class Tokenizer;

    class TokenizerException : public std::exception {
        friend class Tokenizer;

    private:
        TokenizerException(std::string reason) : m_Reason(reason) {
        }
        virtual ~TokenizerException() = default;

    public:
        char const* what() const override {
            return m_Reason.data();
        }

        std::string GetReason() const {
            return ANSI_RED + m_Reason + ANSI_RESET;
        }

    private:
        std::string m_Reason;
    };

} // namespace XRT
