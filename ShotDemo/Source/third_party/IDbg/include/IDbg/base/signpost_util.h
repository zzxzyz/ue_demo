#pragma once

#include <stdint.h>
#include "build_config.h"

#if defined(OS_OSX_IDBG) || defined(OS_IOS_IDBG)
#include <os/signpost.h>
#endif

#define subsystem_wwmapp "com.mj.idbg"

#if defined(OS_OSX_IDBG) || defined(OS_IOS_IDBG)
#define APP_SP_ID_EXCLUSIVE OS_SIGNPOST_ID_EXCLUSIVE
#define APP_SP_ID_INVALID  OS_SIGNPOST_ID_INVALID

#define GET_SIGNPOST_LOG(subsystem, category) \
os_log_t sp_log = os_log_create(subsystem, (#category));

#define MAKE_SIGNPOST_ID_BY_POINTER(ptr) \
os_signpost_id_t interval_id = APP_SP_ID_INVALID;\
if (__builtin_available(macOS 10.14, iOS 12.0, *)) {\
  interval_id = os_signpost_id_make_with_pointer(sp_log, ptr);\
}

#define MAKE_SIGNPOST_ID_BY_CODE(code) \
os_signpost_id_t interval_id = code;\
if (code == APP_SP_ID_INVALID) {\
  if (__builtin_available(macOS 10.14, iOS 12.0, *)) {\
    interval_id = os_signpost_id_generate(sp_log);\
  }\
}

#define SIGNPOST_BEGIN(category, name)\
if (__builtin_available(macOS 10.14, iOS 12.0, *)) {\
  os_signpost_interval_begin(sp_log, interval_id, (#category), "%{public}s", name);\
}

#define SIGNPOST_END(category)\
if (__builtin_available(macOS 10.14, iOS 12.0, *)) {\
  os_signpost_interval_end(sp_log, interval_id, (#category));\
}

// signpost sync
#define SIGNPOST_LOG_BEGIN(subsystem, category, name, code) \
GET_SIGNPOST_LOG(subsystem, category)\
MAKE_SIGNPOST_ID_BY_CODE(code)\
SIGNPOST_BEGIN(category, name)

// signpost async by code
#define SIGNPOST_LOG_END(subsystem, category, name, code) \
GET_SIGNPOST_LOG(subsystem, category)\
MAKE_SIGNPOST_ID_BY_CODE(code)\
SIGNPOST_END(category)

// signpost async by pointer
#define SIGNPOST_LOG_BEGIN_POINTER(subsystem, category, name, ptr) \
GET_SIGNPOST_LOG(subsystem, category)\
MAKE_SIGNPOST_ID_BY_POINTER(ptr)\
SIGNPOST_BEGIN(category, name)

#define SIGNPOST_LOG_END_POINTER(subsystem, category, name, ptr) \
GET_SIGNPOST_LOG(subsystem, category)\
MAKE_SIGNPOST_ID_BY_POINTER(ptr)\
SIGNPOST_END(category)

// signpost and save data
#define SIGNPOST_AND_SAVE_BEGIN(subsystem, category, name, code)\
UNUSED(code);

#define SIGNPOST_AND_SAVE_END(subsystem, category, name, code)\
UNUSED(code);

#else
#define APP_SP_ID_EXCLUSIVE ((uint64_t)0xEEEEB0B5B2B2EEEE)
#define APP_SP_ID_INVALID ((uint64_t)~0)

#define SIGNPOST_LOG_BEGIN(subsystem, category, name, code)\
UNUSED(code);

#define SIGNPOST_LOG_END(subsystem, category, name, code)\
UNUSED(code);

#define SIGNPOST_LOG_BEGIN_POINTER(subsystem, category, name, ptr)
#define SIGNPOST_LOG_END_POINTER(subsystem, category, name, ptr)

#define SIGNPOST_AND_SAVE_BEGIN(subsystem, category, name, code)\
UNUSED(code);

#define SIGNPOST_AND_SAVE_END(subsystem, category, name, code)\
UNUSED(code);

#endif
