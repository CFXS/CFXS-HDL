#pragma once
#include <filesystem>
#include "TokenizerException.hpp"
#include "Token.hpp"

namespace XRT {

    class Tokenizer {
    public:
        Tokenizer()  = default;
        ~Tokenizer() = default;

        std::vector<std::unique_ptr<Token>> Tokenize(const std::string& source, const std::filesystem::path* filePath);
    };

} // namespace XRT