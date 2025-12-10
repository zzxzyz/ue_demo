using UnityEngine;
using SLua;
using System.Collections;

/// <summary>
/// Lua LoginManager 的 C# 辅助脚本
/// 用于在场景中初始化 Lua 版本的 LoginManager
/// 
/// 使用方法：
/// 1. 将此脚本挂载到场景中的任意 GameObject 上（建议挂载到 LoginPanel 或一个专门的 Manager 对象）
/// 2. 确保场景中存在名为 "LoginPanel" 的 GameObject
/// 3. 确保 LoginPanel 下有正确的子对象：UsernameInput, PasswordInput, LoginButton, TitleText
/// 4. 运行场景，Lua LoginManager 会自动初始化并绑定 UI 组件
/// 
/// 注意：如果使用 LuaStarter 加载 login_ui，此脚本会在场景加载完成后确保 LoginManager 正确初始化
/// </summary>
public class LoginManagerLua : MonoBehaviour
{
    void Start()
    {
        // 延迟一帧执行，确保场景完全加载
        StartCoroutine(InitLoginManagerDelayed());
    }
    
    IEnumerator InitLoginManagerDelayed()
    {
        // 等待一帧，确保所有 GameObject 都已激活
        yield return null;
        
        // 确保 LoginPanel 存在
        GameObject loginPanel = GameObject.Find("LoginPanel");
        if (loginPanel == null)
        {
            Debug.LogWarning("LoginManagerLua: 未找到 LoginPanel，请确保场景中存在名为 'LoginPanel' 的 GameObject！");
            yield break;
        }
        
        Debug.Log("LoginManagerLua: 找到 LoginPanel: " + loginPanel.name);
        
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
            Debug.LogWarning("LoginManagerLua: Lua 未初始化，请确保场景中有 LuaStarter 组件！");
            yield break;
        }
        
        // 调用 Lua LoginManager 的初始化函数
        try
        {
            LuaSvr.mainState.doString(@"
                -- 确保 login_ui 模块已加载
                if not LoginManager then
                    require 'business.login_ui'
                end
                -- 调用 Start 函数初始化
                if LoginManager and LoginManager.Start then
                    LoginManager.Start()
                end
            ");
            Debug.Log("LoginManagerLua: Lua LoginManager 已成功启动");
        }
        catch (System.Exception e)
        {
            Debug.LogError("LoginManagerLua: 启动 Lua LoginManager 失败: " + e.Message);
            Debug.LogException(e);
        }
    }
}

