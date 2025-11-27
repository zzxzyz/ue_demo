#ifndef CROSS_MESSAGE_H_
#define CROSS_MESSAGE_H_

#include <stdint.h>
#include <memory>
#include <queue>
#include <mutex>
#include "build_config.h"
#include "base/singleton.h"

#if defined(OS_WIN_IDBG)
#include "monitor/win/framework.h"
#endif

namespace IDbg {

class CrossHandler;

enum class CrossMessageType : int32_t {
  CM_INVALID = 0,
  CM_QUIT = 1,
  CM_TIMER = 2,
  CM_START_TIMER = 3,
  CM_STOP_TIMER = 4,
};

class IDBG_API CrossMessage {
public:
  CrossMessage(CrossMessageType msg_id, uint64_t arg1, uint64_t* arg2);
public:
  using Ptr = std::shared_ptr<CrossMessage>;
  CrossMessageType msg_id = CrossMessageType::CM_INVALID;
  uint64_t arg1 = 0;
  uint64_t* arg2 = 0;
  std::shared_ptr<CrossHandler> target;
#if defined(OS_WIN_IDBG)
  HWND hWnd;
#endif
};


class CrossMessageManager : public IDbg::Singleton<CrossMessageManager> {
  friend class IDbg::Singleton<CrossMessageManager>;
public:
  void Recycle(CrossMessage::Ptr msg);
  
  CrossMessage::Ptr PopOne();
private:
  std::mutex mutex_;
  std::queue<CrossMessage::Ptr> pool_;
};

}  // namespace IDbg
#endif
