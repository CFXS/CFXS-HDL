// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
#pragma once

// This ignores all warnings raised inside External headers
// #pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
// #pragma warning(pop)
#include "LoggerOverloads.hpp"

class Logger {
public:
    static void Initialize();

    static inline std::shared_ptr<spdlog::logger>& GetCoreLogger() {
        return s_CoreLogger;
    }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
};

// Core log macros
#define LOG_DEBUG(...)    ::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#define LOG_TRACE(...)    ::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Logger::GetCoreLogger()->critical(__VA_ARGS__)