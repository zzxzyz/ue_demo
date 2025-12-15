# IDbg 第三方库引用说明

## 目录结构

```
Assets/Plugins/IDbg/
├── IDbg.dll          # Windows 原生 DLL（64位）
├── IDbg.dll.meta     # Unity 插件配置
└── IDbgWrapper.cs    # C# 包装类
```

## 配置说明

### 1. DLL 文件位置

IDbg.dll 已放置在 `Assets/Plugins/IDbg/` 目录下。Unity 会自动识别并加载。

### 2. 平台设置

在 Unity Editor 中：
1. 选择 `Assets/Plugins/IDbg/IDbg.dll`
2. 在 Inspector 面板中配置平台设置：
   - **Editor**: 启用，CPU 选择 `x86_64`（Windows 64位）
   - **Standalone**: Win64 启用
   - 其他平台根据需要配置

### 3. C# 包装类

`IDbgWrapper.cs` 提供了 C# 接口来调用 IDbg 的原生函数。

**注意**：由于 C++ 函数可能使用了 name mangling，实际的导出函数名可能与代码中的不同。需要：

1. 使用工具查看 DLL 的实际导出函数：
   ```powershell
   dumpbin /exports IDbg.dll
   ```
   或使用 Dependency Walker 等工具

2. 根据实际导出函数名调整 `DllImport` 的 `EntryPoint`：
   ```csharp
   [DllImport("IDbg", EntryPoint = "实际的函数名", CallingConvention = CallingConvention.Cdecl)]
   ```

## 使用方法

### 基本使用

```csharp
using IDbg;

// 初始化
if (IDbgWrapper.Initialize())
{
    Debug.Log("IDbg 初始化成功");
    
    // 设置日志回调
    IDbgWrapper.LogDelegate logCallback = (level, msg) => {
        Debug.Log($"[IDbg {level}] {msg}");
    };
    IDbgWrapper.IDbg_Log_SetDelegate(logCallback);
    
    // 使用其他功能...
}
```

### 检查 DLL 是否可用

```csharp
if (IDbgWrapper.IsAvailable())
{
    // DLL 已加载，可以使用
}
else
{
    // DLL 未找到或加载失败
}
```

## 注意事项

1. **函数名映射**：C++ 导出函数可能使用了 name mangling，需要查看实际的导出函数名
2. **调用约定**：确保 `CallingConvention` 与 DLL 的调用约定一致（通常是 `Cdecl`）
3. **平台支持**：目前只配置了 Windows 64位，如需其他平台，需要相应的 DLL 文件
4. **依赖项**：确保 IDbg.dll 的所有依赖项都在系统路径或 Unity 可访问的位置

## 查看 DLL 导出函数

### Windows 方法 1：使用 dumpbin（Visual Studio 工具）

```powershell
# 在 Visual Studio Developer Command Prompt 中运行
dumpbin /exports IDbg.dll > exports.txt
```

### Windows 方法 2：使用 Dependency Walker

下载 Dependency Walker，打开 IDbg.dll 查看导出函数列表。

### Windows 方法 3：使用 PowerShell

```powershell
# 需要安装 .NET Framework SDK
[System.Reflection.Assembly]::LoadFile("IDbg.dll")
```

## 常见问题

### Q: DLL 加载失败
- 检查 DLL 文件是否在正确位置
- 检查平台设置是否正确
- 检查是否有依赖的 DLL 缺失

### Q: 函数调用失败
- 检查函数名是否正确（考虑 name mangling）
- 检查参数类型和调用约定
- 使用 `EntryPoint` 指定实际的导出函数名

### Q: 需要支持其他平台
- 将对应平台的库文件放到相应的目录：
  - Android: `Assets/Plugins/Android/libs/`
  - iOS: `Assets/Plugins/iOS/`
  - macOS: `Assets/Plugins/`

