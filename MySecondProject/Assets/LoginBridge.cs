using UnityEngine;
using SLua;
using System;
using System.Collections;

/// <summary>
/// C#和Lua之间的桥接类
/// 用于注册Lua函数供C#调用，以及注册C#函数供Lua调用
/// </summary>
public class LoginBridge : MonoBehaviour
{
    private static LoginBridge _instance;
    public static LoginBridge Instance
    {
        get
        {
            if (_instance == null)
            {
                GameObject go = new GameObject("LoginBridge");
                _instance = go.AddComponent<LoginBridge>();
                DontDestroyOnLoad(go);
            }
            return _instance;
        }
    }

    // 存储Lua函数的引用
    private LuaFunction luaOnLoginSuccess = null;
    private LuaFunction luaOnLoginFailed = null;
    private LuaFunction luaOnLoginComplete = null;

    void Awake()
    {
        if (_instance == null)
        {
            _instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else if (_instance != this)
        {
            Destroy(gameObject);
        }
    }

    /// <summary>
    /// 注册Lua函数供C#调用
    /// 这个方法应该从Lua中调用，将Lua函数注册到C#
    /// </summary>
    public void RegisterLuaLoginCallbacks(LuaFunction onSuccess, LuaFunction onFailed, LuaFunction onComplete)
    {
        luaOnLoginSuccess = onSuccess;
        luaOnLoginFailed = onFailed;
        luaOnLoginComplete = onComplete;
        Debug.Log("[C# LoginBridge] Lua回调函数已注册");
    }

    /// <summary>
    /// C#调用Lua的成功回调
    /// </summary>
    public void CallLuaOnLoginSuccess(string username, string message)
    {
        if (luaOnLoginSuccess != null)
        {
            try
            {
                luaOnLoginSuccess.call(username, message);
            }
            catch (System.Exception e)
            {
                Debug.LogError($"[C# LoginBridge] 调用Lua成功回调失败: {e.Message}");
            }
        }
    }

    /// <summary>
    /// C#调用Lua的失败回调
    /// </summary>
    public void CallLuaOnLoginFailed(string errorMessage)
    {
        if (luaOnLoginFailed != null)
        {
            try
            {
                luaOnLoginFailed.call(errorMessage);
            }
            catch (System.Exception e)
            {
                Debug.LogError($"[C# LoginBridge] 调用Lua失败回调失败: {e.Message}");
            }
        }
    }

    /// <summary>
    /// C#调用Lua的完成回调
    /// </summary>
    public void CallLuaOnLoginComplete(bool success, string message)
    {
        if (luaOnLoginComplete != null)
        {
            try
            {
                luaOnLoginComplete.call(success, message);
            }
            catch (System.Exception e)
            {
                Debug.LogError($"[C# LoginBridge] 调用Lua完成回调失败: {e.Message}");
            }
        }
    }

    /// <summary>
    /// 从C#调用Lua中定义的登录函数
    /// </summary>
    public void CallLuaLoginFunction(string username, string password)
    {
        if (LuaSvr.mainState == null)
        {
            Debug.LogError("[C# LoginBridge] Lua未初始化");
            return;
        }

        try
        {
            // 方式1：直接执行Lua代码
            LuaSvr.mainState.doString($@"
                if LoginManager and LoginManager.LoginFromCSharp then
                    LoginManager.LoginFromCSharp('{username}', '{password}')
                else
                    print('LoginManager.LoginFromCSharp 函数不存在')
                end
            ");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"[C# LoginBridge] 调用Lua登录函数失败: {e.Message}");
        }
    }

    /// <summary>
    /// 获取Lua函数并调用（更高效的方式）
    /// </summary>
    public void CallLuaLoginFunctionEfficient(string username, string password)
    {
        if (LuaSvr.mainState == null)
        {
            Debug.LogError("[C# LoginBridge] Lua未初始化");
            return;
        }

        try
        {
            // 方式2：获取Lua函数引用并调用（更高效）
            LuaFunction loginFunc = LuaSvr.mainState.getFunction("LoginManager.LoginFromCSharp");
            if (loginFunc != null)
            {
                loginFunc.call(username, password);
            }
            else
            {
                Debug.LogWarning("[C# LoginBridge] LoginManager.LoginFromCSharp 函数不存在");
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError($"[C# LoginBridge] 调用Lua登录函数失败: {e.Message}");
        }
    }

    /// <summary>
    /// 初始化桥接（在Lua初始化完成后调用）
    /// </summary>
    public void InitializeBridge()
    {
        StartCoroutine(WaitForLuaAndRegister());
    }

    private IEnumerator WaitForLuaAndRegister()
    {
        // 等待Lua初始化完成
        int maxWaitFrames = 10;
        int waitFrames = 0;
        while (LuaSvr.mainState == null && waitFrames < maxWaitFrames)
        {
            yield return null;
            waitFrames++;
        }

        if (LuaSvr.mainState == null)
        {
            Debug.LogWarning("[C# LoginBridge] Lua未初始化，无法注册桥接函数");
            yield break;
        }

        // 注册C#函数到Lua全局表
        try
        {
            IntPtr L = LuaSvr.mainState.L;
            
            // 创建 CSharpBridge 表
            LuaSvr.mainState.doString("CSharpBridge = {}");
            
            // 获取 CSharpBridge 表到栈顶
            LuaDLL.lua_getglobal(L, "CSharpBridge");
            
            // 将 LoginService.Instance 推送到栈
            LuaObject.pushObject(L, LoginService.Instance);
            // 设置到 CSharpBridge.LoginService
            LuaDLL.lua_setfield(L, -2, "LoginService");
            
            // 将 LoginBridge.Instance 推送到栈
            LuaObject.pushObject(L, LoginBridge.Instance);
            // 设置到 CSharpBridge.Bridge
            LuaDLL.lua_setfield(L, -2, "Bridge");
            
            // 弹出 CSharpBridge 表
            LuaDLL.lua_pop(L, 1);
            
            Debug.Log("[C# LoginBridge] 桥接函数已注册到Lua");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"[C# LoginBridge] 注册桥接函数失败: {e.Message}");
        }
    }

    void OnDestroy()
    {
        // 清理Lua函数引用
        if (luaOnLoginSuccess != null)
        {
            luaOnLoginSuccess.Dispose();
            luaOnLoginSuccess = null;
        }
        if (luaOnLoginFailed != null)
        {
            luaOnLoginFailed.Dispose();
            luaOnLoginFailed = null;
        }
        if (luaOnLoginComplete != null)
        {
            luaOnLoginComplete.Dispose();
            luaOnLoginComplete = null;
        }
    }
}

