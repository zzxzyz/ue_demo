#ifndef CROSS_MESSAGE_QUEUE_H_
#define CROSS_MESSAGE_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <list>
#include <memory>
#include "cross_message.h"

namespace IDbg {

class CrossMessageQueue {
public:
  using Ptr = std::shared_ptr<CrossMessageQueue>;
  
  void PushBack(CrossMessage::Ptr msg);
  void PushFront(CrossMessage::Ptr msg);
  CrossMessage::Ptr Take();
  void Clear();
  
private:
  std::mutex queue_mutex_;
  std::condition_variable cv_;
  std::list<CrossMessage::Ptr> queue_;
};

}  // namespace IDbg
#endif
