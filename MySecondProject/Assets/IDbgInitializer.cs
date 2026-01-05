using UnityEngine;
#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
using IDbg;
#endif

/// <summary>
/// IDbg 库初始化器
/// 在游戏启动时自动初始化 IDbg 库
/// 使用 RuntimeInitializeOnLoadMethod 自动执行，无需挂载到 GameObject
/// </summary>
public static class IDbgInitializer
{
    #if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
    private static bool isInitialized = false;
    private static IDbgWrapper.LogDelegate logDelegate = null;
    #endif

    /// <summary>
    /// 在场景加载前自动初始化 IDbg
    /// </summary>
    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
    static void InitializeIDbg()
    {
        #if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
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
            // 先初始化 IDbg
            if (IDbgWrapper.Initialize())
            {
                // 初始化成功后再设置日志回调（使用新的C API）
                try
                {
                    IDbgWrapper.IDbg_Log_SetDelegate(logDelegate);
                    Debug.Log("[IDbgInitializer] 日志委托设置成功");
                }
                catch (System.EntryPointNotFoundException e)
                {
                    Debug.LogWarning($"[IDbgInitializer] 设置日志委托失败: 函数未找到 - {e.Message}");
                    Debug.LogWarning("[IDbgInitializer] 提示: 请确保使用最新版本的 IDbg.dll，包含 C API 接口");
                }
                catch (System.Exception e)
                {
                    Debug.LogWarning($"[IDbgInitializer] 设置日志委托失败: {e.Message}");
                }

                // 设置日志详细程度
                try
                {
                    IDbgWrapper.IDbg_Log_SetVerbosity(IDbgWrapper.LogLevel.kInfo);
                }
                catch (System.EntryPointNotFoundException e)
                {
                    Debug.LogWarning($"[IDbgInitializer] 设置日志详细程度失败: 函数未找到 - {e.Message}");
                    Debug.LogWarning("[IDbgInitializer] 提示: 请确保使用最新版本的 IDbg.dll，包含 C API 接口");
                }
                catch (System.Exception e)
                {
                    Debug.LogWarning($"[IDbgInitializer] 设置日志详细程度失败: {e.Message}");
                }

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
        catch (System.EntryPointNotFoundException e)
        {
            Debug.LogError($"[IDbgInitializer] 初始化 IDbg 时出错: 函数未找到 - {e.Message}");
            Debug.LogError("[IDbgInitializer] 提示: 使用 dumpbin /exports IDbg.dll 查看实际的导出函数名");
        }
        catch (System.DllNotFoundException e)
        {
            Debug.LogError($"[IDbgInitializer] 初始化 IDbg 时出错: DLL 未找到 - {e.Message}");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"[IDbgInitializer] 初始化 IDbg 时出错: {e.Message}");
            Debug.LogException(e);
        }
        #else
        Debug.Log("[IDbgInitializer] 非 Windows 平台，跳过 IDbg 初始化");
        #endif
    }

    static void CleanupIDbg()
    {
        #if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
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
        #endif
    }
}

