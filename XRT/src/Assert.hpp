#pragma once

#ifdef __linux__
    #include <csignal>
    #define __BKPT() std::raise(SIGINT)
#else
    #define __BKPT() __debugbreak
#endif

#ifdef DEBUG
    #error debug mode
    #define CFXS_ASSERT(cond, message)                                           \
        if (!(cond)) {                                                           \
            LOG_ERROR("[ASSERT] {}\n - \"{}:\"{}", message, __FILE__, __LINE__); \
            __BKPT();                                                            \
        }
#else
    #define CFXS_ASSERT(cond, message)
#endif