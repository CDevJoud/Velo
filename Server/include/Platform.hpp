#pragma once

#if defined(_WIN32)

    #if !defined(_WIN64)
        #error "32-bit Windows is not supported"
    #endif

    #define VELO_PLATFORM_WINDOWS 1
    #define VELO_PLATFORM_NAME "windows"

#elif defined(__APPLE__) && defined(__MACH__) || defined(__linux__)

    #include <TargetConditionals.h>

    #define VELO_PLATFORM_UNIX 1
	#define VELO_PLATFORM_NAME "unix"

#else

    #error "Unsupported platform"

#endif


#if defined(_M_X64) || defined(__x86_64__)

    #define VELO_ARCH_X64 1
    #define VELO_ARCH_NAME "x64"

#elif defined(_M_ARM64) || defined(__aarch64__)

    #define VELO_ARCH_ARM64 1
    #define VELO_ARCH_NAME "arm64"

#elif defined(_M_IX86) || defined(__i386__)

    #error "32-bit x86 is not supported"

#else

    #error "Unsupported CPU architecture"

#endif

#if defined(_MSC_VER)

    #define VELO_COMPILER_MSVC 1

#elif defined(__clang__)

    #define VELO_COMPILER_CLANG 1

#elif defined(__GNUC__)

    #define VELO_COMPILER_GCC 1

#else

    #error "Unsupported compiler"

#endif

#if defined(NDEBUG)

    #define VELO_BUILD_RELEASE 1

#else

    #define VELO_BUILD_DEBUG 1

#endif