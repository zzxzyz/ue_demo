using UnityEngine;
using SLua;
using System;

/// <summary>
/// 登录服务类 - 供Lua调用的C#登录功能
/// 使用 [CustomLuaClass] 属性，让Lua可以直接访问这个类
/// </summary>
[CustomLuaClass]
public class LoginService
{
    // 单例模式
    private static LoginService _instance;
    public static LoginService Instance
    {
        get
        {
            if (_instance == null)
            {
                _instance = new LoginService();
            }
            return _instance;
        }
    }

    /// <summary>
    /// 执行登录验证（供Lua调用）
    /// </summary>
    /// <param name="username">用户名</param>
    /// <param name="password">密码</param>
    /// <returns>登录是否成功</returns>
    public bool ValidateLogin(string username, string password)
    {
        Debug.Log($"[C# LoginService] 验证登录: 用户名={username}, 密码={password}");
        
        // 模拟登录验证逻辑
        if (string.IsNullOrEmpty(username) || string.IsNullOrEmpty(password))
        {
            Debug.LogWarning("[C# LoginService] 用户名或密码为空");
            return false;
        }

        // 简单的验证逻辑（实际项目中应该连接服务器）
        if (username == "admin" && password == "123456")
        {
            Debug.Log("[C# LoginService] 登录验证成功");
            return true;
        }

        Debug.LogWarning("[C# LoginService] 登录验证失败：用户名或密码错误");
        return false;
    }

    /// <summary>
    /// 网络登录请求（模拟异步操作）
    /// </summary>
    /// <param name="username">用户名</param>
    /// <param name="password">密码</param>
    /// <param name="onSuccess">成功回调（Lua函数）</param>
    /// <param name="onFailed">失败回调（Lua函数）</param>
    public void LoginAsync(string username, string password, LuaFunction onSuccess, LuaFunction onFailed)
    {
        Debug.Log($"[C# LoginService] 开始异步登录: 用户名={username}");
        
        // 模拟异步操作（实际应该使用协程或异步方法）
        // 这里使用协程来模拟
        var go = new GameObject("LoginCoroutineHelper");
        var helper = go.AddComponent<LoginCoroutineHelper>();
        helper.StartLogin(username, password, onSuccess, onFailed);
    }

    /// <summary>
    /// 获取用户信息（供Lua调用）
    /// </summary>
    /// <param name="username">用户名</param>
    /// <returns>用户信息字符串</returns>
    public string GetUserInfo(string username)
    {
        Debug.Log($"[C# LoginService] 获取用户信息: {username}");
        return $"用户: {username}, 等级: 10, 经验: 1000";
    }

    /// <summary>
    /// 保存登录状态（供Lua调用）
    /// </summary>
    /// <param name="username">用户名</param>
    public void SaveLoginState(string username)
    {
        Debug.Log($"[C# LoginService] 保存登录状态: {username}");
        PlayerPrefs.SetString("LastLoginUser", username);
        PlayerPrefs.Save();
    }

    /// <summary>
    /// 获取上次登录的用户名（供Lua调用）
    /// </summary>
    /// <returns>上次登录的用户名</returns>
    public string GetLastLoginUser()
    {
        string lastUser = PlayerPrefs.GetString("LastLoginUser", "");
        Debug.Log($"[C# LoginService] 获取上次登录用户: {lastUser}");
        return lastUser;
    }
}

/// <summary>
/// 协程辅助类，用于执行异步登录操作
/// </summary>
public class LoginCoroutineHelper : MonoBehaviour
{
    public void StartLogin(string username, string password, LuaFunction onSuccess, LuaFunction onFailed)
    {
        StartCoroutine(LoginCoroutine(username, password, onSuccess, onFailed));
    }

    private System.Collections.IEnumerator LoginCoroutine(string username, string password, LuaFunction onSuccess, LuaFunction onFailed)
    {
        // 模拟网络延迟
        yield return new WaitForSeconds(1.0f);

        // 执行验证
        bool success = LoginService.Instance.ValidateLogin(username, password);

        // 调用Lua回调函数
        if (success)
        {
            if (onSuccess != null)
            {
                try
                {
                    onSuccess.call(username, "登录成功！");
                }
                catch (Exception e)
                {
                    Debug.LogError($"[C# LoginService] 调用Lua成功回调失败: {e.Message}");
                }
            }
        }
        else
        {
            if (onFailed != null)
            {
                try
                {
                    onFailed.call("用户名或密码错误");
                }
                catch (Exception e)
                {
                    Debug.LogError($"[C# LoginService] 调用Lua失败回调失败: {e.Message}");
                }
            }
        }

        // 清理
        Destroy(gameObject);
    }
}

