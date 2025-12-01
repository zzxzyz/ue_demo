//
//  WeMeetQAPM.h
//  WeMeet
//
//  Created by mjzheng on 2023/04/17.
//  Copyright © 2023 tencent. All rights reserved.
//

#include <memory>
#include <vector>
#include "thread/cross_handler_thread.h"
#include "thread/cross_handler.h"
#include "thread/cross_timer.h"

class LoopThreadTest: public std::enable_shared_from_this<LoopThreadTest> {
public:
  using Ptr = std::shared_ptr<LoopThreadTest>;
  LoopThreadTest();
  void Start();
  
  void Stop();
private:
  IDbg::CrossTimer::Ptr timer_;
  uint64_t timer_id_;
  int32_t count_ = 0;
};
