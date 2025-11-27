#ifndef CROSS_LOOPER_H_
#define CROSS_LOOPER_H_

#include <unordered_map>
#include <mutex>
#include <memory>
#include <thread>
#include "base/singleton.h"
#include "cross_message.h"
#include "cross_message_queue.h"

namespace IDbg {

using CrossThreadID = std::thread::id;

class IDBG_API CrossLooper {
public:
  using Ptr = std::shared_ptr<CrossLooper>;
  using WeakPtr = std::weak_ptr<CrossLooper>;
  static void Prepare();
  static void Loop();
  static void Exit();
  static CrossLooper::Ptr GetCurrentLooper();
public:
  CrossLooper() = default;
  virtual ~CrossLooper() = default;
  virtual void Quit(bool safely) = 0;
  virtual void MessageLoop() = 0;
  virtual void PostCrossMessage(CrossMessage::Ptr msg) = 0;
};

IDBG_API CrossLooper::Ptr CreateLooper();

class CrossLooperManager : public IDbg::Singleton<CrossLooperManager> {
public:
  friend class IDbg::Singleton<CrossLooperManager>;
  CrossLooper::Ptr Create(CrossThreadID tid);
  CrossLooper::Ptr Get(CrossThreadID tid);
  void Remove(CrossThreadID tid);
  
private:
  CrossLooperManager() = default;
  ~CrossLooperManager();
  
private:
  std::mutex loopers_mutex_;
  std::unordered_map<CrossThreadID, CrossLooper::Ptr> loopers_;
};

}  // namespace IDbg
#endif
