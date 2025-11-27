#ifndef CROSS_TIMER_H_
#define CROSS_TIMER_H_

#include "build_config.h"
#include <mutex>
#include <memory>
#include "base/singleton.h"
#include "cross_handler.h"
#include "cross_handler_thread.h"

namespace IDbg {

using CrossTimerTask = std::function<int(uint64_t)>;

struct TimerInfo {
  using Ptr = std::shared_ptr<TimerInfo>;
  uint64_t timer_id;
  int32_t interval;
  CrossTimerTask task;
  uint64_t* extra_info;
  TimerInfo(uint64_t timer_id, int32_t timeout, CrossTimerTask task) {
    this->timer_id = timer_id;
    this->interval = timeout;
    this->task = task;
    extra_info = nullptr;
  }
};

class IDBG_API CrossTimer : public std::enable_shared_from_this<CrossTimer> {
public:
  using Ptr = std::shared_ptr<CrossTimer>;
  CrossTimer();
  virtual ~CrossTimer();

  void Initialize(const std::string& thread_name);
  uint64_t Start(int32_t interval, CrossTimerTask&& task);
  void Stop(uint64_t timer_id);
protected:
  virtual void OnStartTimer(IDbg::CrossMessage::Ptr msg) = 0;
  virtual void OnStopTimer(IDbg::CrossMessage::Ptr msg) = 0;
  
protected:
  uint64_t AddTimer(int32_t interval, CrossTimerTask&& routine);
  TimerInfo::Ptr GetTimer(uint64_t timer_id);
  void EraseTimer(uint64_t timer_id);
  void OnTimer(uint64_t timer_id);
  
protected:
  IDbg::CrossHandlerThread::Ptr handlerThread_;
  CrossHandler::Ptr handler_;
  std::mutex timer_mutex_;
  std::unordered_map<uint64_t, TimerInfo::Ptr> id_to_timer_;
};

IDBG_API CrossTimer::Ptr CreateTimer(const std::string& thread_name);

class IdManager : public Singleton<IdManager> {
  friend class Singleton<IdManager>;
public:
  uint64_t Get();
private:
  std::mutex mutex_;
  uint64_t alloc_id_ = 0;
};

}

#endif 
