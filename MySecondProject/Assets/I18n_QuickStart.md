# 多语言适配快速开始

## 已实现的功能

✅ 多语言管理模块 (`i18n.txt`)
✅ 中文和英文语言包
✅ 登录界面多语言集成
✅ 自动语言检测（基于系统语言）
✅ 手动语言切换
✅ 语言设置持久化

## 快速使用

### 1. 自动使用（推荐）

登录界面会自动根据系统语言显示对应的文本，无需额外配置。

### 2. 手动切换语言

在Unity Console或Lua代码中执行：

```lua
-- 切换到英文
require("business.login_ui").SetLanguage("en_US")

-- 切换到中文
require("business.login_ui").SetLanguage("zh_CN")
```

### 3. 在其他Lua脚本中使用

```lua
local I18n = require("i18n")

-- 初始化（如果还未初始化）
if not I18n.IsInitialized() then
    I18n.Init()
end

-- 获取文本
local text = I18n.Get("login_title")  -- "登录" 或 "Login"
```

## 文件说明

- `Assets/Slua/Resources/i18n.txt` - 多语言管理核心模块
- `Assets/Slua/Resources/i18n/zh_CN.txt` - 简体中文语言包
- `Assets/Slua/Resources/i18n/en_US.txt` - 英文语言包
- `Assets/Slua/Resources/business/login_ui.txt` - 已集成多语言的登录界面

## 添加新语言

1. 在 `Assets/Slua/Resources/i18n/` 目录下创建新的语言包文件（如 `ja_JP.txt`）
2. 复制 `zh_CN.txt` 的内容，翻译所有文本
3. 在 `i18n.txt` 的 `supportedLanguages` 表中添加新语言

详细说明请参考 `README_I18n.md`

