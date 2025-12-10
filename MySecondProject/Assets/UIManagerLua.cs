using UnityEngine;
using SLua;
using System.Collections;

/// <summary>
/// UIManager 的 Lua 适配脚本
/// 用于在 Lua 中访问 UIManager 的功能
/// </summary>
public class UIManagerLua : MonoBehaviour
{
    void Start()
    {
        // 访问 Instance 会自动初始化 UIManager 和创建 Canvas
        // 不需要手动调用 CreateUICanvas()，因为它会在 Instance getter 中自动调用
        
        // 延迟一帧，确保 Lua 已初始化
        StartCoroutine(RegisterLuaFunctions());
    }
    
    IEnumerator RegisterLuaFunctions()
    {
        // 等待 Lua 初始化完成
        int maxWaitFrames = 10;
        int waitFrames = 0;
        while (LuaSvr.mainState == null && waitFrames < maxWaitFrames)
        {
            yield return null;
            waitFrames++;
        }
        
        if (LuaSvr.mainState == null)
        {
            Debug.LogWarning("UIManagerLua: Lua 未初始化");
            yield break;
        }
        
        // 注册 Lua 函数
        try
        {
            LuaSvr.mainState.doString(@"
                -- UIManager Lua 接口
                UIManager = {}
                
                function UIManager.ShowLoginPanel()
                    local manager = UnityEngine.GameObject.Find('UIManager'):GetComponent('UIManager')
                    if manager then
                        manager:ShowLoginPanel()
                    end
                end
                
                function UIManager.HideLoginPanel()
                    local manager = UnityEngine.GameObject.Find('UIManager'):GetComponent('UIManager')
                    if manager then
                        manager:HideLoginPanel()
                    end
                end
                
                function UIManager.ShowUI(uiName, prefab)
                    local manager = UnityEngine.GameObject.Find('UIManager'):GetComponent('UIManager')
                    if manager then
                        manager:ShowUI(uiName, prefab)
                    end
                end
                
                function UIManager.HideUI(uiName)
                    local manager = UnityEngine.GameObject.Find('UIManager'):GetComponent('UIManager')
                    if manager then
                        manager:HideUI(uiName)
                    end
                end
                
                function UIManager.GetUI(uiName)
                    local manager = UnityEngine.GameObject.Find('UIManager'):GetComponent('UIManager')
                    if manager then
                        return manager:GetUI(uiName)
                    end
                    return nil
                end
                
                function UIManager.IsUIVisible(uiName)
                    local manager = UnityEngine.GameObject.Find('UIManager'):GetComponent('UIManager')
                    if manager then
                        return manager:IsUIVisible(uiName)
                    end
                    return false
                end
            ");
            Debug.Log("UIManagerLua: Lua 函数已注册");
        }
        catch (System.Exception e)
        {
            Debug.LogError("UIManagerLua: 注册 Lua 函数失败: " + e.Message);
        }
    }
}

