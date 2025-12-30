# Passkey Demo

Windows C++ 演示程序，展示如何使用 passkey（Windows Hello）进行登录认证。

## 系统要求

- Windows 10 版本 1809 或更高版本（支持 WebAuthn API）
- Visual Studio 2019 或更高版本
- Windows Hello 已设置（用于实际认证）

## 构建项目

1. 打开 `PasskeyDemo.sln` 文件
2. 选择配置（Debug 或 Release）和平台（x64）
3. 生成解决方案（Build Solution）

## 运行

1. 构建成功后，运行生成的 `PasskeyDemo.exe`
2. 主窗口会显示一个 "Login with Passkey" 按钮
3. 点击按钮会弹出 passkey 认证窗口
4. 在认证窗口中点击 "Authenticate" 按钮
5. 系统会提示使用 Windows Hello 进行认证（PIN、指纹或面部识别）

## 功能说明

- **主窗口**：包含登录按钮的简单窗口
- **Passkey 窗口**：模态对话框，用于执行 passkey 认证
- **WebAuthn 集成**：使用 Windows WebAuthn API 与平台认证器（Windows Hello）交互

## 注意事项

- 这是一个演示程序，实际的 passkey 认证需要：
  - 有效的 RP ID（Relying Party ID）配置
  - 服务器端的挑战生成和验证
  - 凭证 ID 管理
  - 完整的断言验证流程

- 当前实现会检查平台认证器是否可用，并尝试调用 WebAuthn API
- 如果 Windows Hello 未设置，认证会失败并显示相应提示

## 项目结构

```
PasskeyDemo/
├── main.cpp              # 程序入口点
├── MainWindow.h/cpp      # 主窗口实现
├── PasskeyWindow.h/cpp   # Passkey 认证窗口实现
├── resource.h            # 资源定义
├── PasskeyDemo.rc        # 资源文件
└── PasskeyDemo.vcxproj   # Visual Studio 项目文件
```
