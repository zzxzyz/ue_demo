# C# 和 Lua 互相调用指南

本文档说明如何在 Unity 项目中使用 SLua 实现 C# 和 Lua 之间的双向调用，以登录功能为例。

## 目录结构

- `LoginService.cs` - C# 登录服务类，供 Lua 调用
- `LoginBridge.cs` - C# 和 Lua 之间的桥接类
- `business/login_ui.txt` - Lua 登录界面脚本
- `LuaStarter.cs` - Lua 初始化脚本

## 一、C# 调用 Lua

### 方式1：直接执行 Lua 代码字符串

```csharp
// 在 C# 中执行 Lua 代码
LuaSvr.mainState.doString(@"
    if LoginManager and LoginManager.LoginFromCSharp then
        LoginManager.LoginFromCSharp('admin', '123456')
    end
");
```

### 方式2：获取 Lua 函数引用并调用（推荐，更高效）

```csharp
// 获取 Lua 函数引用
LuaFunction loginFunc = LuaSvr.mainState.getFunction("LoginManager.LoginFromCSharp");
if (loginFunc != null)
{
    loginFunc.call("admin", "123456");
}
```

### 方式3：通过 LoginBridge 调用

```csharp
// 使用桥接类调用 Lua 函数
LoginBridge.Instance.CallLuaLoginFunction("admin", "123456");
// 或使用高效方式
LoginBridge.Instance.CallLuaLoginFunctionEfficient("admin", "123456");
```

## 二、Lua 调用 C#

### 方式1：调用标记了 [CustomLuaClass] 的 C# 类

在 C# 中定义类时添加 `[CustomLuaClass]` 属性：

```csharp
[CustomLuaClass]
public class LoginService
{
    public bool ValidateLogin(string username, string password)
    {
        // 登录验证逻辑
        return true;
    }
}
```

在 Lua 中可以直接访问：

```lua
-- 通过 CSharpBridge 访问（推荐）
local isValid = CSharpBridge.LoginService:ValidateLogin("admin", "123456")

-- 或者直接访问（如果已注册到全局）
local isValid = LoginService.Instance:ValidateLogin("admin", "123456")
```

### 方式2：调用 Unity API

Unity 的 API 已经自动绑定到 Lua，可以直接使用：

```lua
-- 查找 GameObject
local go = GameObject.Find("LoginPanel")

-- 获取组件
local button = go:GetComponent("Button")

-- 调用 Unity 方法
go:SetActive(false)
```

### 方式3：通过回调函数传递 Lua 函数给 C#

在 Lua 中定义函数，然后传递给 C#：

```lua
-- 定义回调函数
local function onLoginSuccess(user, msg)
    print("登录成功: " .. user .. ", " .. msg)
end

local function onLoginFailed(errorMsg)
    print("登录失败: " .. errorMsg)
end

-- 调用 C# 的异步函数，传入 Lua 回调
CSharpBridge.LoginService:LoginAsync("admin", "123456", onLoginSuccess, onLoginFailed)
```

在 C# 中接收并调用：

```csharp
public void LoginAsync(string username, string password, LuaFunction onSuccess, LuaFunction onFailed)
{
    // 执行登录逻辑...
    bool success = ValidateLogin(username, password);
    
    if (success && onSuccess != null)
    {
        onSuccess.call(username, "登录成功！");
    }
    else if (!success && onFailed != null)
    {
        onFailed.call("用户名或密码错误");
    }
}
```

## 三、登录功能完整示例

### 1. C# 端实现（LoginService.cs）

```csharp
[CustomLuaClass]
public class LoginService
{
    public static LoginService Instance { get; } = new LoginService();
    
    // 同步验证登录
    public bool ValidateLogin(string username, string password)
    {
        // 验证逻辑
        return username == "admin" && password == "123456";
    }
    
    // 异步登录（带回调）
    public void LoginAsync(string username, string password, 
                          LuaFunction onSuccess, LuaFunction onFailed)
    {
        // 异步执行登录，完成后调用回调
    }
}
```

### 2. Lua 端实现（login_ui.txt）

```lua
-- 登录按钮点击处理
local function OnLoginButtonClicked()
    local username = usernameInput.text
    local password = passwordInput.text
    
    -- 调用 C# 的登录服务
    if CSharpBridge and CSharpBridge.LoginService then
        local isValid = CSharpBridge.LoginService:ValidateLogin(username, password)
        
        if isValid then
            -- 登录成功
            titleText.text = "登录成功！"
            titleText.color = Color.green
        else
            -- 登录失败
            titleText.text = "登录失败！"
            titleText.color = Color.red
        end
    end
end

-- 供 C# 调用的函数
function LoginManager.LoginFromCSharp(username, password)
    usernameInput.text = username
    passwordInput.text = password
    OnLoginButtonClicked()
end
```

### 3. 桥接初始化（LoginBridge.cs）

```csharp
public class LoginBridge : MonoBehaviour
{
    void InitializeBridge()
    {
        // 将 C# 实例注册到 Lua 全局表
        LuaSvr.mainState.doString(@"
            CSharpBridge = {}
            CSharpBridge.LoginService = LoginService.Instance
            CSharpBridge.Bridge = LoginBridge.Instance
        ");
    }
}
```

## 四、使用步骤

1. **确保场景中有 LuaStarter 组件**
   - LuaStarter 会自动初始化 Lua 虚拟机
   - 在 `OnLuaInitComplete` 中加载 Lua 脚本

2. **确保场景中有 LoginBridge 组件（可选）**
   - LoginBridge 会自动初始化桥接
   - 或者手动调用 `LoginBridge.Instance.InitializeBridge()`

3. **在 Lua 中使用 C# 功能**
   ```lua
   -- 通过 CSharpBridge 访问
   CSharpBridge.LoginService:ValidateLogin("admin", "123456")
   ```

4. **在 C# 中调用 Lua 功能**
   ```csharp
   LoginBridge.Instance.CallLuaLoginFunction("admin", "123456");
   ```

## 五、注意事项

1. **初始化顺序**
   - 先初始化 Lua（LuaStarter）
   - 再初始化桥接（LoginBridge）
   - 最后加载业务 Lua 脚本

2. **线程安全**
   - Lua 调用必须在主线程执行
   - 异步操作使用协程或回调

3. **内存管理**
   - LuaFunction 使用完后记得 Dispose
   - 避免在 Lua 中持有 C# 对象的强引用

4. **错误处理**
   - 使用 try-catch 包裹 Lua 调用
   - 检查 Lua 函数是否存在再调用

## 六、常见问题

### Q: Lua 中无法访问 C# 类？
A: 确保类标记了 `[CustomLuaClass]` 属性，并且已通过桥接注册到 Lua。

### Q: C# 调用 Lua 函数返回 null？
A: 检查函数名是否正确，确保 Lua 脚本已加载，函数已定义。

### Q: 如何传递复杂对象？
A: 使用 LuaTable 或序列化为 JSON 字符串传递。

## 七、扩展阅读

- SLua 官方文档
- Unity 协程与 Lua 协程的配合使用
- Lua 内存管理最佳实践

