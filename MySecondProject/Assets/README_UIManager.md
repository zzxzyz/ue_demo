# Unity 跨场景 UI 管理方案

## 概述

本文档说明如何创建独立于 Scene 的 UI 元素，使其可以在多个 Scene 中使用。

## 方案一：使用 UIManager 单例（推荐）

### 1. 创建 UIManager

1. 在 Unity 编辑器中创建一个空的 GameObject，命名为 `UIManager`
2. 将 `UIManager.cs` 脚本挂载到该 GameObject 上
3. 在 Inspector 中设置 `Login Panel Prefab` 字段（拖拽登录面板的 Prefab）

### 2. 创建登录面板 Prefab

1. 在场景中创建登录面板 UI（Canvas、Panel、输入框、按钮等）
2. 确保登录面板的根 GameObject 命名为 `LoginPanel`
3. 将整个登录面板拖拽到 `Assets` 文件夹，创建 Prefab
4. 从场景中删除登录面板（因为会通过 UIManager 动态加载）

### 3. 配置 UIManager

1. 选择 `UIManager` GameObject
2. 在 Inspector 中：
   - 将登录面板 Prefab 拖拽到 `Login Panel Prefab` 字段
   - `UI Canvas` 字段会自动创建（如果为空）

### 4. 使用方式

#### C# 代码中：

```csharp
// 显示登录面板
UIManager.Instance.ShowLoginPanel();

// 隐藏登录面板
UIManager.Instance.HideLoginPanel();

// 显示自定义 UI
UIManager.Instance.ShowUI("SettingsPanel", settingsPanelPrefab);

// 获取 UI 实例
GameObject loginPanel = UIManager.Instance.GetUI("LoginPanel");
```

#### Lua 代码中：

```lua
-- 显示登录面板
UIManager.ShowLoginPanel()

-- 隐藏登录面板
UIManager.HideLoginPanel()

-- 获取 UI 实例
local loginPanel = UIManager.GetUI("LoginPanel")
```

## 方案二：使用 DontDestroyOnLoad

### 1. 创建独立的 UI Canvas

1. 在场景中创建 Canvas
2. 设置 Canvas 的 Render Mode 为 `Screen Space - Overlay`
3. 设置 Sorting Order 为一个较高的值（如 100）
4. 创建登录面板作为 Canvas 的子对象

### 2. 添加 DontDestroyOnLoad 脚本

```csharp
using UnityEngine;

public class PersistentUI : MonoBehaviour
{
    void Awake()
    {
        DontDestroyOnLoad(gameObject);
    }
}
```

将此脚本挂载到 Canvas 的根 GameObject 上。

### 3. 注意事项

- 确保每个场景中只有一个这样的 Canvas
- 使用单例模式避免重复创建

## 方案三：使用 Resources 或 Addressables 动态加载

### 1. 将 UI Prefab 放在 Resources 文件夹

```
Assets/
  Resources/
    UI/
      LoginPanel.prefab
      SettingsPanel.prefab
```

### 2. 动态加载代码

```csharp
// 加载并实例化
GameObject prefab = Resources.Load<GameObject>("UI/LoginPanel");
GameObject instance = Instantiate(prefab, canvas.transform);
```

## 与 Lua 集成

### 1. 更新 LoginManager Lua 脚本

Lua 脚本已经更新，支持从 UIManager 获取 LoginPanel：

```lua
-- 优先从 UIManager 获取
local uiManager = GameObject.Find("UIManager")
if uiManager then
    local manager = uiManager:GetComponent("UIManager")
    loginPanel = manager:GetUI("LoginPanel")
end

-- 如果 UIManager 中没有，则在场景中查找（向后兼容）
if not loginPanel then
    loginPanel = GameObject.Find("LoginPanel")
end
```

### 2. 初始化流程

1. 在第一个场景中创建 `UIManager` GameObject
2. `UIManager` 会自动设置为 `DontDestroyOnLoad`
3. 切换场景时，`UIManager` 和其 UI Canvas 会保留
4. 在任何场景中都可以调用 `UIManager.Instance.ShowLoginPanel()`

## 最佳实践

1. **使用 Prefab**：所有跨场景 UI 都应该创建为 Prefab
2. **单例模式**：使用 UIManager 单例管理所有跨场景 UI
3. **命名规范**：UI 名称保持一致，便于查找和管理
4. **内存管理**：不需要的 UI 及时销毁，避免内存泄漏
5. **Canvas 设置**：
   - Render Mode: `Screen Space - Overlay`
   - Sorting Order: 设置较高值（如 100）
   - Canvas Scaler: 配置适配不同分辨率

## 示例场景

### 场景 A（主菜单）
- 包含 UIManager
- 可以显示登录面板

### 场景 B（游戏场景）
- 不包含登录面板
- 但可以通过 UIManager 显示登录面板（如果需要）

### 场景 C（设置场景）
- 不包含登录面板
- 但可以通过 UIManager 显示登录面板（如果需要）

## 注意事项

1. **Canvas 层级**：确保跨场景 UI 的 Canvas Sorting Order 高于场景内的 UI
2. **事件系统**：确保场景中有 EventSystem，否则 UI 无法交互
3. **初始化顺序**：UIManager 应该在场景加载前初始化
4. **清理资源**：切换场景时，确保清理不需要的 UI 实例

## 故障排除

### 问题：UI 不显示
- 检查 UIManager 是否已初始化
- 检查 Prefab 是否正确设置
- 检查 Canvas 的 Render Mode 设置

### 问题：UI 在场景切换时消失
- 确保 UIManager 使用了 DontDestroyOnLoad
- 检查是否有多个 UIManager 实例

### 问题：UI 无法交互
- 检查场景中是否有 EventSystem
- 检查 Canvas 的 GraphicRaycaster 组件是否存在

