#include "Language/Token.hpp"
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <iostream>
#include <Log/Logger.hpp>
#include <Language/Tokenizer.hpp>
#include <filesystem>
#include <fstream>
#include <StringUtils.hpp>
#include <argparse/argparse.hpp>
#include <regex>
#include <Log/ANSI.hpp>

using argparse::ArgumentParser;
namespace fs = std::filesystem;

int main(int argc, char** argv) {
    Logger::Initialize();
    LOG_TRACE("CFXS XRT");

    std::vector<fs::path> files_to_process;

    ArgumentParser program(CFXS_PROGRAM_NAME, CFXS_VERSION_STRING);
    program.add_argument("files").help("Files to process").remaining();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& e) {
        std::stringstream ss;
        ss << program;
        LOG_ERROR("{}", e.what());
        LOG_TRACE("{}", ss.str());
        return -1;
    }

    try {
        auto files = program.get<std::vector<std::string>>("files");
        for (auto& path : files) {
            path.erase(std::remove(path.begin(), path.end(), '"'), path.end());

            auto fp   = fs::path{path}.lexically_normal();
            auto ext  = fp.extension();
            auto name = fp.filename();
            auto dir  = fp.parent_path();

            bool is_xrt_file = ctre::match<L"\\.xrt", ctre::case_insensitive>(ext.c_str());
            if (is_xrt_file) {
                bool is_wildcard = ctre::search<L"\\*">(name.c_str());
                if (is_wildcard) {
                    if (fs::is_directory(dir) && fs::exists(dir)) {
                        std::string match = name.string();
                        match             = std::regex_replace(match, std::regex("\\."), "\\.");
                        match             = std::regex_replace(match, std::regex("\\*"), ".*");
                        std::regex rx(match);
                        for (const auto& e : fs::directory_iterator(dir)) {
                            if (!fs::is_regular_file(e))
                                continue;
                            auto fileName = e.path().filename().string();
                            if (std::regex_match(fileName.begin(), fileName.end(), rx)) {
                                fs::path addFilePath = dir.string() + PATH_SEPARATOR + fileName;
                                if (fs::exists(addFilePath))
                                    files_to_process.push_back(addFilePath);
                            }
                        }
                    } else {
                        LOG_ERROR("Directory not found: \"{}\"", dir.string());
                    }
                } else {
                    files_to_process.push_back(fp);
                }
            }
        }

        LOG_TRACE("Files:\n{}", files_to_process);
    } catch (std::logic_error&) {
        LOG_WARN("No files to process");
        return -1;
    }

    for (const auto& sourcePath : files_to_process) {
        std::string source;
        std::ifstream file;
        file.open(sourcePath.string());
        if (file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            source = ss.str();
            file.close();
        } else {
            LOG_ERROR("Failed to open source file");
            return -1;
        }

        auto tok = new XRT::Tokenizer;
        try {
            auto tokens = tok->Tokenize(source, &sourcePath);
            // for (const auto& t : tokens) {
            // t->Print();
            // }
            std::string hl = "";
            size_t line    = 1;
            size_t col     = 0;
            for (size_t i = 0; i < source.size(); i++) {
                if (source[i] == '\n') {
                    line++;
                    col = 0;
                    hl += "\n";
                    continue;
                } else {
                    col++;
                }
                std::string color = ANSI_RESET;

                std::string type = "???";
                for (const auto& t : tokens) {
                    if (t->GetLine() == line && col >= t->GetColumn() && col < (t->GetColumn() + t->GetEntrySize())) {
                        switch (t->GetType()) {
                            case XRT::TokenType::KEYWORD:
                                type  = "KEYWORD";
                                color = ANSI_BLUE;
                                break;
                            case XRT::TokenType::IDENTIFIER: {
                                type     = "IDENTIFIER";
                                auto val = t->GetValue_utf8();
                                if (ctre::match<"bool|unsigned|true|false|unsigned|logic">(val.begin(), val.end())) {
                                    color = ANSI_MAGENTA;
                                } else if (ctre::match<"std|range_of|CFXS|EventSource|pow|is_integral|clockable|Event">(val.begin(),
                                                                                                                        val.end())) {
                                    color = "\u001b[33m";
                                } else if (ctre::match<"CascadeClockDivider">(val.begin(), val.end())) {
                                    color = ANSI_GREEN;
                                } else {
                                    color = ANSI_RESET;
                                }
                                break;
                            }
                            case XRT::TokenType::INVALID_IDENTIFIER:
                                type  = "INVALID_IDENTIFIER";
                                color = ANSI_RED;
                                break;
                            case XRT::TokenType::LITERAL:
                                type  = "LITERAL";
                                color = ANSI_CYAN;
                                break;
                            case XRT::TokenType::INVALID_LITERAL:
                                type  = "INVALID_LITERAL";
                                color = ANSI_RED;
                                break;
                            case XRT::TokenType::OPERATOR:
                                type  = "OPERATOR";
                                color = ANSI_YELLOW;
                                break;
                            case XRT::TokenType::PUNCTUATOR:
                                type  = "PUNCTUATOR";
                                color = ANSI_GRAY;
                                break;
                            case XRT::TokenType::COMMENT:
                                type  = "COMMENT";
                                color = ANSI_DARK_GREEN;
                                break;
                        }
                        break;
                    }
                }

                // hl += color + "[ " + type + " | " + source[i] + " ]\n" + ANSI_RESET;
                hl += color + source[i] + ANSI_RESET;
            }

            LOG_TRACE("\n{}", hl);

        } catch (const XRT::TokenizerException& e) {
            LOG_TRACE(e.GetReason());
        } catch (std::exception* e) {
            LOG_CRITICAL("Tokenize error: {}", e->what());
        }
    }

    return 0;
}