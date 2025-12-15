using UnityEngine;
using IDbg;

/// <summary>
/// IDbg 库初始化器
/// 在游戏启动时自动初始化 IDbg 库
/// 使用 RuntimeInitializeOnLoadMethod 自动执行，无需挂载到 GameObject
/// </summary>
public static class IDbgInitializer
{
    private static bool isInitialized = false;
    private static IDbgWrapper.LogDelegate logDelegate = null;

    /// <summary>
    /// 在场景加载前自动初始化 IDbg
    /// </summary>
    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
    static void InitializeIDbg()
    {
        if (isInitialized)
        {
            return;
        }

        Debug.Log("[IDbgInitializer] 开始初始化 IDbg 库...");

        // 检查 DLL 是否可用
        if (!IDbgWrapper.IsAvailable())
        {
            Debug.LogWarning("[IDbgInitializer] IDbg.dll 不可用，跳过初始化");
            return;
        }

        // 设置日志回调
        logDelegate = (level, msg) =>
        {
            string logMessage = $"[IDbg {level}] {msg}";
            switch (level)
            {
                case IDbgWrapper.LogLevel.kError:
                    Debug.LogError(logMessage);
                    break;
                case IDbgWrapper.LogLevel.kWarning:
                    Debug.LogWarning(logMessage);
                    break;
                case IDbgWrapper.LogLevel.kInfo:
                case IDbgWrapper.LogLevel.kDebug:
                case IDbgWrapper.LogLevel.kPerf:
                default:
                    Debug.Log(logMessage);
                    break;
            }
        };

        try
        {
            IDbgWrapper.IDbg_Log_SetDelegate(logDelegate);
            IDbgWrapper.IDbg_Log_SetVerbosity(IDbgWrapper.LogLevel.kInfo);

            // 初始化 IDbg
            if (IDbgWrapper.Initialize())
            {
                isInitialized = true;
                Debug.Log("[IDbgInitializer] IDbg 库初始化成功");
                
                // 注册应用退出时的清理回调
                Application.quitting += CleanupIDbg;
            }
            else
            {
                Debug.LogError("[IDbgInitializer] IDbg 库初始化失败");
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError($"[IDbgInitializer] 初始化 IDbg 时出错: {e.Message}");
            Debug.LogException(e);
        }
    }

    static void CleanupIDbg()
    {
        if (isInitialized)
        {
            try
            {
                IDbgWrapper.IDbg_Cleanup();
                Debug.Log("[IDbgInitializer] IDbg 库已清理");
                isInitialized = false;
            }
            catch (System.Exception e)
            {
                Debug.LogError($"[IDbgInitializer] 清理 IDbg 时出错: {e.Message}");
            }
        }
    }
}

