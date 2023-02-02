#pragma once
#include <chrono>
#include <Log/Logger.hpp>

class ScopeExecTime {
public:
    ScopeExecTime(const char* label) : m_Label(label) {
        m_TimePoint = std::chrono::high_resolution_clock::now();
    }
    ~ScopeExecTime() {
        auto now = std::chrono::high_resolution_clock::now();
        LOG_TRACE("[ScopeExecTime] \"{}\" {:.3f}ms",
                  m_Label,
                  std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_TimePoint).count() / 1000000.0);
    }

private:
    const char* m_Label;
    std::chrono::steady_clock::time_point m_TimePoint;
};