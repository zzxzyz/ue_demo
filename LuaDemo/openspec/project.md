# Project Context

## Purpose
LuaDemo 是一个基于 Unreal Engine 4.26 的 Lua 脚本集成演示项目。该项目的主要目标是：

1. **学习和研究** UE4 与 Lua 脚本的集成方案
2. **演示** 如何使用 slua_unreal 插件将 Lua 脚本绑定到 UE4 游戏对象
3. **提供** 一个可运行的第一人称游戏模板，支持热更新的 Lua 游戏逻辑

## Tech Stack
- **游戏引擎**: Unreal Engine 4.26
- **主要开发语言**: C++ (UE4 原生)
- **脚本语言**: Lua 5.3+ (通过 slua_unreal 插件集成)
- **Lua 绑定插件**: slua_unreal - 用于将 Lua 脚本绑定到 UE4 对象
- **调试工具**: 
  - LuaPanda - Lua 远程调试器 (端口 8818)
  - slua_profile - Lua 性能分析工具 (端口 8081)
  - IDbg - 第三方调试库
- **目标平台**: Windows (Win64)
- **构建系统**: UnrealBuildTool (UBT)

## Project Conventions

### Code Style
- **注释语言**: 使用中文编写代码注释
- **命名规范**: 
  - 函数和变量使用驼峰命名法 (camelCase / PascalCase)
  - UE4 类遵循 UE4 命名规范 (A 前缀用于 Actor, U 前缀用于 UObject)
- **实现原则**: 优先使用简洁的实现方式，避免过度设计
- **Lua 文件**: 存放在 `Content/Lua/` 目录下，支持 `.lua` 和 `.luac` 格式

### Architecture Patterns
- **GameInstance**: `UMyGameInstance` 是全局游戏实例，负责创建和管理 LuaState
- **LuaState 生命周期**: 在 GameInstance 构造函数中创建，在 Shutdown 时销毁
- **Lua 文件加载机制**: 
  - GameInstance 在构造时加载 `helloworld.lua`
  - 其他对象 (GameMode, PlayerController, Pawn, Actor) 通过 `PostLuaHook()` 机制加载
- **Lua 绑定**: 使用 `ILuaOverriderInterface` 接口实现 C++ 与 Lua 的双向绑定
- **蓝图集成**: 支持通过蓝图扩展的 GameMode (`BP_AdventureGameMode`)

### Module Dependencies
```
LuaDemo (主模块)
├── Core, CoreUObject, Engine, InputCore
├── EnhancedInput, UMG
├── slua_unreal (Lua 绑定)
├── slua_profile (仅编辑器模式)
├── Slate, SlateCore
├── Http
└── IDbg (第三方调试库)
```

### Testing Strategy
- 使用 LuaPanda 进行 Lua 脚本的断点调试
- 使用 slua_profile 进行 Lua 性能分析和内存追踪
- 通过 UE4 编辑器 PIE (Play In Editor) 模式进行功能测试
- 支持独立游戏 (Standalone) 模式运行测试

### Git Workflow
- 主分支用于稳定版本
- 使用 `.gitignore` 排除中间文件和生成文件
- 提交信息使用中文描述

## Domain Context

### UE4 启动流程
项目已记录详细的 UE4 启动流程 (见 `UE启动流程分析.md`)：

1. **GameInstance 初始化** → 创建 LuaState → 加载 `helloworld.lua`
2. **地图加载** → 加载 `FirstPersonExampleMap`
3. **GameMode 创建** → `BP_AdventureGameMode` → PostLuaHook
4. **PlayerController 创建** → PostLuaHook
5. **Pawn 创建** → `AAdventureCharacter` → PostLuaHook
6. **Actor 创建** → 各 LuaActor → PostLuaHook
7. **BeginPlay 阶段** → 游戏开始运行

### Lua 脚本结构
```
Content/Lua/
├── helloworld.lua       # 入口脚本，初始化调试器和性能分析
├── LuaPanda.lua         # LuaPanda 调试器库
├── AdventureCharacter.lua
├── LoginPanel.lua
├── LoginWidget.lua
├── MyLuaActor.lua
└── PlayLuaActor.lua
```

### slua_unreal 插件功能
- 自动将 Lua 脚本绑定到 UE4 对象
- 提供 `LuaActor`, `LuaGameMode`, `LuaPlayerController`, `LuaPawn` 基类
- 支持在 Lua 中重写 UE4 事件 (BeginPlay, Tick 等)
- 提供 `slua_profile` 编辑器模块进行性能分析

## Important Constraints
- **引擎版本**: 必须使用 Unreal Engine 4.26，不兼容其他版本
- **平台限制**: 当前仅支持 Windows (Win64) 平台
- **Lua 运行时**: 依赖 slua_unreal 插件的 Lua 5.3+ 运行时
- **调试端口**: LuaPanda 使用 8818 端口，slua_profile 使用 8081 端口
- **编辑器模式**: slua_profile 模块仅在编辑器模式下可用

## External Dependencies

### 插件依赖
| 插件 | 用途 | 加载阶段 |
|------|------|----------|
| slua_unreal | Lua 脚本绑定 | PreLoadingScreen |
| slua_profile | Lua 性能分析 (仅编辑器) | PreDefault |
| lua_wrapper | Lua 封装 | - |

### 第三方库
| 库名 | 路径 | 用途 |
|------|------|------|
| IDbg | `Source/third_party/IDbg/` | 调试支持 |

### 外部工具
| 工具 | 端口 | 用途 |
|------|------|------|
| LuaPanda VSCode 扩展 | 8818 | Lua 远程调试 |
| slua_profile 客户端 | 8081 | 性能分析和内存追踪 |

## Key Files Reference
| 文件 | 用途 |
|------|------|
| `Source/LuaDemo/MyGameInstance.cpp` | 游戏实例，管理 LuaState 生命周期 |
| `Source/LuaDemo/AdventureGameMode.cpp` | 自定义游戏模式 |
| `Source/LuaDemo/AdventureCharacter.cpp` | 自定义角色类 |
| `Source/LuaDemo/MyLuaActor.cpp` | Lua Actor 示例 |
| `Content/Lua/helloworld.lua` | Lua 入口脚本 |
| `Config/DefaultEngine.ini` | 引擎配置 |
| `UE启动流程分析.md` | UE4 启动流程详细文档 |
