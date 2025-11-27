#ifndef IDBG_CORE_CROSS_GPU_INFO_H_
#define IDBG_CORE_CROSS_GPU_INFO_H_

#include "build_config.h"
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace IDbg {

struct ImageInfo {
  using Ptr = std::shared_ptr<ImageInfo>;
  uintptr_t address;
  uintptr_t vm_address;
  uintptr_t size;
  std::string name;
  std::string full_path;
  std::string uuid;
  std::string architecture;
  std::string version;
  ImageInfo() {
    address = 0;
    vm_address = 0;
    size = 0;
  }
};

using ImageList = std::vector<ImageInfo::Ptr>;
using ImageMap = std::map<uint64_t, ImageInfo::Ptr>;

// get cpu physics core number
IDBG_API int GetCpuCore();

IDBG_API double GetAppGPU();

IDBG_API double GetSysGPU();

//
/*
 get application current memory usage in MB
 */
IDBG_API float GetAppMemory();

/*
 get system current memory usage in MB
 */
IDBG_API float GetSysMemory();

// get sys cpu
IDBG_API float GetSysCpu();

// get app cpu, max value is 100%*cpu core
IDBG_API float GetAppCpu(float* max_thread_cpu = nullptr);

IDBG_API ImageList GetImageList();

#if defined(OS_WIN_IDBG)
IDBG_API void InitializeEnv();
IDBG_API void UninitializeEnv();
#endif

}  // namespace IDbg

#endif
