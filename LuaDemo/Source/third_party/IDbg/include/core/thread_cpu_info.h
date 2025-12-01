//
//  Created by mjzheng on 2019/6/28.
//  Copyright © 2020年 mjzheng. All rights reserved.
//

#ifndef IDBG_CORE_THREAD_CPU_INFO_H_
#define IDBG_CORE_THREAD_CPU_INFO_H_

#include "build_config.h"
#if defined(OS_IOS_IDBG) || defined(OS_OSX_IDBG)
#include <mach/mach.h>
#endif
#include <vector>
#include <string>
#include <memory>
#include <map>

namespace IDbg {

#if defined(OS_IOS_IDBG) || defined(OS_OSX_IDBG)
using ThreadID = thread_t;
#elif defined(OS_ANDROID_IDBG)
using ThreadID = uint32_t;
#elif defined(OS_WIN_IDBG)
#include<Windows.h>
using ThreadID = DWORD;
#endif
using ThreadIdArray = std::vector<ThreadID>;

struct IDBG_API FrameInfo {
  using Ptr = std::shared_ptr<FrameInfo>;
  int index;
  uintptr_t module_base;
  uintptr_t address;
  uintptr_t offset;
  std::string module_name;
  std::string func_name;
  FrameInfo() {
    index = 0;
    module_base = 0;
    address = 0;
    offset = 0;
  }
};

using FrameList = std::vector<FrameInfo::Ptr>;

struct IDBG_API ThreadStack {
  using Ptr = std::shared_ptr<ThreadStack>;
  ThreadID th;
  float cpu;
  std::string name;
  FrameList frames;
  ThreadStack(ThreadID thread_id) {
    th = thread_id;
    cpu = 0.0f;
  }
};

using ThreadStackArray = std::vector<ThreadStack::Ptr>;
using ThreadStackSlices = std::vector<ThreadStackArray>;

enum class ThreadOptions : uint64_t {
  kBasic = 1 << 0,  // 0000 0001
  kFrames = 1 << 1,
};

/*
 get all thread id
 */
IDBG_API ThreadIdArray GetAllThreadId();

/*
 get all thread stack, thread stack is store in ls
 */
IDBG_API ThreadStackArray GetAllThreadInfo(const ThreadOptions options);

/*
 get some thread stack, threa stack is store in ls
 id_ls: thread id list
 */
IDBG_API ThreadStackArray GetThreadInfoById(const ThreadOptions options, const ThreadIdArray &id_ls);

/*
 
 */
IDBG_API ThreadStack::Ptr GetThreadInfo(const ThreadOptions options, const ThreadID thread_id);

// calc app cpu
IDBG_API float CalAppCpu(const ThreadStackArray& ls);

}  // namespace IDbg

#endif  // IDBG_CORE_THREAD_CPU_INFO_H_
