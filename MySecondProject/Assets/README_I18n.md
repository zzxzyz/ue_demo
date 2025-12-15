# 多语言适配使用说明

## 概述

本项目已实现完整的多语言适配系统，支持动态语言切换。登录界面已集成多语言功能，可以根据系统语言自动切换，也支持手动切换语言。

## 文件结构

```
Assets/Slua/Resources/
├── i18n.txt                    # 多语言管理模块
└── i18n/
    ├── zh_CN.txt              # 简体中文语言包
    └── en_US.txt              # 英文语言包
```

## 支持的语言

- `zh_CN` - 简体中文（默认）
- `zh_TW` - 繁体中文
- `en_US` - English
- `ja_JP` - 日本語
- `ko_KR` - 한국어

## 使用方法

### 1. 基本使用

在Lua代码中使用多语言：

```lua
local I18n = require("i18n")

-- 初始化多语言系统（会自动检测系统语言）
I18n.Init()

-- 获取本地化文本
local text = I18n.Get("login_title")  -- 返回 "登录" 或 "Login"（取决于当前语言）

-- 简写形式
local text = I18n.T("login_button")  -- 返回 "登录" 或 "Login"
```

### 2. 切换语言

```lua
-- 切换到英文
I18n.SetLanguage("en_US")

-- 切换到简体中文
I18n.SetLanguage("zh_CN")

-- 获取当前语言
local currentLang = I18n.GetCurrentLanguage()  -- 返回 "zh_CN" 或 "en_US"
```

### 3. 在登录界面中使用

登录界面已集成多语言功能，所有文本都会自动根据当前语言显示：

```lua
-- 登录界面会自动使用多语言
-- 例如：登录成功时显示 "登录成功！" 或 "Login successful!"
```

### 4. UI控件初始化时使用多语言

在UI初始化时自动设置多语言文本：

```lua
local I18n = require("i18n")

-- 初始化多语言系统
I18n.Init()

-- 设置Text组件
local titleText = GameObject.Find("TitleText"):GetComponent("TextMeshProUGUI")
I18n.SetText(titleText, "login_title")

-- 设置Button文本
local loginButton = GameObject.Find("LoginButton"):GetComponent("Button")
I18n.SetButtonText(loginButton, "login_button")

-- 设置InputField占位符
local usernameInput = GameObject.Find("UsernameInput"):GetComponent("TMP_InputField")
I18n.SetInputPlaceholder(usernameInput, "login_username_placeholder")
```

**详细说明请参考：** `README_UI_I18n.md`

### 4. 添加新的语言文本

#### 步骤1：在语言包文件中添加键值

编辑 `Assets/Slua/Resources/i18n/zh_CN.txt`：

```lua
local zh_CN = {
    -- ... 现有文本 ...
    my_new_text = "我的新文本",
}
```

编辑 `Assets/Slua/Resources/i18n/en_US.txt`：

```lua
local en_US = {
    -- ... 现有文本 ...
    my_new_text = "My New Text",
}
```

#### 步骤2：在代码中使用

```lua
local text = I18n.Get("my_new_text")
```

### 5. 字符串格式化

支持使用 `string.format` 进行格式化：

```lua
-- 在语言包中定义
-- zh_CN: welcome_message = "欢迎，%s！"
-- en_US: welcome_message = "Welcome, %s!"

-- 在代码中使用
local message = I18n.Get("welcome_message", "张三")  
-- 中文：欢迎，张三！
-- 英文：Welcome, 张三!
```

## 登录界面多语言键值列表

| 键值 | 中文 | English |
|------|------|---------|
| `login_title` | 登录 | Login |
| `login_username` | 用户名 | Username |
| `login_password` | 密码 | Password |
| `login_button` | 登录 | Login |
| `login_success` | 登录成功！ | Login successful! |
| `login_failed` | 登录失败！ | Login failed! |
| `login_error_username_or_password` | 用户名或密码错误 | Invalid username or password |
| `login_error_username_empty` | 请输入用户名！ | Please enter username! |
| `login_error_password_empty` | 请输入密码！ | Please enter password! |

## 自动语言检测

系统会自动检测Unity的系统语言并设置对应的语言：

- `SystemLanguage.Chinese` → `zh_CN`
- `SystemLanguage.ChineseSimplified` → `zh_CN`
- `SystemLanguage.ChineseTraditional` → `zh_TW`
- `SystemLanguage.English` → `en_US`
- `SystemLanguage.Japanese` → `ja_JP`
- `SystemLanguage.Korean` → `ko_KR`

如果系统语言不在支持列表中，默认使用 `zh_CN`。

## 语言设置持久化

用户手动切换的语言会保存到 `PlayerPrefs` 中，下次启动时会自动使用上次设置的语言。

## 扩展支持更多语言

### 步骤1：创建新的语言包文件

例如添加日文支持，创建 `Assets/Slua/Resources/i18n/ja_JP.txt`：

```lua
local ja_JP = {
    login_title = "ログイン",
    login_username = "ユーザー名",
    login_password = "パスワード",
    -- ... 其他文本 ...
}
return ja_JP
```

### 步骤2：在 i18n.txt 中添加支持

编辑 `Assets/Slua/Resources/i18n.txt`，在 `supportedLanguages` 表中添加：

```lua
I18n.supportedLanguages = {
    -- ... 现有语言 ...
    ja_JP = "日本語",
}
```

### 步骤3：添加语言映射（可选）

如果需要自动检测系统语言，在 `languageMap` 中添加映射：

```lua
local languageMap = {
    -- ... 现有映射 ...
    [UnityEngine.SystemLanguage.Japanese] = "ja_JP",
}
```

## 注意事项

1. **语言文件路径**：语言包文件必须放在 `Assets/Slua/Resources/i18n/` 目录下
2. **文件命名**：语言包文件名必须与语言代码一致（如 `zh_CN.txt`）
3. **键值一致性**：所有语言包必须包含相同的键值，否则会显示警告
4. **初始化时机**：建议在游戏启动时尽早初始化多语言系统
5. **性能考虑**：语言数据在首次加载时会被缓存，切换语言时会重新加载

## 示例代码

### 在C#中切换语言

```csharp
// 从C#调用Lua切换语言
LuaSvr.mainState.doString("require 'business.login_ui'; LoginManager.SetLanguage('en_US')");
```

### 在Lua中切换语言

```lua
local LoginManager = require("business.login_ui")

-- 切换到英文
LoginManager.SetLanguage("en_US")

-- 切换到中文
LoginManager.SetLanguage("zh_CN")
```

## 故障排除

### 问题：文本显示为键值本身

**原因**：语言包中缺少对应的键值

**解决**：检查语言包文件，确保所有使用的键值都已定义

### 问题：语言切换不生效

**原因**：可能语言包文件未正确加载

**解决**：
1. 检查语言包文件路径是否正确
2. 检查文件命名是否正确
3. 查看Unity Console中的错误日志

### 问题：系统语言检测失败

**原因**：系统语言不在支持列表中

**解决**：系统会自动使用默认语言 `zh_CN`，或手动调用 `I18n.SetLanguage()` 设置语言

