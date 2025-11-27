#ifndef CROSS_HANDLER_THREAD_H_
#define CROSS_HANDLER_THREAD_H_

#include <thread>
#include <memory>
#include <condition_variable>
#include <mutex>
#include "build_config.h"
#include "thread/cross_looper.h"

namespace IDbg {

class IDBG_API CrossHandlerThread {
public:
  using Ptr = std::shared_ptr<CrossHandlerThread>;
  CrossHandlerThread(const std::string& name);
  ~CrossHandlerThread();
  CrossLooper::WeakPtr GetLooper();
private:
  bool Quit(bool safely);
  void Run();
  void SetLooper(CrossLooper::Ptr looper);
private:
  std::string thread_name_;
  std::unique_ptr<std::thread> th_;
  
  std::condition_variable cv_;
  std::mutex looper_mutex_;
  CrossLooper::Ptr looper_;
};

}  // namespace IDbg
#endif
