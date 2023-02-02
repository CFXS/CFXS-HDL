#pragma once
#include <filesystem>
#include "TokenizerException.hpp"
#include "Token.hpp"

namespace XRT {

    class Tokenizer {
    public:
        Tokenizer()  = default;
        ~Tokenizer() = default;

        void Tokenize(const std::string& source, std::filesystem::path* filePath);
    };

} // namespace XRT