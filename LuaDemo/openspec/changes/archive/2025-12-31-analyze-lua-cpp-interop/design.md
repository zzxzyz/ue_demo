## Context

本项目使用 slua_unreal 插件实现 Lua 脚本与 Unreal Engine C++ 代码的双向调用。这种集成方式允许：
- 在 C++ 中调用 Lua 脚本函数
- 在 Lua 中访问和调用 C++ 类、方法和属性
- 支持 UE4 事件（BeginPlay、Tick 等）的 Lua 重写

## Goals / Non-Goals

### Goals
- 文档化现有的 Lua-C++ 互调机制
- 提供清晰的接口使用说明
- 帮助开发者理解绑定原理

### Non-Goals
- 修改现有实现
- 添加新功能
- 性能优化

## Decisions

### 核心架构设计

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

### C++ 调用 Lua 机制

1. **接口继承**: C++ 类实现 `ILuaOverriderInterface` 接口
2. **路径映射**: 通过 `GetLuaFilePath()` 返回对应的 Lua 文件名
3. **函数调用**: 使用模板方法 `CallLuaFunction<ReturnType>(FunctionName, Args...)`

### Lua 调用 C++ 机制

1. **import 函数**: Lua 中使用 `import("ClassName")` 获取 C++ 类引用
2. **类型支持**: 支持 UClass、UScriptStruct、UEnum 三种类型
3. **缓存机制**: 已导入的类型会被缓存，避免重复查找

### 委托绑定机制

Lua 可以直接绑定 UE4 的多播委托：

```lua
-- 在 Lua 中绑定按钮点击事件
self.LoginButton.OnClicked:Add(function()
    self:OnLoginButtonClicked()
end)
```

## Risks / Trade-offs

### 风险
- **生命周期管理**: Lua 对象引用可能在 C++ 对象销毁后仍然存在
- **性能开销**: 跨语言调用有一定性能损耗

### 缓解措施
- 在 `EndPlay` 或 `NativeDestruct` 中清理 Lua 绑定
- 对频繁调用的逻辑考虑使用纯 C++ 实现

## Migration Plan

不适用（仅文档变更）

## Open Questions

1. 是否需要为常见的互调模式提供封装宏或辅助函数？
2. 是否需要添加更多的调试日志来追踪 Lua-C++ 调用？
