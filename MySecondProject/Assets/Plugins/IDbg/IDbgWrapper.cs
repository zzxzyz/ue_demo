using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace IDbg
{
    /// <summary>
    /// IDbg 原生库的 C# 包装类
    /// 用于调用 IDbg.dll 中的 C++ 函数
    /// </summary>
    public static class IDbgWrapper
    {
        private const string DLL_NAME = "IDbg";

        // ========== 枚举定义 ==========
        public enum LogLevel : int
        {
            kPerf = 0,
            kError = 1,
            kWarning = 2,
            kInfo = 3,
            kDebug = 4,
        }

        public enum PerfMonitorType : long
        {
            kPerfNone = 0,
            kHighCpu = 1 << 0,
            kHang = 1 << 1,
        }

        public enum MonitorStatus : uint
        {
            kStop = 0,
            kRunning = 1,
            kRestore = 2,
        }

        public enum AppState : int
        {
            kInBackground = 0,
            kInForeground = 1,
        }

        // ========== 委托定义 ==========
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void LogDelegate(LogLevel level, [MarshalAs(UnmanagedType.LPStr)] string msg);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void SampleDelegate(
            [MarshalAs(UnmanagedType.LPStr)] string exceptionType,
            [MarshalAs(UnmanagedType.LPStr)] string msg,
            IntPtr slices);

        // ========== DLL 导入声明 ==========
        // 注意：这些函数名需要根据实际的 DLL 导出函数名来调整
        // 如果 DLL 使用了 C++ name mangling，可能需要使用 EntryPoint 指定实际函数名

        // ========== 环境初始化相关 ==========
        /// <summary>
        /// 初始化 IDbg 环境（使用实际的 C++ mangled 函数名）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?InitializeEnv@IDbg@@YAXXZ")]
        public static extern void InitializeEnv();

        /// <summary>
        /// 清理 IDbg 环境（使用实际的 C++ mangled 函数名）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?UninitializeEnv@IDbg@@YAXXZ")]
        public static extern void UninitializeEnv();

        // ========== 基础系统工具函数 (base/sys_util.h) ==========
        /// <summary>
        /// 获取当前毫秒数
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetCurrentMsc@IDbg@@YA_KXZ")]
        public static extern ulong GetCurrentMsc();

        /// <summary>
        /// 打印版本信息
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?PrintVersion@IDbg@@YAXXZ")]
        public static extern void PrintVersion();

        /// <summary>
        /// 设置线程名称
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "?SetThreadName@IDbg@@YAXPEBD@Z")]
        public static extern void SetThreadName(string name);

        /// <summary>
        /// 获取进程ID
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetProcessId@IDbg@@YAHXZ")]
        public static extern int GetProcessId();

        /// <summary>
        /// 获取父进程ID
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetProcessParentId@IDbg@@YAHXZ")]
        public static extern int GetProcessParentId();

        /// <summary>
        /// 获取进程创建时间
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetProcessCreateTime@IDbg@@YA_KXZ")]
        public static extern ulong GetProcessCreateTime();

        // ========== Log 单例访问 ==========
        /// <summary>
        /// 获取 Log 单例实例（使用实际的 C++ mangled 函数名）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?Instance@?$Singleton@VLog@IDbg@@@IDbg@@SAPEAVLog@2@XZ")]
        private static extern IntPtr GetLogInstance();

        // ========== Log 类方法 ==========
        /// <summary>
        /// 设置日志详细程度（Log类的成员函数，需要Log实例指针）
        /// 注意：在 x64 平台上，C++ 成员函数使用统一的调用约定（类似 Cdecl）
        /// this 指针作为第一个参数传递
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?SetVerbosity@Log@IDbg@@QEAAXW4LogLevel@2@@Z")]
        private static extern void SetVerbosity(IntPtr logInstance, LogLevel verbosity);

        /// <summary>
        /// 设置日志委托（Log类的成员函数，需要Log实例指针）
        /// 注意：这个函数接受 std::function，但我们尝试传递函数指针
        /// 在 x64 平台上，std::function 可能可以接受函数指针
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?SetDelegate@Log@IDbg@@QEAAX$$QEAV?$function@$$A6AXW4LogLevel@IDbg@@PEBD@Z@std@@@Z")]
        private static extern void SetDelegate(IntPtr logInstance, IntPtr functionPtr);

        // 保持委托引用，防止被 GC 回收
        private static GCHandle logDelegateHandle;
        private static LogDelegate currentLogDelegate;

        // ========== 公共接口（保持向后兼容） ==========
        /// <summary>
        /// 初始化 IDbg 库（公共接口）
        /// </summary>
        public static int IDbg_Initialize()
        {
            try
            {
                InitializeEnv();
                return 0; // 成功
            }
            catch
            {
                return -1; // 失败
            }
        }

        /// <summary>
        /// 清理 IDbg 库（公共接口）
        /// </summary>
        public static void IDbg_Cleanup()
        {
            try
            {
                // 释放委托引用
                if (logDelegateHandle.IsAllocated)
                {
                    logDelegateHandle.Free();
                    currentLogDelegate = null;
                }

                UninitializeEnv();
            }
            catch (Exception e)
            {
                Debug.LogError($"[IDbg] Cleanup 时出错: {e.Message}");
            }
        }

        /// <summary>
        /// 设置日志详细程度（公共接口）
        /// </summary>
        public static void IDbg_Log_SetVerbosity(LogLevel verbosity)
        {
            try
            {
                IntPtr logInstance = GetLogInstance();
                if (logInstance != IntPtr.Zero)
                {
                    SetVerbosity(logInstance, verbosity);
                }
                else
                {
                    Debug.LogWarning("[IDbg] 无法获取 Log 实例");
                }
            }
            catch (Exception e)
            {
                Debug.LogError($"[IDbg] 设置日志详细程度时出错: {e.Message}");
            }
        }

        /// <summary>
        /// 设置日志委托（公共接口）
        /// 注意：由于 C++ std::function 的复杂性，这个函数尝试使用函数指针
        /// </summary>
        public static void IDbg_Log_SetDelegate(LogDelegate delegateFunc)
        {
            try
            {
                IntPtr logInstance = GetLogInstance();
                if (logInstance == IntPtr.Zero)
                {
                    Debug.LogWarning("[IDbg] 无法获取 Log 实例");
                    return;
                }

                // 释放之前的委托引用（如果存在）
                if (logDelegateHandle.IsAllocated)
                {
                    logDelegateHandle.Free();
                }

                // 保存委托引用，防止被 GC 回收
                currentLogDelegate = delegateFunc;
                logDelegateHandle = GCHandle.Alloc(currentLogDelegate);

                // 获取委托的函数指针
                IntPtr functionPtr = Marshal.GetFunctionPointerForDelegate(currentLogDelegate);

                // 尝试调用 SetDelegate
                // 注意：这可能在 C++ 端不工作，因为 std::function 需要特殊构造
                // 但如果 C++ 端有接受函数指针的构造函数，这可能可以工作
                SetDelegate(logInstance, functionPtr);
            }
            catch (EntryPointNotFoundException e)
            {
                Debug.LogWarning($"[IDbg] SetDelegate 函数未找到: {e.Message}");
                Debug.LogWarning("[IDbg] 提示: 可能需要 C 包装器 DLL 来桥接 C# 委托和 C++ std::function");
            }
            catch (Exception e)
            {
                Debug.LogError($"[IDbg] 设置日志委托时出错: {e.Message}");
                Debug.LogException(e);
                Debug.LogWarning("[IDbg] 提示: C++ std::function 不能直接从 C# 传递，可能需要 C 包装器 DLL");
            }
        }

        // ========== 性能信息函数 (core/basic_perf_info.h) ==========
        /// <summary>
        /// 获取CPU物理核心数
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetCpuCore@IDbg@@YAHXZ")]
        public static extern int GetCpuCore();

        /// <summary>
        /// 获取应用GPU使用率
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetAppGPU@IDbg@@YANXZ")]
        public static extern double GetAppGPU();

        /// <summary>
        /// 获取系统GPU使用率
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetSysGPU@IDbg@@YANXZ")]
        public static extern double GetSysGPU();

        /// <summary>
        /// 获取应用内存使用量(MB)
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetAppMemory@IDbg@@YAMXZ")]
        public static extern float GetAppMemory();

        /// <summary>
        /// 获取系统内存使用量(MB)
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetSysMemory@IDbg@@YAMXZ")]
        public static extern float GetSysMemory();

        /// <summary>
        /// 获取系统CPU使用率
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetSysCpu@IDbg@@YAMXZ")]
        public static extern float GetSysCpu();

        /// <summary>
        /// 获取应用CPU使用率
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?GetAppCpu@IDbg@@YAMPEAM@Z")]
        public static extern float GetAppCpu(ref float maxThreadCpu);

        /// <summary>
        /// 获取应用CPU使用率（不返回最大线程CPU）
        /// </summary>
        public static float GetAppCpu()
        {
            float maxThreadCpu = 0;
            // 使用局部变量避免 ref/in 参数问题
            return GetAppCpu(ref maxThreadCpu);
        }

        // ========== 性能监控相关 ==========
        /// <summary>
        /// 创建性能监控器
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr IDbg_CreatePerfMonitor(PerfMonitorType monitorType, IntPtr config);

        /// <summary>
        /// 启动性能监控
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void IDbg_PerfMonitor_Start(IntPtr monitor);

        /// <summary>
        /// 停止性能监控
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void IDbg_PerfMonitor_Stop(IntPtr monitor);

        /// <summary>
        /// 刷新性能数据
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void IDbg_PerfMonitor_Flush(IntPtr monitor);

        // ========== 字符串返回函数（需要特殊处理） ==========
        // 注意：返回std::string的函数需要特殊处理，因为C++的std::string不能直接marshal
        // 如果DLL提供了C接口包装，可以使用以下方式：

        /// <summary>
        /// 获取当前日期字符串（需要DLL提供C接口）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr GetDateInternal();

        /// <summary>
        /// 获取当前日期字符串（公共接口）
        /// </summary>
        public static string GetDate()
        {
            try
            {
                IntPtr ptr = GetDateInternal();
                if (ptr == IntPtr.Zero) return string.Empty;
                string result = Marshal.PtrToStringAnsi(ptr);
                // 注意：如果DLL内部分配了内存，可能需要调用释放函数
                return result ?? string.Empty;
            }
            catch
            {
                return string.Empty;
            }
        }

        // ========== 辅助方法 ==========
        /// <summary>
        /// 尝试加载DLL并检查是否可用
        /// </summary>
        public static bool IsAvailable()
        {
            try
            {
                // 尝试调用一个简单的函数来验证DLL是否加载成功
                int cpuCore = GetCpuCore();
                return cpuCore > 0;
            }
            catch (Exception e)
            {
                Debug.LogWarning($"IDbg DLL不可用: {e.Message}");
                return false;
            }
        }

        /// <summary>
        /// 初始化 IDbg（带错误处理）
        /// </summary>
        public static bool Initialize()
        {
            try
            {
                if (!IsAvailable())
                {
                    return false;
                }

                int result = IDbg_Initialize();
                if (result == 0)
                {
                    Debug.Log("[IDbg] 初始化成功");
                    return true;
                }
                else
                {
                    Debug.LogError($"[IDbg] 初始化失败，错误代码: {result}");
                    return false;
                }
            }
            catch (EntryPointNotFoundException e)
            {
                Debug.LogError($"[IDbg] 初始化时出错: 函数未找到 - {e.Message}。请检查 DLL 导出函数名是否正确。");
                Debug.LogError("[IDbg] 提示: 使用 dumpbin /exports IDbg.dll 查看实际的导出函数名");
                return false;
            }
            catch (DllNotFoundException e)
            {
                Debug.LogError($"[IDbg] 初始化时出错: DLL 未找到 - {e.Message}");
                return false;
            }
            catch (Exception e)
            {
                Debug.LogError($"[IDbg] 初始化时出错: {e.Message}");
                Debug.LogException(e);
                return false;
            }
        }
    }

    /// <summary>
    /// IDbg日志辅助类
    /// </summary>
    public static class IDbgLog
    {
        private static bool initialized = false;

        /// <summary>
        /// 初始化IDbg日志系统
        /// </summary>
        public static void Initialize()
        {
            if (initialized)
                return;

            try
            {
                // Windows平台尝试初始化环境（可选，失败不影响其他功能）
                #if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
                try
                {
                    IDbgWrapper.InitializeEnv();
                    Debug.Log("IDbg环境初始化成功");
                }
                catch (DllNotFoundException)
                {
                    Debug.LogWarning("IDbg DLL未找到，跳过环境初始化");
                }
                catch (EntryPointNotFoundException)
                {
                    // InitializeEnv函数可能不存在或被name mangling，这是正常的
                    Debug.LogWarning("IDbg InitializeEnv函数未找到（可能被name mangling），跳过环境初始化");
                }
                catch (Exception e)
                {
                    Debug.LogWarning($"IDbg环境初始化失败（非致命）: {e.Message}");
                }
                #endif
                
                initialized = true;
                Debug.Log("IDbg日志系统初始化成功");
            }
            catch (Exception e)
            {
                Debug.LogError($"IDbg日志系统初始化失败: {e.Message}");
            }
        }

        /// <summary>
        /// 清理IDbg日志系统
        /// </summary>
        public static void Cleanup()
        {
            if (!initialized)
                return;

            try
            {
                #if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
                try
                {
                    IDbgWrapper.UninitializeEnv();
                }
                catch (EntryPointNotFoundException)
                {
                    // UninitializeEnv函数可能不存在或被name mangling，这是正常的
                    Debug.LogWarning("IDbg UninitializeEnv函数未找到，跳过清理");
                }
                catch (Exception e)
                {
                    Debug.LogWarning($"IDbg环境清理失败（非致命）: {e.Message}");
                }
                #endif
                
                initialized = false;
                Debug.Log("IDbg日志系统已清理");
            }
            catch (Exception e)
            {
                Debug.LogWarning($"IDbg日志系统清理失败: {e.Message}");
                initialized = false; // 即使清理失败也标记为未初始化
            }
        }
    }
}

