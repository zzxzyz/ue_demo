# Lua-C++ 互调机制规范

## Purpose

定义项目中 Lua 脚本与 C++ 代码双向调用的技术规范。该规范覆盖 slua_unreal 插件提供的核心接口、调用模式、委托绑定机制以及生命周期管理，帮助开发者正确使用 Lua-C++ 互调功能。

## Requirements

### Requirement: ILuaOverriderInterface 核心接口
C++ 类 MUST 通过实现 `ILuaOverriderInterface` 接口来支持与 Lua 脚本的双向调用。该接口定义在 `Plugins/slua_unreal/Source/slua_unreal/Public/LuaOverriderInterface.h`。

接口 SHALL 提供以下核心方法：
- `GetLuaFilePath()`: 返回关联的 Lua 文件路径
- `GetSelfTable()`: 获取 Lua 中对应的 self 表
- `CallLuaFunction<RET>(FunctionName, Args...)`: 调用 Lua 函数并返回结果
- `CallLuaFunctionIfExist(FunctionName, Args...)`: 如果函数存在则调用
- `IsLuaFunctionExist(FunctionName)`: 检查 Lua 函数是否存在

#### Scenario: C++ 类继承 ILuaOverriderInterface
- **GIVEN** 一个继承自 `AActor` 的 C++ 类
- **WHEN** 该类同时实现 `ILuaOverriderInterface` 接口
- **THEN** 该类可以通过 `LuaFilePath` 属性关联 Lua 脚本
- **AND** 可以在适当的生命周期方法中调用 Lua 函数

#### Scenario: 检查并调用 Lua 函数
- **GIVEN** 一个实现了 `ILuaOverriderInterface` 的 C++ 对象
- **WHEN** 调用 `IsLuaFunctionExist(TEXT("NativeConstruct"))`
- **THEN** 返回 `true` 如果 Lua 脚本中定义了 `NativeConstruct` 函数
- **AND** 可以使用 `CallLuaFunctionIfExist(TEXT("NativeConstruct"))` 安全调用

---

### Requirement: C++ 调用 Lua 函数
C++ 代码 SHALL 通过 `ILuaOverriderInterface` 提供的模板方法调用 Lua 函数。系统 MUST 支持带返回值和不带返回值两种形式。

关键文件：
- `Source/LuaDemo/LoginWidget.cpp`: 在 `NativeConstruct()` 中调用 Lua 的 `NativeConstruct`

#### Scenario: 无返回值的 Lua 函数调用
- **GIVEN** C++ 类 `ULoginWidget` 实现了 `ILuaOverriderInterface`
- **AND** Lua 文件 `LoginWidget.lua` 定义了 `NativeConstruct` 函数
- **WHEN** C++ 代码执行 `CallLuaFunctionIfExist(TEXT("NativeConstruct"))`
- **THEN** Lua 中的 `NativeConstruct` 函数被调用
- **AND** `self` 参数自动传递给 Lua 函数

#### Scenario: 带返回值的 Lua 函数调用
- **GIVEN** Lua 函数返回一个布尔值
- **WHEN** C++ 代码执行 `CallLuaFunction<bool>(TEXT("SomeLuaFunc"))`
- **THEN** 返回值被正确转换为 C++ 的 `bool` 类型

---

### Requirement: Lua 调用 C++ 类和函数
Lua 脚本 SHALL 通过 `import()` 全局函数导入 C++ 类，然后可以创建实例或调用静态方法。

关键实现：
- `Plugins/slua_unreal/Source/slua_unreal/Private/LuaState.cpp` 中的 `import()` 函数

系统 MUST 支持导入以下类型：
- `UClass`: C++ 类（包括 Actor、Widget、BlueprintFunctionLibrary 等）
- `UScriptStruct`: 结构体类型
- `UEnum`: 枚举类型

#### Scenario: 导入蓝图函数库
- **GIVEN** C++ 定义了 `UUIHelper` 蓝图函数库类
- **AND** 该类标记为 `UCLASS()` 并包含 `UFUNCTION(BlueprintCallable)` 方法
- **WHEN** Lua 代码执行 `local UIHelper = import("UIHelper")`
- **THEN** 可以调用 `UIHelper.SetInputModeGameAndUI(playerController, widget)`

#### Scenario: 导入缓存机制
- **GIVEN** 之前已经执行过 `import("UIHelper")`
- **WHEN** 再次执行 `import("UIHelper")`
- **THEN** 直接从缓存返回，不重新查找类

---

### Requirement: Lua 访问 C++ 对象属性
Lua 脚本 SHALL 能够直接访问通过 `UPROPERTY()` 宏标记的 C++ 属性，包括绑定的 UI 控件。

#### Scenario: 访问绑定的 UI 控件
- **GIVEN** C++ 类 `ULoginWidget` 有属性 `UPROPERTY(meta = (BindWidget)) class UButton* CancelButton`
- **WHEN** Lua 代码访问 `self.CancelButton`
- **THEN** 获取到 C++ 中绑定的 `UButton` 对象引用

#### Scenario: 使用 FindWidget 查找控件
- **GIVEN** Widget 层次结构中存在名为 "EmailTextBox" 的控件
- **WHEN** Lua 代码执行 `self.EmailTextBox = self:FindWidget("EmailTextBox")`
- **THEN** 获取到对应的控件引用

---

### Requirement: Lua 绑定 UE4 委托
Lua MUST 能够绑定 UE4 的多播委托（如按钮点击事件），使用 `Add()` 方法添加回调函数。

关键文件：
- `Content/Lua/LoginPanel.lua`: 演示按钮点击事件绑定
- `Content/Lua/LoginWidget.lua`: 演示委托绑定

#### Scenario: 绑定按钮点击事件
- **GIVEN** Lua 获取到 `UButton` 控件引用
- **WHEN** 执行 `self.LoginButton.OnClicked:Add(function() ... end)`
- **THEN** 按钮点击时触发 Lua 回调函数

#### Scenario: 清除已有委托绑定
- **GIVEN** 按钮已有其他绑定
- **WHEN** 执行 `self.LoginButton.OnClicked:Clear()` 后再 `Add()`
- **THEN** 旧的绑定被移除，只保留新的回调

---

### Requirement: Lua 脚本类定义规范
Lua 脚本 MUST 使用特定的模式定义可被 C++ 调用的类，通过 `return Class(nil, nil, table)` 返回类定义。

关键模式：
```lua
local MyClass = {}

function MyClass:SomeMethod()
    -- 方法实现
end

return Class(nil, nil, MyClass)
```

#### Scenario: 标准 Lua 脚本类定义
- **GIVEN** Lua 文件 `MyLuaActor.lua` 按照标准模式定义了 `actor` 表
- **AND** 文件末尾返回 `Class(nil, nil, actor)`
- **WHEN** C++ 的 `AMyLuaActor` 加载此脚本
- **THEN** Lua 表中的方法可以被 C++ 调用

#### Scenario: 重写 UE4 生命周期方法
- **GIVEN** Lua 脚本定义了 `ReceiveBeginPlay` 方法
- **WHEN** UE4 Actor 进入 BeginPlay 阶段
- **THEN** Lua 中的 `ReceiveBeginPlay` 方法被调用
- **AND** 可以通过 `self.Super:ReceiveBeginPlay()` 调用父类方法

---

### Requirement: LuaState 生命周期管理
`NS_SLUA::LuaState` SHALL 管理 Lua 虚拟机的生命周期，由 `UMyGameInstance` 负责创建和销毁。

关键文件：
- `Source/LuaDemo/MyGameInstance.cpp`: LuaState 的创建和管理
- `Source/LuaDemo/MyGameInstance.h`: GameInstance 类定义

#### Scenario: LuaState 创建
- **GIVEN** 游戏启动，`UMyGameInstance` 被实例化
- **WHEN** 构造函数执行
- **THEN** 创建 `NS_SLUA::LuaState` 实例
- **AND** 设置 Lua 文件加载委托（指向 `Content/Lua/` 目录）
- **AND** 调用 `state->init()` 初始化 Lua 环境
- **AND** 加载入口脚本 `helloworld.lua`

#### Scenario: LuaState 销毁
- **GIVEN** 游戏关闭，`UMyGameInstance::Shutdown()` 被调用
- **WHEN** `CloseLuaState()` 执行
- **THEN** 调用 `state->close()` 关闭 Lua 虚拟机
- **AND** 释放 LuaState 内存

---

### Requirement: slua_unreal 预定义基类
slua_unreal 插件 SHALL 提供一系列预定义的 Lua 绑定基类，简化 C++ 类与 Lua 的集成。

可用基类（定义在 `Plugins/slua_unreal/Source/slua_unreal/Public/`）：
- `ALuaActor`: Lua 绑定的 Actor 基类
- `ALuaCharacter`: Lua 绑定的 Character 基类
- `ALuaPawn`: Lua 绑定的 Pawn 基类
- `ALuaGameMode`: Lua 绑定的 GameMode 基类
- `ALuaPlayerController`: Lua 绑定的 PlayerController 基类
- `ALuaPlayerState`: Lua 绑定的 PlayerState 基类
- `ULuaUserWidget`: Lua 绑定的 UserWidget 基类

#### Scenario: 使用 ALuaActor 基类
- **GIVEN** C++ 类 `AMyLuaActor` 继承自 `ALuaActor`
- **WHEN** 在构造函数中设置 `LuaFilePath = "MyLuaActor"`
- **THEN** Actor 初始化时自动加载 `Content/Lua/MyLuaActor.lua`
- **AND** Lua 脚本可以重写 `ReceiveBeginPlay`、`Tick` 等方法

#### Scenario: 自定义实现 ILuaOverriderInterface
- **GIVEN** 需要为不在预定义列表中的类添加 Lua 支持
- **WHEN** C++ 类同时继承目标基类和 `ILuaOverriderInterface`
- **THEN** 需要手动实现 `GetLuaFilePath_Implementation()`
- **AND** 需要在适当位置调用 `PostLuaHook()` 或 `TryHook()`
