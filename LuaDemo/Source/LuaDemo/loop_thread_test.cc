//
//  WeMeetQAPM.m
//  WeMeet
//
//  Created by mjzheng on 2023/04/17.
//  Copyright © 2023 tencent. All rights reserved.
//

#include "loop_thread_test.h"
#include "base/log.h"
#include "base/sys_util.h"
#include "thread/cross_message.h"
#include "Async/Async.h"
#include "Engine/Engine.h"
#include <cassert>

LoopThreadTest::LoopThreadTest() {
    LOG_INFO("loop thread");
}

void LoopThreadTest::Start() {
    IDbg::PrintVersion();
    timer_ = IDbg::CreateTimer("test_timer");
    timer_id_ = timer_->Start(2000, [this](uint64_t timer_id){
      UNUSED(timer_id);
      const int32 CurrentCount = ++count_;
      LOG_INFO("on timer %d", CurrentCount);
      AsyncTask(ENamedThreads::GameThread, [CurrentCount]() {
          if (GEngine)
          {
              const FString Message = FString::Printf(TEXT("Timer count: %d"), CurrentCount);
              GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, Message);
          }
      });
      return 1;
    });
}

void LoopThreadTest::Stop() {
    LOG_INFO("stop thread");
    if (timer_ != nullptr) {
        timer_->Stop(timer_id_);
        timer_.reset();
    }
}
