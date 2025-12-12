using UnityEngine;
using SLua;

/// <summary>
/// 登录功能测试示例
/// 演示如何在C#中调用Lua，以及如何在Lua中调用C#
/// </summary>
public class LoginTestExample : MonoBehaviour
{
    void Start()
    {
        // 等待Lua初始化完成
        StartCoroutine(TestAfterLuaInit());
    }

    private System.Collections.IEnumerator TestAfterLuaInit()
    {
        // 等待Lua初始化
        int maxWaitFrames = 20;
        int waitFrames = 0;
        while (LuaSvr.mainState == null && waitFrames < maxWaitFrames)
        {
            yield return null;
            waitFrames++;
        }

        if (LuaSvr.mainState == null)
        {
            Debug.LogError("Lua未初始化，无法执行测试");
            yield break;
        }

        // 等待桥接初始化
        yield return new WaitForSeconds(0.5f);

        Debug.Log("========== 开始测试 C# 和 Lua 互相调用 ==========");

        // 测试1：C# 调用 Lua 函数
        TestCallLuaFunction();

        // 等待一下
        yield return new WaitForSeconds(1.0f);

        // 测试2：C# 调用 Lua 函数（高效方式）
        TestCallLuaFunctionEfficient();

        // 等待一下
        yield return new WaitForSeconds(1.0f);

        // 测试3：测试 C# 登录服务（供 Lua 调用）
        TestLoginService();
    }

    /// <summary>
    /// 测试1：C# 调用 Lua 函数（方式1：直接执行代码）
    /// </summary>
    void TestCallLuaFunction()
    {
        Debug.Log("--- 测试1：C# 调用 Lua 函数（直接执行代码） ---");
        
        try
        {
            LuaSvr.mainState.doString(@"
                if LoginManager and LoginManager.LoginFromCSharp then
                    LoginManager.LoginFromCSharp('testuser', 'testpass')
                    print('[Lua] 收到来自C#的登录请求')
                end
            ");
            Debug.Log("✓ C# 成功调用 Lua 函数");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"✗ C# 调用 Lua 函数失败: {e.Message}");
        }
    }

    /// <summary>
    /// 测试2：C# 调用 Lua 函数（方式2：获取函数引用）
    /// </summary>
    void TestCallLuaFunctionEfficient()
    {
        Debug.Log("--- 测试2：C# 调用 Lua 函数（获取函数引用） ---");
        
        try
        {
            LuaFunction loginFunc = LuaSvr.mainState.getFunction("LoginManager.LoginFromCSharp");
            if (loginFunc != null)
            {
                loginFunc.call("admin", "123456");
                Debug.Log("✓ C# 成功调用 Lua 函数（高效方式）");
            }
            else
            {
                Debug.LogWarning("✗ Lua 函数 LoginManager.LoginFromCSharp 不存在");
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError($"✗ C# 调用 Lua 函数失败: {e.Message}");
        }
    }

    /// <summary>
    /// 测试3：测试 C# 登录服务（供 Lua 调用）
    /// </summary>
    void TestLoginService()
    {
        Debug.Log("--- 测试3：C# LoginService（供 Lua 调用） ---");
        
        // 测试同步验证
        bool result1 = LoginService.Instance.ValidateLogin("admin", "123456");
        Debug.Log($"✓ 同步验证测试: admin/123456 = {result1}");

        bool result2 = LoginService.Instance.ValidateLogin("wrong", "wrong");
        Debug.Log($"✓ 同步验证测试: wrong/wrong = {result2}");

        // 测试获取用户信息
        string userInfo = LoginService.Instance.GetUserInfo("admin");
        Debug.Log($"✓ 获取用户信息: {userInfo}");

        // 测试保存和获取登录状态
        LoginService.Instance.SaveLoginState("admin");
        string lastUser = LoginService.Instance.GetLastLoginUser();
        Debug.Log($"✓ 保存和获取登录状态: {lastUser}");

        // 测试异步登录（带回调）
        Debug.Log("--- 测试异步登录（带 Lua 回调） ---");
        try
        {
            // 在 Lua 中定义回调函数
            LuaSvr.mainState.doString(@"
                local function onSuccess(user, msg)
                    print('[Lua回调] 登录成功: ' .. user .. ', ' .. msg)
                end
                
                local function onFailed(errorMsg)
                    print('[Lua回调] 登录失败: ' .. errorMsg)
                end
                
                -- 调用 C# 的异步登录
                if CSharpBridge and CSharpBridge.LoginService then
                    CSharpBridge.LoginService:LoginAsync('admin', '123456', onSuccess, onFailed)
                end
            ");
            Debug.Log("✓ 异步登录测试已启动（1秒后会有回调结果）");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"✗ 异步登录测试失败: {e.Message}");
        }
    }

    /// <summary>
    /// 在 Inspector 中点击按钮测试
    /// </summary>
    [ContextMenu("测试 C# 调用 Lua")]
    public void TestCallLua()
    {
        if (LuaSvr.mainState == null)
        {
            Debug.LogError("Lua 未初始化");
            return;
        }

        LoginBridge.Instance.CallLuaLoginFunction("admin", "123456");
    }

    /// <summary>
    /// 在 Inspector 中点击按钮测试
    /// </summary>
    [ContextMenu("测试 Lua 调用 C#")]
    public void TestCallCSharp()
    {
        if (LuaSvr.mainState == null)
        {
            Debug.LogError("Lua 未初始化");
            return;
        }

        try
        {
            LuaSvr.mainState.doString(@"
                if CSharpBridge and CSharpBridge.LoginService then
                    local result = CSharpBridge.LoginService:ValidateLogin('admin', '123456')
                    print('[Lua] 调用 C# LoginService.ValidateLogin 结果: ' .. tostring(result))
                    
                    local userInfo = CSharpBridge.LoginService:GetUserInfo('admin')
                    print('[Lua] 获取用户信息: ' .. userInfo)
                else
                    print('[Lua] CSharpBridge 未初始化')
                end
            ");
        }
        catch (System.Exception e)
        {
            Debug.LogError($"测试失败: {e.Message}");
        }
    }
}

