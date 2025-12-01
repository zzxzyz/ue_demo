//
//  Created by mjzheng on 2021/7/22.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_BASE_LOG_H_
#define IDBG_BASE_LOG_H_

#include <functional>
#include "build_config.h"
#include "singleton.h"

namespace IDbg {

enum class LogLevel : int32_t {
  kPerf = 0,
  kError = 1,
  kWarning = 2,
  kInfo = 3,
  kDebug = 4,
};

using LogDelegate = std::function<void(LogLevel level, const char* msg)>;

class IDBG_API Log : public Singleton<Log>{
  friend class Singleton<Log>;
 public:
  void SetDelegate(LogDelegate&& p);
  void SetVerbosity(const LogLevel verbosity);
  Log& operator()(const LogLevel level, const char *fmt, ...);

 protected:
  void WriteMessage(const LogLevel level, const char *msg);

 private:
  Log() = default;
  ~Log() = default;

 private:
  LogLevel verbosity_ = LogLevel::kDebug;
  LogDelegate delegate_ = nullptr;
};

}  // namespace IDbg

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LOG(level, fmt, ...) \
(*IDbg::Log::Instance())(level, "%s:%d %s " fmt, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)

#define LOG_ONLY(level, fmt, ...) \
(*IDbg::Log::Instance())(level, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) LOG(IDbg::LogLevel::kDebug, fmt,  ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG(IDbg::LogLevel::kInfo, fmt,  ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) LOG(IDbg::LogLevel::kWarning, fmt,  ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG(IDbg::LogLevel::kError, fmt,  ##__VA_ARGS__)
#define LOG_PERF(fmt, ...) LOG_ONLY(IDbg::LogLevel::kPerf, fmt,  ##__VA_ARGS__)

#define ImplStringReadAndWrite(name, value) \
std::string& Get##name() { \
    static std::string name = value; \
    return name; \
}\
void Set##name(const std::string& new_value) { \
    auto& tmp = Get##name();\
    tmp = new_value;\
}

#define DefStringReadAndWrite(name) \
std::string& Get##name();\
void Set##name(const std::string& new_value);


#define ImplStringReadOnly(name, value) \
std::string& Get##name() { \
    static std::string name = value; \
    return name; \
}

#define DefStringReadOnly(name) std::string& Get##name();

#endif  // IDBG_BASE_LOG_H_
