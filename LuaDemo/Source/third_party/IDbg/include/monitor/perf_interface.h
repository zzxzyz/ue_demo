//
//  Created by mjzheng on 2021/7/22.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_MONITOR_PERF_MONITOR_H_
#define IDBG_MONITOR_PERF_MONITOR_H_

#include <functional>
#include <string>
#include <map>
#include "build_config.h"
#include <memory>

namespace IDbg {

enum class PerfMonitorType : int64_t {
  kPerfNone = 0,
  kHighCpu = 1 << 0,
  kHang = 1 << 1,
};

struct PerfConfig {
  using Ptr = std::shared_ptr<PerfConfig>;
  float threshold_b = 60;
  float threshold_f = 80;
};

enum class MonitorStatus: uint32_t {
  kStop = 0,
  kRunning = 1,
  kRestore = 2,
};

enum class AppState : int32_t {
  kInBackground = 0,
  kInForeground = 1,
};

class PerfInterface {
 public:
  using Ptr = std::shared_ptr<PerfInterface>;
  virtual ~PerfInterface() = default;
  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Flush() = 0; // flush cache perf data
};

IDBG_API PerfInterface::Ptr CreatePerfMonitor(PerfMonitorType monitor_type, PerfConfig::Ptr config);

}  // namespace IDbg

#endif  // IDBG_MONITOR_PERF_MONITOR_H_
