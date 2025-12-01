//
//  Created by mjzheng on 2021/4/13.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_CORE_MINI_DUMP_FILE_H_
#define IDBG_CORE_MINI_DUMP_FILE_H_

#include <stdint.h>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "build_config.h"
#include "thread_cpu_info.h"
#include "basic_perf_info.h"

namespace IDbg {

enum DumpOptions : uint32_t {
  kHeader =  1 << 0,   // 0000 0001
  kStack =  1 << 1,   // 0000 0010
  kImage =  1 << 2,   // 0000 0100
  kDefault = kHeader | kStack | kImage,
};

struct FrameNode {
  using Ptr = std::shared_ptr<FrameNode>;
  uint32_t level = 0;
  uint32_t sample_count = 0;
  FrameInfo::Ptr value;
  std::vector<FrameNode::Ptr> childs;
  FrameNode(const uint32_t idx, const uint32_t count, FrameInfo::Ptr info) {
    level = idx;
    sample_count = count;
    value = info;
  }
};

struct FrameRootNode {
  using Ptr = std::shared_ptr<FrameRootNode>;
  ThreadID th;
  //float cpu;
  std::string name;
  FrameNode::Ptr root;
  FrameRootNode(ThreadID thread_id, const std::string& thread_name, FrameNode::Ptr thread_root) {
    th = thread_id;
    name = thread_name;
    root = thread_root;
  }
};

using FrameForest = std::map<ThreadID, FrameRootNode::Ptr>;
using StringArray = std::vector<std::string>;
using StringSet = std::set<std::string>;

/*
 save data to file
 file name is like 'stack_20211220153059.log'
 */
IDBG_API void SaveFileToPath(const std::string& file_name, const std::string& data);

//
IDBG_API std::string GenerateFileName();

/*
 */
IDBG_API std::string GetFmtHeader();

/*
 */
IDBG_API std::string GetExceptionNote(const std::string& msg = "");

IDBG_API StringSet GetModuleList(const ThreadStackSlices& slices);

IDBG_API FrameForest SlicesToForest(const ThreadStackSlices& slices);

IDBG_API StringArray FormatForest(const FrameForest& forest);

IDBG_API ThreadStackArray SlicesToList(const ThreadStackSlices& slices);

IDBG_API StringArray FormatThreadStatck(const ThreadStackArray& ls);

/*
 
 */
IDBG_API std::string GetFmtBinaryImages(const ImageList& ls);

IDBG_API std::string JoinStringArray(const StringArray& ls, const char* split);

IDBG_API std::string GenerateMiniDump(const IDbg::ThreadStackSlices& slices, const char* msg);

}  // namespace IDbg

#endif  // IDBG_CORE_MINI_DUMP_FILE_H_
