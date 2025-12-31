## ADDED Requirements

### Requirement: 登录窗口 UI 资源

登录窗口 SHALL 使用 UMG (Unreal Motion Graphics) Widget 蓝图实现。

**UI 资源文件**:
| 资源 | 路径 | 父类 | 用途 |
|------|------|------|------|
| WBP_LoginPanel | `Content/FirstPersonBP/Blueprints/WBP_LoginPanel.uasset` | ULuaUserWidget | 登录面板主界面 （当前使用）|
| WBP_LoginDialog | `Content/FirstPersonBP/Blueprints/WBP_LoginDialog.uasset` | **ULoginWidget** | 登录对话框 （未用）|

**蓝图与 C++ 类的继承关系**:
```
UUserWidget (UE4 基类)
    │
    ├── ULuaUserWidget (slua_unreal 插件)
    │       │
    │       └── WBP_LoginPanel (蓝图)
    │               └─ Lua 脚本: LoginPanel.lua
    │
    └── ULoginWidget (Source/LuaDemo/LoginWidget.cpp)
            │
            └── WBP_LoginDialog (蓝图)
                    └─ Lua 脚本: LoginWidget.lua
```

**说明**:
- `WBP_LoginDialog` 继承自 `ULoginWidget` C++ 类，因此：
  - 自动拥有 C++ 中定义的控件绑定（EmailTextBox, PasswordTextBox, LoginButton, CancelButton, ErrorText）
  - 自动拥有 C++ 中实现的验证逻辑（IsValidEmail, IsValidPassword）
  - 登录按钮点击事件在 C++ 中处理
  - 通过 `ILuaOverriderInterface` 支持 Lua 扩展

- `WBP_LoginPanel` 继承自 `ULuaUserWidget`，逻辑主要在 Lua 中实现

#### Scenario: 登录面板资源存在
- **WHEN** 编辑器加载 `Content/FirstPersonBP/Blueprints/` 目录
- **THEN** 应能找到 `WBP_LoginPanel.uasset` 和 `WBP_LoginDialog.uasset`

#### Scenario: WBP_LoginDialog 继承 ULoginWidget
- **WHEN** 在编辑器中打开 `WBP_LoginDialog` 蓝图
- **THEN** 其父类应为 `ULoginWidget`
- **AND** 蓝图中的控件名称应与 C++ 中的 `UPROPERTY(meta = (BindWidget))` 属性名称匹配

---

### Requirement: C++ 登录控件类

登录窗口 SHALL 由 `ULoginWidget` C++ 类实现核心逻辑。

**类定义位置**: `Source/LuaDemo/LoginWidget.h`
**类实现位置**: `Source/LuaDemo/LoginWidget.cpp`

**类继承关系**:
```
ULoginWidget
├── UUserWidget (UMG 基类)
└── ILuaOverriderInterface (Lua 绑定接口)
```

**UI 控件绑定** (使用 `UPROPERTY(meta = (BindWidget))`):
| 控件名称 | 控件类型 | 用途 |
|----------|----------|------|
| EmailTextBox | UEditableTextBox | 邮箱输入框 |
| PasswordTextBox | UEditableTextBox | 密码输入框 |
| LoginButton | UButton | 登录按钮 |
| CancelButton | UButton | 取消按钮 |
| ErrorText | UTextBlock | 错误信息显示 |

#### Scenario: 控件自动绑定
- **WHEN** Widget 蓝图中的控件名称与 C++ 属性名称匹配
- **THEN** UE4 自动将蓝图控件绑定到 C++ 属性

---

### Requirement: Lua 脚本扩展

登录窗口 SHALL 支持通过 Lua 脚本扩展功能。

**Lua 脚本文件**:
| 文件 | 路径 | 用途 |
|------|------|------|
| LoginWidget.lua | `Content/Lua/LoginWidget.lua` | ULoginWidget 的 Lua 扩展 |
| LoginPanel.lua | `Content/Lua/LoginPanel.lua` | 纯 Lua 实现的登录面板逻辑 |

**Lua 绑定机制**:
- C++ 类通过 `ILuaOverriderInterface` 接口与 Lua 交互
- `LuaFilePath` 属性指定对应的 Lua 文件名
- 在 `NativeConstruct()` 中调用 `CallLuaFunctionIfExist()` 执行 Lua 函数

#### Scenario: Lua 函数调用
- **WHEN** C++ 的 `NativeConstruct()` 被调用
- **THEN** 调用 Lua 脚本中的 `NativeConstruct()` 函数

---

### Requirement: 登录验证逻辑

登录窗口 SHALL 在客户端进行基础输入验证。

**验证规则**:
| 字段 | 规则 | 错误信息 |
|------|------|----------|
| 邮箱 | 非空 | "请输入邮箱地址" |
| 邮箱 | 包含 @ 和 . 符号 | "邮箱格式不正确" |
| 密码 | 非空 | "请输入密码" |
| 密码 | 长度 >= 6 | "密码长度至少为6位" |

**相关函数**:
- `IsValidEmail(Email)` - 验证邮箱格式
- `IsValidPassword(Password)` - 验证密码长度
- `ShowError(ErrorMessage)` - 显示错误信息
- `ClearError()` - 清除错误信息

#### Scenario: 邮箱验证失败
- **WHEN** 用户输入无效邮箱格式
- **THEN** 显示 "邮箱格式不正确" 错误信息

#### Scenario: 密码验证失败
- **WHEN** 用户输入少于6位的密码
- **THEN** 显示 "密码长度至少为6位" 错误信息

---

### Requirement: 按钮事件绑定

登录窗口 SHALL 支持在 C++ 和 Lua 中混合绑定按钮事件。

**绑定方式**:
| 按钮 | 绑定位置 | 绑定方法 |
|------|----------|----------|
| LoginButton | C++ | `OnClicked.AddDynamic()` |
| CancelButton | Lua | `OnClicked:Add()` |
| CloseButton | Lua | 通过 `FindWidget()` 查找后绑定 |

**C++ 绑定代码** (`LoginWidget.cpp:27`):
```cpp
LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginButtonClicked);
```

**Lua 绑定代码** (`LoginWidget.lua:10`):
```lua
self.CancelButton.OnClicked:Add(function()
    self:OnCancelButtonClicked()
end)
```

#### Scenario: 登录按钮点击
- **WHEN** 用户点击登录按钮
- **THEN** 执行 C++ 的 `OnLoginButtonClicked()` 函数

#### Scenario: 取消按钮点击
- **WHEN** 用户点击取消按钮
- **THEN** 执行 Lua 的 `OnCancelButtonClicked()` 函数

---

### Requirement: 登录成功处理

登录窗口 SHALL 在验证成功后关闭并触发后续流程。

**处理流程**:
1. 清除错误信息
2. 输出日志信息
3. 调用 `RemoveFromParent()` 关闭窗口
4. 可选：触发关卡切换或主菜单加载

#### Scenario: 登录成功关闭窗口
- **WHEN** 邮箱和密码验证通过
- **THEN** 调用 `HandleLoginSuccess()` 关闭登录窗口

---

### Requirement: slua_unreal Widget 基类

登录窗口 MAY 使用 `ULuaUserWidget` 作为基类（替代直接继承 UUserWidget）。

**基类定义**: `Plugins/slua_unreal/Source/slua_unreal/Public/LuaUserWidget.h`

**ULuaUserWidget 特性**:
- 继承自 `UUserWidget` 和 `ILuaOverriderInterface`
- 在 `Initialize()` 中自动调用 Lua 的 `Initialize()` 函数
- 在 `BeginDestroy()` 中调用 Lua 的 `OnDestroy()` 函数
- 通过 `LuaFilePath` 属性指定 Lua 脚本路径

#### Scenario: 使用 LuaUserWidget 基类
- **WHEN** Widget 蓝图选择 `ULuaUserWidget` 作为父类
- **THEN** 可以直接在 Lua 中实现所有逻辑

---

## 关键代码文件索引

| 文件路径 | 类型 | 说明 |
|----------|------|------|
| `Source/LuaDemo/LoginWidget.h` | C++ Header | ULoginWidget 类定义 |
| `Source/LuaDemo/LoginWidget.cpp` | C++ Source | ULoginWidget 类实现 |
| `Content/Lua/LoginWidget.lua` | Lua | ULoginWidget 的 Lua 扩展 |
| `Content/Lua/LoginPanel.lua` | Lua | 纯 Lua 登录面板逻辑 |
| `Content/FirstPersonBP/Blueprints/WBP_LoginPanel.uasset` | UAsset | 登录面板 Widget 蓝图 |
| `Content/FirstPersonBP/Blueprints/WBP_LoginDialog.uasset` | UAsset | 登录对话框 Widget 蓝图 |
| **`Content/FirstPersonBP/Blueprints/FirstPersonCharacter.uasset`** | **UAsset** | **角色蓝图，负责创建和显示登录面板** |
| `Plugins/slua_unreal/Source/slua_unreal/Public/LuaUserWidget.h` | C++ Header | slua 的 UserWidget 基类 |
| `Plugins/slua_unreal/Source/slua_unreal/Private/LuaUserWidget.cpp` | C++ Source | slua 的 UserWidget 实现 |

---

## 架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                        登录窗口架构                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────┐     ┌─────────────────────┐          │
│  │   WBP_LoginPanel    │     │   WBP_LoginDialog   │          │
│  │      (UAsset)       │     │      (UAsset)       │          │
│  └──────────┬──────────┘     └─────────────────────┘          │
│             │                                                   │
│             ▼                                                   │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │                    ULoginWidget (C++)                    │  │
│  │  ┌─────────────────────────────────────────────────────┐│  │
│  │  │ - EmailTextBox     (BindWidget)                     ││  │
│  │  │ - PasswordTextBox  (BindWidget)                     ││  │
│  │  │ - LoginButton      (BindWidget, C++ 事件)           ││  │
│  │  │ - CancelButton     (BindWidget, Lua 事件)           ││  │
│  │  │ - ErrorText        (BindWidget)                     ││  │
│  │  └─────────────────────────────────────────────────────┘│  │
│  │                          │                               │  │
│  │     ┌────────────────────┼────────────────────┐         │  │
│  │     │ ILuaOverriderInterface                  │         │  │
│  │     │ - GetLuaFilePath() → "LoginWidget"      │         │  │
│  │     │ - CallLuaFunctionIfExist()              │         │  │
│  │     └────────────────────┼────────────────────┘         │  │
│  └──────────────────────────┼──────────────────────────────┘  │
│                             │                                   │
│                             ▼                                   │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │              LoginWidget.lua / LoginPanel.lua            │  │
│  │  ┌─────────────────────────────────────────────────────┐│  │
│  │  │ - NativeConstruct()                                 ││  │
│  │  │ - OnCancelButtonClicked()                           ││  │
│  │  │ - OnCloseButtonClicked()                            ││  │
│  │  │ - IsValidEmail() / IsValidPassword()                ││  │
│  │  │ - ShowError() / ClearError()                        ││  │
│  │  └─────────────────────────────────────────────────────┘│  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 显示流程说明

登录窗口通过 `FirstPersonCharacter` 蓝图在 `Event BeginPlay` 时创建并显示。

**蓝图位置**: `Content/FirstPersonBP/Blueprints/FirstPersonCharacter.uasset`

**蓝图流程**:
```
Event BeginPlay (事件开始运行)
    │
    ├─→ 头戴显示器已启用? (Is HMD Enabled)
    │       │
    │       ├─ True → 分支检查 Motion Controllers
    │       │           │
    │       │           ├─ True → 设置使用控制器旋转 Yaw
    │       │           │         设置 Mesh2P 隐藏
    │       │           │         设置 VR_Gun 显示
    │       │           │
    │       │           └─ False → (跳过 VR 设置)
    │       │
    │       └─ False → (非 VR 模式)
    │
    └─→ 创建 WBP Login Panel 控件 (Create Widget)
            Class: WBP_LoginPanel
            Owning Player: (当前玩家)
                │
                └─→ 添加到视口 (Add to Viewport)
                        目标: 返回的 Widget 实例
```

**关键蓝图节点**:
| 节点 | 说明 |
|------|------|
| 事件开始运行 | Event BeginPlay，角色生成时触发 |
| 头戴显示器已启用 | 检查是否在 VR 模式 |
| 创建WBP Login Panel控件 | 创建登录面板 Widget 实例 |
| 添加到视口 | 将 Widget 添加到屏幕显示 |

**VR 模式额外逻辑**:
- 检查 Motion Controllers 是否启用
- 设置控制器旋转 Yaw
- 隐藏 Mesh2P（第一人称网格）
- 显示 VR_Gun（VR 枪支模型）
