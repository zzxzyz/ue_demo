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

        /// <summary>
        /// 初始化 IDbg 库
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern int IDbg_Initialize();

        /// <summary>
        /// 清理 IDbg 库
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void IDbg_Cleanup();

        // ========== 日志相关 ==========
        /// <summary>
        /// 设置日志委托
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void IDbg_Log_SetDelegate(LogDelegate delegateFunc);

        /// <summary>
        /// 设置日志详细程度
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void IDbg_Log_SetVerbosity(LogLevel verbosity);

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

        // ========== 辅助方法 ==========
        /// <summary>
        /// 检查 DLL 是否可用
        /// </summary>
        public static bool IsAvailable()
        {
            try
            {
                // 尝试调用一个简单的函数来检查 DLL 是否加载成功
                IDbg_Log_SetVerbosity(LogLevel.kInfo);
                return true;
            }
            catch (DllNotFoundException)
            {
                Debug.LogWarning("[IDbg] IDbg.dll 未找到，请确保 DLL 文件在正确的位置");
                return false;
            }
            catch (Exception e)
            {
                Debug.LogError($"[IDbg] 检查 DLL 可用性时出错: {e.Message}");
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
            catch (Exception e)
            {
                Debug.LogError($"[IDbg] 初始化时出错: {e.Message}");
                return false;
            }
        }
    }
}

