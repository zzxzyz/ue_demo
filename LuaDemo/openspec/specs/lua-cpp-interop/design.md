# Lua-C++ 互调机制技术设计

## Context

本项目使用 slua_unreal 插件实现 Lua 脚本与 Unreal Engine C++ 代码的双向调用。这种集成方式允许：
- 在 C++ 中调用 Lua 脚本函数
- 在 Lua 中访问和调用 C++ 类、方法和属性
- 支持 UE4 事件（BeginPlay、Tick 等）的 Lua 重写

## 核心架构设计

slua_unreal 采用以下架构实现 Lua-C++ 互调：

```
┌─────────────────────────────────────────────────────────────┐
│                      UMyGameInstance                        │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                   NS_SLUA::LuaState                  │   │
│  │  - 管理 Lua 虚拟机生命周期                            │   │
│  │  - 提供 import() 全局函数                             │   │
│  │  - 加载和执行 Lua 文件                                │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                              │
          ┌───────────────────┼───────────────────┐
          ▼                   ▼                   ▼
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│    ALuaActor    │  │  ULoginWidget   │  │  其他 Lua 绑定类 │
│ (继承 AActor)    │  │(继承 UUserWidget)│  │                 │
├─────────────────┤  ├─────────────────┤  ├─────────────────┤
│ILuaOverriderInterface                                      │
│  - GetLuaFilePath()                                        │
│  - GetSelfTable()                                          │
│  - CallLuaFunction()                                       │
│  - IsLuaFunctionExist()                                    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      Lua 脚本文件                            │
│  Content/Lua/                                               │
│  ├── MyLuaActor.lua     (对应 AMyLuaActor)                  │
│  ├── LoginWidget.lua    (对应 ULoginWidget)                 │
│  └── LoginPanel.lua     (对应纯 Lua UI 逻辑)                │
└─────────────────────────────────────────────────────────────┘
```

## C++ 调用 Lua 机制

### 实现步骤

1. **接口继承**: C++ 类实现 `ILuaOverriderInterface` 接口
2. **路径映射**: 通过 `GetLuaFilePath()` 返回对应的 Lua 文件名
3. **函数调用**: 使用模板方法 `CallLuaFunction<ReturnType>(FunctionName, Args...)`

### 代码示例

```cpp
// LoginWidget.cpp
void ULoginWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 调用 Lua 的 NativeConstruct 函数
    if (IsLuaFunctionExist(TEXT("NativeConstruct")))
    {
        CallLuaFunctionIfExist(TEXT("NativeConstruct"));
    }
}
```

## Lua 调用 C++ 机制

### 实现步骤

1. **import 函数**: Lua 中使用 `import("ClassName")` 获取 C++ 类引用
2. **类型支持**: 支持 UClass、UScriptStruct、UEnum 三种类型
3. **缓存机制**: 已导入的类型会被缓存，避免重复查找

### 代码示例

```lua
-- LoginPanel.lua
local UIHelper = import("UIHelper")

function LoginPanel:SetUIInputMode()
    local playerController = self:GetOwningPlayer()
    if playerController then
        UIHelper.SetInputModeGameAndUI(playerController, self)
    end
end
```

## 委托绑定机制

Lua 可以直接绑定 UE4 的多播委托：

```lua
-- 在 Lua 中绑定按钮点击事件
self.LoginButton.OnClicked:Add(function()
    self:OnLoginButtonClicked()
end)

-- 清除已有绑定
self.LoginButton.OnClicked:Clear()
```

## Lua 脚本类定义模式

标准的 Lua 脚本类定义模式：

```lua
local MyClass = {}

function MyClass:Initialize()
    -- 初始化逻辑
end

function MyClass:NativeConstruct()
    -- 构造逻辑
end

function MyClass:SomeMethod()
    -- 方法实现
end

return Class(nil, nil, MyClass)
```

## 生命周期管理

### LuaState 创建流程

```
UMyGameInstance 构造
    ↓
CreateLuaState()
    ↓
new NS_SLUA::LuaState("SLuaMainState", this)
    ↓
setLoadFileDelegate(...)  // 设置文件加载委托
    ↓
state->init()  // 初始化 Lua 环境
    ↓
state->doFile("helloworld")  // 加载入口脚本
```

### 对象绑定流程

```
C++ 对象创建（如 ALuaActor）
    ↓
PostInitializeComponents()
    ↓
ILuaOverriderInterface::PostLuaHook()
    ↓
加载对应的 Lua 脚本
    ↓
Lua 方法可被 C++ 调用
```

## 风险与权衡

### 风险
- **生命周期管理**: Lua 对象引用可能在 C++ 对象销毁后仍然存在
- **性能开销**: 跨语言调用有一定性能损耗

### 缓解措施
- 在 `EndPlay` 或 `NativeDestruct` 中清理 Lua 绑定
- 对频繁调用的逻辑考虑使用纯 C++ 实现

## 关键文件索引

| 文件 | 用途 |
|------|------|
| `Source/LuaDemo/MyGameInstance.cpp` | LuaState 生命周期管理 |
| `Source/LuaDemo/LoginWidget.cpp` | C++ 调用 Lua 示例 |
| `Source/LuaDemo/UIHelper.cpp` | 蓝图函数库示例 |
| `Content/Lua/LoginPanel.lua` | Lua 调用 C++ 示例 |
| `Content/Lua/LoginWidget.lua` | Lua 委托绑定示例 |
| `Content/Lua/MyLuaActor.lua` | Lua Actor 示例 |
| `Plugins/slua_unreal/Source/slua_unreal/Public/LuaOverriderInterface.h` | 核心接口定义 |
| `Plugins/slua_unreal/Source/slua_unreal/Private/LuaState.cpp` | import 函数实现 |
