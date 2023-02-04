#pragma once

#define CFXS_ASSERT(cond, message)                                           \
    if (!(cond)) {                                                           \
        LOG_ERROR("[ASSERT] {}\n - \"{}:\"{}", message, __FILE__, __LINE__); \
        __debugbreak();                                                      \
    }