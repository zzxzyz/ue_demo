//
//  Created by mjzheng on 2021/4/13.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_BUILD_CONFIG_H_
#define IDBG_BUILD_CONFIG_H_

#if defined(ANDROID)
  #define OS_POSIX_IDBG
  #define OS_ANDROID_IDBG
#elif defined(__APPLE__)
  #include <TargetConditionals.h>
  #define OS_POSIX_IDBG
  #if defined(TARGET_OS_OSX) && TARGET_OS_OSX
    #define OS_OSX_IDBG
  #endif
  #if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
    #define OS_IOS_IDBG
    #define OS_IOS_SIMULATOR_IDBG
  #elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    #define OS_IOS_IDBG
  #endif
  #if defined(__LP64__)
    #define OS_64
  #else
    #define OS_32
  #endif
#elif defined(__linux__)
  #include <unistd.h>
  #define OS_POSIX_IDBG
  #define OS_LINUX_IDBG
  #if defined(__GLIBC__) && !defined(__UCLIBC__)
    #define LIBC_GLIBC
  #endif
#elif defined(_WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  #define OS_WIN_IDBG
  #ifdef _MSC_VER
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4201)
    #pragma warning(disable: 4251)
    #pragma warning(disable: 4099)
    #pragma warning(disable: 4819)
  #endif
  #ifdef _WIN64
    #define OS_64
  #else
    #define OS_32
  #endif
#else
  #error Please add support for your platform in build_config.h
#endif

// only for IDBG_API
#if defined(OS_WIN_IDBG)
  #if defined(EXPORTING_IDBG)
    #define IDBG_API __declspec(dllexport)
  #else
    #define IDBG_API __declspec(dllimport)
  #endif
#else
  #define IDBG_API
#endif

// define trace format
#if defined(OS_WIN_IDBG)
  #if defined(OS_64)
    #define TRACE_FMT         "%-4d%-31s 0x%016llx %s + %llx\n"
    #define POINTER_FMT       "0x%016llx"
    #define POINTER_SHORT_FMT "0x%llx"
    #define IMAGE_FMT         "0x%016llx - 0x%016llx %s %s <%s> %s\n"
  #else
    #define TRACE_FMT         "%-4d%-31s 0x%08lx %s + %lu\n"
    #define POINTER_FMT       "0x%08lx"
    #define POINTER_SHORT_FMT "0x%lx"
    #define IMAGE_FMT         "0x%08lx - 0x%08lx %s %s <%s> %s\n"
  #endif
#else
  #if defined(OS_64)
    #define TRACE_FMT         "%-4d%-31s 0x%016lx %s + %lu\n"
    #define POINTER_FMT       "0x%016lx"
    #define POINTER_SHORT_FMT "0x%lx"
    #define IMAGE_FMT         "0x%016lx - 0x%016llx %s %s <%s> %s\n"
  #else
    #define TRACE_FMT         "%-4d%-31s 0x%08lx %s + %lu\n"
    #define POINTER_FMT       "0x%08lx"
    #define POINTER_SHORT_FMT "0x%lx"
    #define IMAGE_FMT         "0x%08lx - 0x%08lx %s %s <%s> %s\n"
  #endif
#endif
#define UNUSED(x) (void)(x)

#endif  // IDBG_BUILD_CONFIG_H_
