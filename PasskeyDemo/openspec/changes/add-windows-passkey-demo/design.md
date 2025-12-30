# Design: Windows C++ Passkey Demo

## Context
需要创建一个简单的 Windows 桌面应用程序，演示 passkey 登录功能。该 demo 主要用于学习和测试目的，需要能够在 Visual Studio 中方便地开发和调试。

## Goals / Non-Goals

### Goals
- 提供可编译运行的 Windows C++ 应用程序
- 实现基本的 passkey 认证流程
- 提供清晰的 UI 界面（主窗口 + 登录按钮）
- 支持 Visual Studio 开发和调试

### Non-Goals
- 完整的用户管理系统
- 复杂的错误处理和重试机制
- 多平台支持（仅 Windows）
- 生产级别的安全性（demo 用途）

## Decisions

### Decision: 使用 Windows API 和 WebAuthn
- **What**: 使用 Windows API（Win32）创建窗口，使用 Windows WebAuthn API 实现 passkey 功能
- **Why**: 
  - Windows 平台原生支持 WebAuthn API（通过 webauthn.dll）
  - Win32 API 提供最直接的控制和最小的依赖
  - Visual Studio 原生支持 Win32 项目
- **Alternatives considered**:
  - Qt/WxWidgets: 增加依赖，对于简单 demo 过于复杂
  - .NET: 不符合 C++ 要求
  - MFC: 较老的框架，现代项目较少使用

### Decision: 使用 Visual Studio 项目格式
- **What**: 创建 .sln 和 .vcxproj 文件
- **Why**: 
  - Visual Studio 是 Windows C++ 开发的标准 IDE
  - .sln/.vcxproj 格式广泛支持
  - 便于调试和开发
- **Alternatives considered**:
  - CMake: 增加构建系统复杂度，不符合直接可用的要求
  - Makefile: Windows 上不够友好

### Decision: 简单的窗口结构
- **What**: 主窗口包含一个登录按钮，点击后弹出 passkey 窗口
- **Why**: 
  - 符合 demo 的简单性要求
  - 清晰的用户流程
  - 易于理解和维护
- **Alternatives considered**:
  - 内嵌 passkey UI: 增加复杂度，不符合"弹出窗口"的要求

## Risks / Trade-offs

### Risk: WebAuthn API 兼容性
- **Risk**: 不同 Windows 版本对 WebAuthn 的支持可能不同
- **Mitigation**: 在代码中添加版本检查，提供清晰的错误提示

### Risk: 依赖 Windows 10/11 特定功能
- **Risk**: Passkey 功能可能需要较新的 Windows 版本
- **Mitigation**: 在文档中明确最低系统要求

### Trade-off: 简单性 vs 完整性
- **Trade-off**: 选择简单实现，可能缺少一些边缘情况处理
- **Acceptance**: Demo 用途，简单性优先

## Migration Plan
N/A - 新功能，无需迁移

## Open Questions
- Passkey 是否需要先注册流程，还是仅演示登录？
  - **Decision**: 根据实际需求，可以先实现登录流程，注册流程可作为后续扩展
