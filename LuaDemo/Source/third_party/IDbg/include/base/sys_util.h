//
//  Created by mjzheng on 2021/4/13.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_BASE_SYS_UTIL_H_
#define IDBG_BASE_SYS_UTIL_H_

#include <string>
#include <vector>
#include "build_config.h"

#define FLOAT_EQU(a, b) (fabs((a)-(b))) < (FLT_EPSILON)

#define FLOAT_GREATER(a, b) ((a) - (b)) > (FLT_EPSILON)
#define FLOAT_LESS_EQU(a, b) ((a) - (b)) < (FLT_EPSILON)

#define FLOAT_LESS(a, b) ((a) - (b)) < -(FLT_EPSILON)
#define FLOAT_GREATER_EQU(a, b) ((a) - (b)) > -(FLT_EPSILON)

#define DOUBLE_EQU(a, b) (fabs((a)-(b))) < (DBL_EPSILON)

#define DOUBLE_GREATER(a, b) ((a) - (b)) > (DBL_EPSILON)
#define DOUBLE_LESS_EQU(a, b) ((a) - (b)) < (DBL_EPSILON)

#define DOUBLE_LESS(a, b) ((a) - (b)) < -(DBL_EPSILON)
#define DOUBLE_GREATER_EQU(a, b) ((a) - (b)) > -(DBL_EPSILON)

IDBG_API long ShowGood();

namespace IDbg {

IDBG_API uint64_t GetCurrentMsc();

IDBG_API void PrintVersion();

IDBG_API const char* GetLastPathEntry(const char* const path);

/*
 2023-09-01 17:39:10
 */
IDBG_API std::string GetDate();

IDBG_API void SetThreadName(const char* name);

IDBG_API std::string GetThreadName();

/*
 get process name, idbg_demo
 */
IDBG_API std::string GetProcessName();

/*
 get process id
 */
IDBG_API int GetProcessId();

/*
 get parent process id
 */
IDBG_API int GetProcessParentId();

/*
 get app full patch
 */
IDBG_API std::string GetProcessFullPath();

/*
 
 */
IDBG_API std::string GetAppVersion();

/*
 com.mj.cross-demo
 */
IDBG_API std::string GetAppIndentifier();


/*
 iPhone14,4 etc
 */
IDBG_API std::string GetDeviceModel();

/*
 iPhone OS 15.6.1 (19G82)
 */
IDBG_API std::string GetOSVersion();


/*
 arm, arm64, i386, x64
 */
IDBG_API std::string GetCPUArch();

/*
 26AF557A-30CE-3BA7-B54A-D0FB9AF5FA77
 8f2afae1160f4d94817962fd3992435c
 transfer uuid or generate a new uuid
 */
IDBG_API std::string GetUUID(const uint8_t* bytes, bool split);

IDBG_API uint64_t GetProcessCreateTime();

IDBG_API char* TrimString(char* str1, char** str2);
IDBG_API std::vector<std::string> SplitString(char* str1, char* delimiter);

#if defined(OS_WIN_IDBG)
std::string UnicodeToUTF8(const std::wstring&src);
std::wstring UTF8ToUnicode(const std::string& src);
std::wstring AsciiToUnicode(const std::string& src);
std::string UnicodeToAscii(const std::wstring& src);
#endif

}  // namespace IDbg

#endif  // IDBG_BASE_SYS_UTIL_H_
