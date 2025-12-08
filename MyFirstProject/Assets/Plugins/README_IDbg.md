# IDbg库在Unity中的使用说明

## 目录结构

```
Assets/
  Plugins/
    IDbg.dll          # IDbg的Windows DLL文件
    IDbgWrapper.cs    # C#包装类
    IDbgExample.cs    # 使用示例
```

## 快速开始

1. **DLL已放置在正确位置**：`Assets/Plugins/IDbg.dll`

2. **在代码中使用**：
```csharp
using IDbg;
using UnityEngine;

public class MyScript : MonoBehaviour
{
    void Start()
    {
        // 初始化IDbg（Windows平台需要）
        IDbgLog.Initialize();
        
        // 检查DLL是否可用
        if (IDbgWrapper.IsAvailable())
        {
            // 获取系统信息
            int cpuCore = IDbgWrapper.GetCpuCore();
            int processId = IDbgWrapper.GetProcessId();
            
            // 获取性能信息
            float appMemory = IDbgWrapper.GetAppMemory();
            float appCpu = IDbgWrapper.GetAppCpu();
            
            Debug.Log($"CPU核心数: {cpuCore}, 进程ID: {processId}");
            Debug.Log($"应用内存: {appMemory} MB, CPU: {appCpu}%");
        }
    }
    
    void OnDestroy()
    {
        IDbgLog.Cleanup();
    }
}
```

## 注意事项

### C++ Name Mangling问题

IDbg是C++库，函数名会被name mangling修饰。如果直接使用函数名无法找到，需要：

1. **查找实际的导出函数名**：
   ```bash
   # 使用dumpbin（Visual Studio工具）
   dumpbin /EXPORTS Assets/Plugins/IDbg.dll
   
   # 或使用Dependency Walker查看
   ```

2. **在DllImport中指定EntryPoint**：
   ```csharp
   [DllImport("IDbg", EntryPoint = "?GetCpuCore@IDbg@@YAHXZ", CallingConvention = CallingConvention.Cdecl)]
   public static extern int GetCpuCore();
   ```

3. **如果函数名不匹配**，可能需要：
   - 检查头文件中的函数签名
   - 确认DLL的调用约定（Cdecl/StdCall）
   - 使用工具反编译查看实际的导出名称

### 字符串返回函数

返回`std::string`的函数需要特殊处理。如果DLL提供了C接口包装，可以使用`IntPtr`接收，然后转换为C#字符串：

```csharp
[DllImport("IDbg", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
private static extern IntPtr GetProcessNameInternal();

public static string GetProcessName()
{
    IntPtr ptr = GetProcessNameInternal();
    if (ptr == IntPtr.Zero) return string.Empty;
    return Marshal.PtrToStringAnsi(ptr) ?? string.Empty;
}
```

## 已实现的函数

### 基础系统工具 (base/sys_util.h)
- `GetCurrentMsc()` - 获取当前毫秒数
- `PrintVersion()` - 打印版本信息
- `SetThreadName(string)` - 设置线程名称
- `GetProcessId()` - 获取进程ID
- `GetProcessParentId()` - 获取父进程ID
- `GetProcessCreateTime()` - 获取进程创建时间

### 性能信息 (core/basic_perf_info.h)
- `GetCpuCore()` - 获取CPU物理核心数
- `GetAppGPU()` - 获取应用GPU使用率
- `GetSysGPU()` - 获取系统GPU使用率
- `GetAppMemory()` - 获取应用内存使用量(MB)
- `GetSysMemory()` - 获取系统内存使用量(MB)
- `GetSysCpu()` - 获取系统CPU使用率
- `GetAppCpu()` - 获取应用CPU使用率
- `InitializeEnv()` - 初始化环境（Windows）
- `UninitializeEnv()` - 清理环境（Windows）

## 故障排查

### DLL加载失败
1. 确认`IDbg.dll`在`Assets/Plugins/`目录下
2. 确认DLL的平台设置正确（Windows x64/x86）
3. 检查是否有依赖的DLL缺失

### 函数调用失败（EntryPointNotFoundException）
1. 使用`dumpbin /EXPORTS IDbg.dll`查看实际导出函数名
2. 在`DllImport`中添加正确的`EntryPoint`
3. 确认调用约定（`CallingConvention`）正确

### 字符串函数返回乱码
1. 确认字符集设置（`CharSet.Ansi`或`CharSet.Unicode`）
2. 检查内存管理（是否需要释放返回的字符串）

## 参考文档

- Unity Native Plugins: https://docs.unity3d.com/Manual/NativePlugins.html
- P/Invoke文档: https://www.pinvoke.net/

