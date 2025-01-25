#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
    #define PZVEND_IS_WINDOWS

    #if defined(_WIN64)
        #define PZVEND_IS_X64
    #else
        #define PZVEND_IS_X32
    #endif
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__))
    #define PZVEND_IS_LINUX

    // TODO: i should see if there are significant differences in win32/64 and linux32/64 for assembly or if i can just unify them like that.
    #if defined(__x86_64__) || defined(__ppc64__)
        #define PZVEND_IS_X64
    #else
        #define PZVEND_IS_X32
    #endif

    #error Linux is not yet supported
#else
    #error Unsupported OS
#endif