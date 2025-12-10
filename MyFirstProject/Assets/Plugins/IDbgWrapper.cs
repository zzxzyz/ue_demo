using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace IDbg
{
    /// <summary>
    /// IDbg库的C#包装类
    /// 基于third_party/IDbg/include中的头文件定义
    /// 
    /// 注意：C++ DLL的函数名会被name mangling修饰，EntryPoint需要使用修饰后的名称
    /// 可以使用dumpbin /EXPORTS IDbg.dll 或 Dependency Walker 查看实际的导出函数名
    /// </summary>
    public static class IDbgWrapper
    {
        private const string DLL_NAME = "IDbg";

        // ========== 枚举定义 ==========

        // 日志级别枚举（对应C++的LogLevel）
        public enum LogLevel
        {
            kPerf = 0,
            kError = 1,
            kWarning = 2,
            kInfo = 3,
            kDebug = 4
        }

        // 性能监控类型枚举（对应C++的PerfMonitorType）
        public enum PerfMonitorType : long
        {
            kPerfNone = 0,
            kHighCpu = 1 << 0,
            kHang = 1 << 1
        }

        // 监控状态枚举
        public enum MonitorStatus : uint
        {
            kStop = 0,
            kRunning = 1,
            kRestore = 2
        }

        // 应用状态枚举
        public enum AppState : int
        {
            kInBackground = 0,
            kInForeground = 1
        }

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

        /// <summary>
        /// 初始化环境（Windows平台）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?InitializeEnv@IDbg@@YAXXZ")]
        public static extern void InitializeEnv();

        /// <summary>
        /// 清理环境（Windows平台）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, EntryPoint = "?UninitializeEnv@IDbg@@YAXXZ")]
        public static extern void UninitializeEnv();

        // ========== 字符串返回函数（需要特殊处理） ==========
        // 注意：返回std::string的函数需要特殊处理，因为C++的std::string不能直接marshal
        // 如果DLL提供了C接口包装，可以使用以下方式：

        /// <summary>
        /// 获取当前日期字符串（需要DLL提供C接口）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr GetDateInternal();

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

        // ========== 工具方法 ==========

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
