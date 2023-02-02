#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <iostream>
#include <Log/Logger.hpp>
#include <Language/Tokenizer.hpp>
#include <filesystem>
#include <fstream>
#include <StringUtils.hpp>

int main(int argc, char** argv) {
    Logger::Initialize();
    LOG_TRACE("CFXS XRT");

    std::string source;
    std::filesystem::path sourcePath = "C:/CFXS_Projects/CFXS-XRT-Hardware-Description-Language/XRT/Dev/test.xrt";
    std::ifstream file;
    file.open(sourcePath.string());
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        source.resize(size);
        file.seekg(0);
        file.read(&source[0], size);
        file.close();
    } else {
        LOG_ERROR("Failed to open source file");
        return -1;
    }

    auto tok = new XRT::Tokenizer;
    try {
        tok->Tokenize(source, &sourcePath);
    } catch (const XRT::TokenizerException& e) {
        LOG_TRACE(e.GetReason());
    } catch (std::exception* e) {
        LOG_CRITICAL("Tokenize error: {}", e->what());
    }

    return 0;
}