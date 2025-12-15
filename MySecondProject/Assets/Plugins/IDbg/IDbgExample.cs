using UnityEngine;
using IDbg;

/// <summary>
/// IDbg使用示例
/// </summary>
public class IDbgExample : MonoBehaviour
{
    void Start()
    {
        // 初始化IDbg
        IDbgLog.Initialize();

        // 检查DLL是否可用
        if (IDbgWrapper.IsAvailable())
        {
            Debug.Log("IDbg库加载成功");

            // 获取系统信息
            Debug.Log($"CPU核心数: {IDbgWrapper.GetCpuCore()}");
            Debug.Log($"进程ID: {IDbgWrapper.GetProcessId()}");
            Debug.Log($"父进程ID: {IDbgWrapper.GetProcessParentId()}");

            // 获取性能信息
            float appMemory = IDbgWrapper.GetAppMemory();
            float sysMemory = IDbgWrapper.GetSysMemory();
            float appCpu = IDbgWrapper.GetAppCpu();
            float sysCpu = IDbgWrapper.GetSysCpu();

            Debug.Log($"应用内存: {appMemory} MB");
            Debug.Log($"系统内存: {sysMemory} MB");
            Debug.Log($"应用CPU: {appCpu}%");
            Debug.Log($"系统CPU: {sysCpu}%");

            // 打印版本信息
            IDbgWrapper.PrintVersion();
        }
        else
        {
            Debug.LogError("IDbg库加载失败，请检查DLL是否正确放置");
        }
    }

    void OnDestroy()
    {
        // 清理资源
        IDbgLog.Cleanup();
    }

    void Update()
    {
        // 每帧可以获取性能信息
        if (Time.frameCount % 60 == 0) // 每60帧输出一次
        {
            if (IDbgWrapper.IsAvailable())
            {
                float appCpu = IDbgWrapper.GetAppCpu();
                float appMemory = IDbgWrapper.GetAppMemory();
                Debug.Log($"[性能监控] CPU: {appCpu}%, 内存: {appMemory} MB");
            }
        }
    }
}

