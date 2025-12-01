//
//  Created by mjzheng on 2021/7/22.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_MONITOR_PERF_SAMPLE_H_
#define IDBG_MONITOR_PERF_SAMPLE_H_

#include <functional>
#include <string>
#include <map>
#include "base/singleton.h"
#include "core/thread_cpu_info.h"
#include "thread/cross_handler_thread.h"
#include "thread/cross_timer.h"
#include "monitor/perf_interface.h"

namespace IDbg {

using IdToSlices = std::map<uint64_t, IDbg::ThreadStackSlices>;

using SampleDelegate = std::function<void(std::string&& exception_type, std::string&& msg, IDbg::ThreadStackSlices&& slices)>;

class IDBG_API PerfSample : public Singleton<PerfSample>{
  friend class Singleton<PerfSample>;
  
public:
  void SetSampleDelegate(SampleDelegate&& delegate);

  uint64_t Start(int32_t interval, const ThreadIdArray& id_ls);
  
  void Stop(uint64_t timer_id);
  
  IDbg::ThreadStackSlices GetResult(uint64_t timer_id);
  
  void OnSampleDelegate(std::string&& exception_type, std::string&& msg, IDbg::ThreadStackSlices&& slice);
  
private:
  PerfSample() = default;
  ~PerfSample();
  
private:
  void SampleOne(uint64_t timer_id, const ThreadIdArray& id_ls);
  
  void AddSlice(uint64_t timer_id, ThreadStackArray&& slice);
  
private:
  IDbg::CrossTimer::Ptr timer_;
  SampleDelegate delegate_;
  
  std::mutex mu_;
  IdToSlices id_to_slices_;
};

}  // namespace IDbg

#endif  // IDBG_MONITOR_PERF_SAMPLE_H_
