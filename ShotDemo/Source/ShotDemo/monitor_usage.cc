//
//  monitor_usage.cc
//  idbg_demo
//
//  Created by 郑俊明 on 2022/1/15.
//

#include "monitor_usage.h"
#include <memory>

#include "build_config.h"
#if defined(OS_WIN_IDBG)
#include <Windows.h>
#endif


#include "base/log.h"
#include "base/report.h"
#include "base/sys_util.h"
#include "core/basic_perf_info.h"
#include "monitor/perf_interface.h"

#include "loop_thread_test.h"

//#include "thread_task.h"
//#include "sys_util.h"

//int RunIDbg() {
//  std::cout << "mjzheng" << std::endl;
//  IDbg::SetThreadName("main");
//  IDbg::ThreadTask th{ "demo_thread" };
//  th.PushTask([] {
//    std::cout << "in thread model " << std::this_thread::get_id() << std::endl;
//    });
//  std::this_thread::sleep_for(std::chrono::seconds(3));
//  return 0;
//}

static void LogPerfImpl(const char* msg) {
}

static void LogImpl(IDbg::LogLevel level, const char* msg) {
  if (level == IDbg::LogLevel::kPerf) {
    LogPerfImpl(msg);
  }
  else {
#if defined(OS_WIN_IDBG)
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
#elif defined(OS_IOS_IDBG) || defined(OS_OSX_IDBG)
    NSLog(@"%s", msg);
#else
    printf("%s", msg);
#endif
  }
}

static void ReportImpl(const std::string& event_name, std::map<std::string, std::string>&& event_properties) {
}

class MonitorUsageImpl : public MonitorUsage {
public:
  using Ptr = std::shared_ptr<MonitorUsage>;
  
  MonitorUsageImpl();
  
  void Start() override;
  
  void Stop() override;

private:
  void StartLoopThread();

  void StopLoopThread();
 
private:
  IDbg::PerfInterface::Ptr perf_;
  LoopThreadTest::Ptr thread_test_;
};

MonitorUsageImpl::MonitorUsageImpl() {
  IDbg::Log::Instance()->SetDelegate(LogImpl);
  IDbg::Report::Instance()->SetDelegate(ReportImpl);
}


MonitorUsage::Ptr CreateMonitorUsage() {
  return std::make_shared<MonitorUsageImpl>();
}

void MonitorUsageImpl::Start() {
 /* if (perf_ == nullptr) {
    auto config = std::make_shared<IDbg::PerfConfig>();
    config->options = 3;
    config->threshold_b = 0;
    config->threshold_f = 0;
    perf_ = IDbg::CreatePerfMonitor(config);
  }
  if (perf_ != nullptr) {
    perf_->Start();
  }*/
  IDbg::GetAppGPU();
  StartLoopThread();
}

void MonitorUsageImpl::Stop() {
  /*if (perf_ != nullptr) {
    perf_->Stop();
  }*/
  StopLoopThread();
  printf("%s\n", "stop monitor");
}

void MonitorUsageImpl::StartLoopThread() {
  thread_test_ = std::make_shared<LoopThreadTest>();
  thread_test_->Start();
}

void MonitorUsageImpl::StopLoopThread() {
  if (thread_test_) {
    thread_test_->Stop();
  }
}
