#ifndef CROSS_HANDLER_H_
#define CROSS_HANDLER_H_

#include <functional>
#include "build_config.h"
#include "cross_looper.h"
#include "cross_message.h"

namespace IDbg {

using MessageProc = std::function<void(CrossMessage::Ptr)>;

class IDBG_API CrossHandler : public std::enable_shared_from_this<CrossHandler> {
public:
  using Ptr = std::shared_ptr<CrossHandler>;
  virtual ~CrossHandler();
  CrossHandler(MessageProc&& proc);
  CrossHandler(CrossLooper::WeakPtr looper, MessageProc&& proc);
  void DispatchCrossMessage(CrossMessage::Ptr msg);
  bool PostCrossMessage(CrossMessage::Ptr msg);
  bool PostCrossMessage(CrossMessageType what, uint64_t arg1, uint64_t* arg2);
  
private:
  CrossLooper::WeakPtr looper_;
  MessageProc proc_;
};

}  // namespace IDbg
#endif
