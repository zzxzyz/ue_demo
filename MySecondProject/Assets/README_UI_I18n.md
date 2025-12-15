# Unity UI 控件多语言初始化指南

## 概述

本指南说明如何在Unity UI控件初始化时使用多语言系统。系统提供了便捷的API来自动设置UI组件的多语言文本。

## 快速开始

### 1. 基本使用

在UI初始化函数中调用多语言设置：

```lua
local I18n = require("i18n")

-- 初始化多语言系统
I18n.Init()

-- 设置Text组件
local titleText = GameObject.Find("TitleText"):GetComponent("TextMeshProUGUI")
I18n.SetText(titleText, "login_title")  -- 自动显示"登录"或"Login"

-- 设置Button文本
local loginButton = GameObject.Find("LoginButton"):GetComponent("Button")
I18n.SetButtonText(loginButton, "login_button")  -- 自动显示"登录"或"Login"

-- 设置InputField占位符
local usernameInput = GameObject.Find("UsernameInput"):GetComponent("TMP_InputField")
I18n.SetInputPlaceholder(usernameInput, "login_username_placeholder")
```

### 2. 在登录界面中的实现

登录界面已经集成了多语言初始化，在 `InitUIComponents()` 函数中自动调用：

```lua
-- 在 InitUIComponents() 函数末尾自动调用
InitUITexts()  -- 自动设置所有UI组件的多语言文本
```

## API 参考

### I18n.SetText(textComponent, key, ...)

设置Text或TextMeshProUGUI组件的多语言文本。

**参数：**
- `textComponent`: Text或TextMeshProUGUI组件
- `key`: 多语言键值
- `...`: 可选参数，用于字符串格式化

**示例：**
```lua
-- 基本使用
I18n.SetText(titleText, "login_title")

-- 带格式化参数
I18n.SetText(messageText, "welcome_message", "张三")
-- 如果语言包中定义：welcome_message = "欢迎，%s！"
-- 结果：显示"欢迎，张三！"
```

### I18n.SetButtonText(button, key, ...)

设置Button的文本（自动查找Button子对象中的Text组件）。

**参数：**
- `button`: Button组件
- `key`: 多语言键值
- `...`: 可选参数，用于字符串格式化

**示例：**
```lua
local loginButton = GameObject.Find("LoginButton"):GetComponent("Button")
I18n.SetButtonText(loginButton, "login_button")
```

**工作原理：**
1. 首先查找 `Button/Text` 子对象中的 `TextMeshProUGUI` 组件
2. 如果没找到，查找 `Text` 组件
3. 如果还是没找到，尝试直接在Button对象上查找Text组件

### I18n.SetInputPlaceholder(inputField, key, ...)

设置InputField或TMP_InputField的占位符文本。

**参数：**
- `inputField`: TMP_InputField或InputField组件
- `key`: 多语言键值
- `...`: 可选参数，用于字符串格式化

**示例：**
```lua
local usernameInput = GameObject.Find("UsernameInput"):GetComponent("TMP_InputField")
I18n.SetInputPlaceholder(usernameInput, "login_username_placeholder")
```

### I18n.SetInputText(inputField, text)

设置InputField的文本内容（非占位符）。

**参数：**
- `inputField`: TMP_InputField或InputField组件
- `text`: 文本内容（可选，如果为空则清空）

**示例：**
```lua
I18n.SetInputText(usernameInput, "")  -- 清空输入框
I18n.SetInputText(usernameInput, "test")  -- 设置文本
```

### I18n.SetUI(config)

批量设置多个UI组件的多语言文本。

**参数：**
- `config`: 配置表数组，每个元素包含：
  - `component`: UI组件
  - `type`: 类型（"text" | "button" | "placeholder"）
  - `key`: 多语言键值

**示例：**
```lua
I18n.SetUI({
    {component = titleText, type = "text", key = "login_title"},
    {component = loginButton, type = "button", key = "login_button"},
    {component = usernameInput, type = "placeholder", key = "login_username_placeholder"},
    {component = passwordInput, type = "placeholder", key = "login_password_placeholder"},
})
```

## 完整示例

### 示例1：在UI初始化函数中使用

```lua
local I18n = require("i18n")

local function InitMyUI()
    -- 初始化多语言系统
    if not I18n.IsInitialized() then
        I18n.Init()
    end
    
    -- 查找UI组件
    local titleText = GameObject.Find("MyPanel/TitleText"):GetComponent("TextMeshProUGUI")
    local confirmButton = GameObject.Find("MyPanel/ConfirmButton"):GetComponent("Button")
    local nameInput = GameObject.Find("MyPanel/NameInput"):GetComponent("TMP_InputField")
    
    -- 设置多语言文本
    I18n.SetText(titleText, "my_title")
    I18n.SetButtonText(confirmButton, "confirm")
    I18n.SetInputPlaceholder(nameInput, "my_name_placeholder")
end
```

### 示例2：使用批量设置

```lua
local function InitMyUI()
    if not I18n.IsInitialized() then
        I18n.Init()
    end
    
    local titleText = GameObject.Find("MyPanel/TitleText"):GetComponent("TextMeshProUGUI")
    local confirmButton = GameObject.Find("MyPanel/ConfirmButton"):GetComponent("Button")
    local nameInput = GameObject.Find("MyPanel/NameInput"):GetComponent("TMP_InputField")
    
    -- 批量设置
    I18n.SetUI({
        {component = titleText, type = "text", key = "my_title"},
        {component = confirmButton, type = "button", key = "confirm"},
        {component = nameInput, type = "placeholder", key = "my_name_placeholder"},
    })
end
```

### 示例3：切换语言后刷新UI

```lua
-- 切换语言
I18n.SetLanguage("en_US")

-- 刷新UI文本
local titleText = GameObject.Find("TitleText"):GetComponent("TextMeshProUGUI")
I18n.SetText(titleText, "login_title")  -- 现在显示"Login"

-- 或者在登录界面中使用
require("business.login_ui").RefreshUITexts()
```

## 在登录界面中的实现

登录界面 (`login_ui.txt`) 已经完整实现了多语言初始化：

1. **自动初始化**：在 `InitUIComponents()` 函数中自动调用 `InitUITexts()`
2. **支持的组件**：
   - 标题文本（TitleText）
   - 登录按钮（LoginButton）
   - 用户名输入框占位符（UsernameInput）
   - 密码输入框占位符（PasswordInput）

3. **语言切换**：调用 `LoginManager.SetLanguage()` 会自动刷新所有UI文本

```lua
local LoginManager = require("business.login_ui")

-- 切换语言（会自动刷新UI）
LoginManager.SetLanguage("en_US")
```

## 语言包键值

### 登录界面相关

| 键值 | 中文 | English |
|------|------|---------|
| `login_title` | 登录 | Login |
| `login_button` | 登录 | Login |
| `login_username_placeholder` | 请输入用户名 | Enter username |
| `login_password_placeholder` | 请输入密码 | Enter password |

### 通用键值

| 键值 | 中文 | English |
|------|------|---------|
| `confirm` | 确认 | Confirm |
| `cancel` | 取消 | Cancel |
| `ok` | 确定 | OK |

## 最佳实践

### 1. 初始化时机

```lua
function MyUIManager.Start()
    -- 1. 先初始化多语言系统
    if not I18n.IsInitialized() then
        I18n.Init()
    end
    
    -- 2. 查找UI组件
    local components = FindUIComponents()
    
    -- 3. 设置多语言文本
    InitUITexts(components)
end
```

### 2. 错误处理

```lua
local function SetTextSafe(textComponent, key)
    if not textComponent then
        Debug.LogWarning("Text component is nil")
        return false
    end
    
    if not I18n.IsInitialized() then
        Debug.LogWarning("I18n not initialized")
        return false
    end
    
    return I18n.SetText(textComponent, key)
end
```

### 3. 语言切换时的处理

```lua
function MyUIManager.OnLanguageChanged()
    -- 重新设置所有UI文本
    I18n.SetText(titleText, "my_title")
    I18n.SetButtonText(confirmButton, "confirm")
    I18n.SetInputPlaceholder(nameInput, "my_name_placeholder")
end

-- 监听语言切换事件（如果实现了事件系统）
-- EventManager:AddListener("LanguageChanged", OnLanguageChanged)
```

## 注意事项

1. **初始化顺序**：确保多语言系统在设置UI文本之前已初始化
2. **组件查找**：`SetButtonText` 会自动查找Button子对象中的Text组件，确保UI结构正确
3. **占位符设置**：InputField的占位符通常位于 `InputField/Placeholder` 子对象中
4. **性能考虑**：批量设置使用 `SetUI()` 比逐个调用更高效
5. **错误处理**：所有API都会返回布尔值表示是否成功，可以用于错误处理

## 故障排除

### 问题：文本没有显示多语言

**原因**：多语言系统未初始化

**解决**：
```lua
if not I18n.IsInitialized() then
    I18n.Init()
end
```

### 问题：Button文本没有更新

**原因**：Button的Text组件位置不正确

**解决**：检查Button的UI结构，确保Text组件在 `Button/Text` 子对象中

### 问题：占位符文本没有设置

**原因**：InputField的placeholder组件未找到

**解决**：确保InputField有placeholder子对象，且组件类型正确

## 扩展

如果需要支持其他UI组件类型，可以在 `i18n.txt` 中添加新的辅助函数：

```lua
-- 设置Dropdown选项（示例）
function I18n.SetDropdownOptions(dropdown, keyPrefix, count)
    -- 实现逻辑
end
```

参考现有的 `SetText`、`SetButtonText` 等函数实现。

