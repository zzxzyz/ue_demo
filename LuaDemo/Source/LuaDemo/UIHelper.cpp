// Fill out your copyright notice in the Description page of Project Settings.

#include "UIHelper.h"
#include "GameFramework/PlayerController.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"

// Windows API 头文件（用于 Passkey 认证）
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <string>
#include "Windows/HideWindowsPlatformTypes.h"

// 获取窗口句柄所需头文件
#include "Engine/GameViewportClient.h"
#include "Widgets/SWindow.h"
#include "GenericPlatform/GenericWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/PlatformProcess.h"



// Static variable to hold the temporary window handle for fullscreen mode
static HWND g_hTempWebAuthNWindow = NULL;

// Window procedure for temporary WebAuthN window
static LRESULT CALLBACK TempWebAuthNWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Helper function to create a temporary topmost window for WebAuthN dialogs in fullscreen mode
static HWND CreateTempWebAuthNWindow() {
    if (g_hTempWebAuthNWindow != NULL && IsWindow(g_hTempWebAuthNWindow)) {
        UE_LOG(LogTemp, Log, TEXT("[PassKey] CreateTempWebAuthNWindow: Reusing existing temporary window 0x%p"), g_hTempWebAuthNWindow);
        return g_hTempWebAuthNWindow;
    }
    
    UE_LOG(LogTemp, Log, TEXT("[PassKey] CreateTempWebAuthNWindow: Creating temporary window for WebAuthN dialog"));
    
    // Register window class if not already registered
    static bool classRegistered = false;
    const wchar_t* className = L"INTLWebAuthNTempWindow";
    
    if (!classRegistered) {
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = TempWebAuthNWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = className;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        
        if (RegisterClassExW(&wc) == 0) {
            DWORD error = GetLastError();
            if (error != ERROR_CLASS_ALREADY_EXISTS) {
                UE_LOG(LogTemp, Error, TEXT("[PassKey] CreateTempWebAuthNWindow: RegisterClassExW failed, error = 0x%08X"), error);
                return NULL;
            }
        }
        classRegistered = true;
        UE_LOG(LogTemp, Log, TEXT("[PassKey] CreateTempWebAuthNWindow: Window class registered"));
    }
    
    // Get primary monitor dimensions to center the window
    // Use primary monitor to ensure dialog appears on the main screen
    HMONITOR hMonitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo = { 0 };
    monitorInfo.cbSize = sizeof(MONITORINFO);
    
    int windowX = 0;
    int windowY = 0;
    int windowWidth = 1;
    int windowHeight = 1;
    
    if (hMonitor != NULL && GetMonitorInfo(hMonitor, &monitorInfo)) {
        RECT monitorRect = monitorInfo.rcMonitor;
        windowX = (monitorRect.left + monitorRect.right) / 2;
        windowY = (monitorRect.top + monitorRect.bottom) / 2;
        UE_LOG(LogTemp, Log, TEXT("[PassKey] CreateTempWebAuthNWindow: Primary monitor rect = (%ld, %ld, %ld, %ld), window position = (%d, %d)"), 
            monitorRect.left, monitorRect.top, monitorRect.right, monitorRect.bottom, windowX, windowY);
    } else {
        // Fallback to system metrics if monitor info fails
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        windowX = screenWidth / 2;
        windowY = screenHeight / 2;
        UE_LOG(LogTemp, Log, TEXT("[PassKey] CreateTempWebAuthNWindow: Using system metrics, screen size = %dx%d, window position = (%d, %d)"), 
            screenWidth, screenHeight, windowX, windowY);
    }
    
    g_hTempWebAuthNWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        className,
        L"INTL WebAuthN Temp Window",
        WS_POPUP,
        windowX, windowY, windowWidth, windowHeight,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (g_hTempWebAuthNWindow == NULL) {
        DWORD error = GetLastError();
        UE_LOG(LogTemp, Error, TEXT("[PassKey] CreateTempWebAuthNWindow: CreateWindowExW failed, error = 0x%08X"), error);
        return NULL;
    }
    
    // Make sure it's topmost and visible (but minimal size to be unobtrusive)
    SetWindowPos(g_hTempWebAuthNWindow, HWND_TOPMOST, windowX, windowY, windowWidth, windowHeight, 
        SWP_NOACTIVATE | SWP_SHOWWINDOW);
    
    UE_LOG(LogTemp, Log, TEXT("[PassKey] CreateTempWebAuthNWindow: Temporary window created successfully, handle = 0x%p, position = (%d, %d)"), 
        g_hTempWebAuthNWindow, windowX, windowY);
    return g_hTempWebAuthNWindow;
}

// 静态变量：保存切换前的原始窗口模式
static EWindowMode::Type OriginalWindowMode = EWindowMode::Windowed;

void UUIHelper::SetInputModeGameAndUI(APlayerController* PlayerController, UWidget* WidgetToFocus)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::SetInputModeGameAndUI - PlayerController is null"));
		return;
	}

	// 显示鼠标光标
	PlayerController->bShowMouseCursor = true;

	// 设置输入模式为 Game And UI
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	// 如果提供了 Widget，设置焦点
	if (WidgetToFocus)
	{
		InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
	}
	
	PlayerController->SetInputMode(InputMode);

	UE_LOG(LogTemp, Log, TEXT("UIHelper::SetInputModeGameAndUI - Input mode set to GameAndUI, mouse cursor visible"));
}

void UUIHelper::SetInputModeGameOnly(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::SetInputModeGameOnly - PlayerController is null"));
		return;
	}

	// 隐藏鼠标光标
	PlayerController->bShowMouseCursor = false;

	// 设置输入模式为 Game Only
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);

	UE_LOG(LogTemp, Log, TEXT("UIHelper::SetInputModeGameOnly - Input mode set to GameOnly, mouse cursor hidden"));
}

void UUIHelper::SetInputModeUIOnly(APlayerController* PlayerController, UWidget* WidgetToFocus)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::SetInputModeUIOnly - PlayerController is null"));
		return;
	}

	// 显示鼠标光标
	PlayerController->bShowMouseCursor = true;

	// 设置输入模式为 UI Only
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	// 如果提供了 Widget，设置焦点
	if (WidgetToFocus)
	{
		InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
	}
	
	PlayerController->SetInputMode(InputMode);

	UE_LOG(LogTemp, Log, TEXT("UIHelper::SetInputModeUIOnly - Input mode set to UIOnly, mouse cursor visible"));
}

int32 UUIHelper::GetWindowMode()
{
	if (!GEngine)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::GetWindowMode - GEngine is null"));
		return 2; // 默认返回窗口模式
	}

	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::GetWindowMode - GameUserSettings is null"));
		return 2;
	}

	EWindowMode::Type WindowMode = UserSettings->GetFullscreenMode();
	return static_cast<int32>(WindowMode);
}

void UUIHelper::SetWindowMode(int32 Mode)
{
	if (!GEngine)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::SetWindowMode - GEngine is null"));
		return;
	}

	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::SetWindowMode - GameUserSettings is null"));
		return;
	}

	EWindowMode::Type NewMode = static_cast<EWindowMode::Type>(FMath::Clamp(Mode, 0, 2));
	
	// 获取当前分辨率
	FIntPoint Resolution = UserSettings->GetScreenResolution();
	
	// 设置新的窗口模式
	UserSettings->SetFullscreenMode(NewMode);
	UserSettings->SetScreenResolution(Resolution);
	UserSettings->ApplySettings(false);

	UE_LOG(LogTemp, Log, TEXT("UIHelper::SetWindowMode - Window mode set to %d (%s)"), 
		Mode, *GetWindowModeString(Mode));
}

int32 UUIHelper::ToggleWindowMode()
{
	if (!GEngine)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::ToggleWindowMode - GEngine is null"));
		return 2;
	}

	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIHelper::ToggleWindowMode - GameUserSettings is null"));
		return 2;
	}

	EWindowMode::Type CurrentMode = UserSettings->GetFullscreenMode();
	EWindowMode::Type NewMode;

	// 如果当前是无边框窗口模式，切换回原来的模式
	if (CurrentMode == EWindowMode::WindowedFullscreen)
	{
		NewMode = OriginalWindowMode;
		UE_LOG(LogTemp, Log, TEXT("UIHelper::ToggleWindowMode - Switching from WindowedFullscreen to original mode: %d"), 
			static_cast<int32>(OriginalWindowMode));
	}
	else
	{
		// 保存当前模式，然后切换到无边框窗口
		OriginalWindowMode = CurrentMode;
		NewMode = EWindowMode::WindowedFullscreen;
		UE_LOG(LogTemp, Log, TEXT("UIHelper::ToggleWindowMode - Switching from %d to WindowedFullscreen"), 
			static_cast<int32>(CurrentMode));
	}

	// 获取当前分辨率
	FIntPoint Resolution = UserSettings->GetScreenResolution();
	
	// 应用新模式
	UserSettings->SetFullscreenMode(NewMode);
	UserSettings->SetScreenResolution(Resolution);
	UserSettings->ApplySettings(false);

	return static_cast<int32>(NewMode);
}

FString UUIHelper::GetWindowModeString(int32 Mode)
{
	switch (static_cast<EWindowMode::Type>(Mode))
	{
	case EWindowMode::Fullscreen:
		return TEXT("全屏模式 (Fullscreen)");
	case EWindowMode::WindowedFullscreen:
		return TEXT("无边框窗口 (Borderless)");
	case EWindowMode::Windowed:
		return TEXT("窗口模式 (Windowed)");
	default:
		return TEXT("未知模式 (Unknown)");
	}
}

// 打印当前窗口显示模式（同时输出到日志和屏幕）
void UUIHelper::PrintWindowMode()
{
	if (GEngine && GEngine->GameViewport)
	{
		// 屏幕显示的持续时间（秒）
		const float DisplayTime = 10.0f;
		// 屏幕显示的颜色
		const FColor TitleColor = FColor::Yellow;
		const FColor InfoColor = FColor::Green;
		
		int32 MsgKey = 100; // 消息的唯一标识，用于更新同一位置的消息
		
		// 方式1：从 GameUserSettings 获取
		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		if (UserSettings)
		{
			EWindowMode::Type WindowMode = UserSettings->GetFullscreenMode();
			FIntPoint Resolution = UserSettings->GetScreenResolution();
			
			// 输出到日志
			UE_LOG(LogTemp, Warning, TEXT("========== 窗口显示模式信息 =========="));
			UE_LOG(LogTemp, Warning, TEXT("窗口模式: %s"), *UUIHelper::GetWindowModeString(WindowMode));
			UE_LOG(LogTemp, Warning, TEXT("分辨率: %d x %d"), Resolution.X, Resolution.Y);
			UE_LOG(LogTemp, Warning, TEXT("是否使用 VSync: %s"), UserSettings->IsVSyncEnabled() ? TEXT("是") : TEXT("否"));
			UE_LOG(LogTemp, Warning, TEXT("====================================="));
			
			// 输出到屏幕
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, TitleColor, TEXT("========== 窗口显示模式信息 =========="));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口模式: %s"), *UUIHelper::GetWindowModeString(WindowMode)));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("分辨率: %d x %d"), Resolution.X, Resolution.Y));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("VSync: %s"), UserSettings->IsVSyncEnabled() ? TEXT("是") : TEXT("否")));
		}
		
		// 方式2：从实际窗口获取
		TSharedPtr<SWindow> GameWindow = GEngine->GameViewport->GetWindow();
		if (GameWindow.IsValid())
		{
			EWindowMode::Type ActualMode = GameWindow->GetWindowMode();
			FVector2D WindowSize = GameWindow->GetSizeInScreen();
			FVector2D WindowPos = GameWindow->GetPositionInScreen();
			
			// 输出到日志
			UE_LOG(LogTemp, Warning, TEXT("========== 实际窗口状态 =========="));
			UE_LOG(LogTemp, Warning, TEXT("实际窗口模式: %s"), *UUIHelper::GetWindowModeString(ActualMode));
			UE_LOG(LogTemp, Warning, TEXT("窗口大小: %.0f x %.0f"), WindowSize.X, WindowSize.Y);
			UE_LOG(LogTemp, Warning, TEXT("窗口位置: (%.0f, %.0f)"), WindowPos.X, WindowPos.Y);
			UE_LOG(LogTemp, Warning, TEXT("窗口标题: %s"), *GameWindow->GetTitle().ToString());
			UE_LOG(LogTemp, Warning, TEXT("=================================="));
			
			// 输出到屏幕
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, TitleColor, TEXT("========== 实际窗口状态 =========="));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("实际窗口模式: %s"), *UUIHelper::GetWindowModeString(ActualMode)));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口大小: %.0f x %.0f"), WindowSize.X, WindowSize.Y));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口位置: (%.0f, %.0f)"), WindowPos.X, WindowPos.Y));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口标题: %s"), *GameWindow->GetTitle().ToString()));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("无法获取窗口信息：GEngine 或 GameViewport 未初始化"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("无法获取窗口信息：GameViewport 未初始化"));
		}
	}
}

bool UUIHelper::PerformPasskeyAuthentication()
{
    // Check if WebAuthn API is available
    HMODULE hWebAuthn = LoadLibrary(L"webauthn.dll");
    if (!hWebAuthn)
    {
        UE_LOG(LogTemp, Warning, TEXT("WebAuthn API not available on this system"));
        return false;
    }

    // Use the official webauthn.h structures
    // WEBAUTHN_CLIENT_DATA structure
    struct WEBAUTHN_CLIENT_DATA_LOCAL {
        DWORD dwVersion;              // Version of this structure
        DWORD cbClientDataJSON;       // Size of the pbClientDataJSON
        PBYTE pbClientDataJSON;       // UTF-8 encoded JSON
        LPCWSTR pwszHashAlgId;        // Hash algorithm ID
    };

    // Credential structure
    struct WEBAUTHN_CREDENTIAL_LOCAL {
        DWORD dwVersion;
        DWORD cbId;
        PBYTE pbId;
        LPCWSTR pwszCredentialType;
    };

    // Credentials list
    struct WEBAUTHN_CREDENTIALS_LOCAL {
        DWORD cCredentials;
        WEBAUTHN_CREDENTIAL_LOCAL* pCredentials;
    };

    // Extensions
    struct WEBAUTHN_EXTENSIONS_LOCAL {
        DWORD cExtensions;
        PVOID pExtensions;
    };

    // WEBAUTHN_AUTHENTICATOR_GET_ASSERTION_OPTIONS - Version 1 (simplest)
    struct WEBAUTHN_GET_ASSERTION_OPTIONS_V1 {
        DWORD dwVersion;                       // 1
        DWORD dwTimeoutMilliseconds;
        WEBAUTHN_CREDENTIALS_LOCAL CredentialList;
        WEBAUTHN_EXTENSIONS_LOCAL Extensions;
        DWORD dwAuthenticatorAttachment;
        DWORD dwUserVerificationRequirement;
        DWORD dwFlags;
    };

    // Get function pointer
    typedef HRESULT(WINAPI* PFN_WEBAUTHN_GET_ASSERTION)(
        HWND hWnd,
        LPCWSTR pwszRpId,
        WEBAUTHN_CLIENT_DATA_LOCAL* pWebAuthNClientData,
        WEBAUTHN_GET_ASSERTION_OPTIONS_V1* pWebAuthNGetAssertionOptions,
        PVOID* ppWebAuthNAssertion
    );

    typedef VOID(WINAPI* PFN_WEBAUTHN_FREE_ASSERTION)(PVOID pWebAuthNAssertion);

    PFN_WEBAUTHN_GET_ASSERTION pfnGetAssertion = 
        (PFN_WEBAUTHN_GET_ASSERTION)GetProcAddress(hWebAuthn, "WebAuthNAuthenticatorGetAssertion");
    PFN_WEBAUTHN_FREE_ASSERTION pfnFreeAssertion = 
        (PFN_WEBAUTHN_FREE_ASSERTION)GetProcAddress(hWebAuthn, "WebAuthNFreeAssertion");

    if (!pfnGetAssertion)
    {
        FreeLibrary(hWebAuthn);
        UE_LOG(LogTemp, Warning, TEXT("WebAuthNAuthenticatorGetAssertion not available"));
        return false;
    }

    // RP ID - this is the domain shown in the passkey dialog
    LPCWSTR rpId = L"levelinfinite.com";

    // Client data JSON (UTF-8 encoded)
    std::string clientDataJsonStr = "{\"type\":\"webauthn.get\",\"challenge\":\"AQIDBAUGBwgJCgsMDQ4PEBESExQVFhcYGRobHB0eHyA\",\"origin\":\"https://levelinfinite.com\",\"crossOrigin\":false}";
    
    // Set up client data
    WEBAUTHN_CLIENT_DATA_LOCAL clientData = {};
    clientData.dwVersion = 1;  // WEBAUTHN_CLIENT_DATA_CURRENT_VERSION
    clientData.cbClientDataJSON = (DWORD)clientDataJsonStr.size();
    clientData.pbClientDataJSON = (PBYTE)clientDataJsonStr.c_str();
    clientData.pwszHashAlgId = L"SHA-256";

    // Set up assertion options (Version 1 - simplest form)
    WEBAUTHN_GET_ASSERTION_OPTIONS_V1 options = {};
    options.dwVersion = 1;
    options.dwTimeoutMilliseconds = 60000;  // 60 seconds
    options.CredentialList.cCredentials = 0;
    options.CredentialList.pCredentials = nullptr;
    options.Extensions.cExtensions = 0;
    options.Extensions.pExtensions = nullptr;
    options.dwAuthenticatorAttachment = 0;  // WEBAUTHN_AUTHENTICATOR_ATTACHMENT_ANY
    options.dwUserVerificationRequirement = 0;  // WEBAUTHN_USER_VERIFICATION_REQUIREMENT_ANY
    options.dwFlags = 0;

    UE_LOG(LogTemp, Log, TEXT("Opening Passkey authentication window..."));

    // 获取游戏窗口句柄
    //HWND hWnd = GetForegroundWindow();
	HWND hWnd = CreateTempWebAuthNWindow();
    // if (GEngine && GEngine->GameViewport)
    // {
    //     TSharedPtr<SWindow> GameWindow = GEngine->GameViewport->GetWindow();
    //     if (GameWindow.IsValid())
    //     {
    //         TSharedPtr<FGenericWindow> NativeWindow = GameWindow->GetNativeWindow();
    //         if (NativeWindow.IsValid())
    //         {
    //             hWnd = static_cast<HWND>(NativeWindow->GetOSWindowHandle());
    //         }
    //     }
    // }

    // if (!hWnd)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("Failed to get game window handle"));
    //     FreeLibrary(hWebAuthn);
    //     return false;
    // }

    // Call WebAuthNAuthenticatorGetAssertion
    PVOID pAssertion = nullptr;
    HRESULT hr = pfnGetAssertion(
        hWnd,
        rpId,
        &clientData,
        &options,
        &pAssertion
    );

    bool success = false;
    if (SUCCEEDED(hr) && pAssertion)
    {
        success = true;
        if (pfnFreeAssertion)
        {
            pfnFreeAssertion(pAssertion);
        }
        UE_LOG(LogTemp, Log, TEXT("Authentication successful!"));
    }
    else
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            UE_LOG(LogTemp, Warning, TEXT("Authentication cancelled by user"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Passkey error: 0x%08X"), hr);
        }
    }

    FreeLibrary(hWebAuthn);
    return success;
}

// 辅助函数：处理消息循环并等待指定时间（毫秒）
static void ProcessMessagesAndWait(int32 WaitTimeMs)
{
	UE_LOG(LogTemp, Log, TEXT("[PassKey] ProcessMessagesAndWait: Waiting %d ms"), WaitTimeMs);
	
	// 分段等待，每段处理消息循环
	const int32 StepMs = 50;
	int32 ElapsedMs = 0;
	
	while (ElapsedMs < WaitTimeMs)
	{
		// 处理 Windows 消息循环，确保窗口状态更新
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		// 让 Slate 处理事件
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().PumpMessages();
			FSlateApplication::Get().Tick();
		}
		
		// 短暂休眠
		FPlatformProcess::Sleep(StepMs / 1000.0f);
		ElapsedMs += StepMs;
	}
	
	UE_LOG(LogTemp, Log, TEXT("[PassKey] ProcessMessagesAndWait: Done waiting"));
}

// 辅助函数：等待窗口模式切换完成
static bool WaitForWindowModeChange(int32 TargetMode, float TimeoutSeconds = 2.0f)
{
	UE_LOG(LogTemp, Log, TEXT("[PassKey] WaitForWindowModeChange: Waiting for mode %d"), TargetMode);
	
	const float StepSeconds = 0.05f;  // 每 50ms 检查一次
	float ElapsedSeconds = 0.0f;
	
	while (ElapsedSeconds < TimeoutSeconds)
	{
		// 检查当前窗口模式
		int32 CurrentMode = UUIHelper::GetWindowMode();
		
		if (CurrentMode == TargetMode)
		{
			UE_LOG(LogTemp, Log, TEXT("[PassKey] WaitForWindowModeChange: Mode changed to %d after %.2f seconds"), 
				TargetMode, ElapsedSeconds);
			
			// 额外等待一小段时间确保渲染稳定
			FPlatformProcess::Sleep(0.1f);
			return true;
		}
		
		// 让出时间片，让系统处理窗口切换
		FPlatformProcess::Sleep(StepSeconds);
		ElapsedSeconds += StepSeconds;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[PassKey] WaitForWindowModeChange: Timeout waiting for mode %d, current mode = %d"), 
		TargetMode, UUIHelper::GetWindowMode());
	return false;
}

bool UUIHelper::PerformPasskeyAuthenticationWithFullScreen()
{
	UE_LOG(LogTemp, Log, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Starting authentication"));
	
	// 检查一下当前窗口是否是独占全屏，如果是切换到无边框全屏
	int32 currentWindowMode = GetWindowMode();
	UE_LOG(LogTemp, Log, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Current window mode = %d (%s)"), 
		currentWindowMode, *GetWindowModeString(currentWindowMode));
	
	bool bWasFullscreen = (currentWindowMode == 0);
	
	if (bWasFullscreen)
	{
		UE_LOG(LogTemp, Log, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Switching from Fullscreen to Borderless"));
		SetWindowMode(1);
		
		// 等待窗口模式切换完成（轮询检查）
		if (!WaitForWindowModeChange(1, 2.0f))
		{
			UE_LOG(LogTemp, Error, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Failed to switch to Borderless mode"));
		}
		
		PrintWindowMode();
	}

	// 执行 Passkey 认证
	bool ret = PerformPasskeyAuthentication();
	UE_LOG(LogTemp, Log, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Authentication result = %s"), 
		ret ? TEXT("Success") : TEXT("Failed"));
	
	// 如果之前是独占全屏，恢复回去
	if (bWasFullscreen)
	{
		// 短暂延迟，等待 Passkey 对话框完全关闭
		FPlatformProcess::Sleep(0.1f);
		
		UE_LOG(LogTemp, Log, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Restoring to Fullscreen mode"));
		SetWindowMode(0);
		
		// 等待窗口模式切换完成（轮询检查）
		if (!WaitForWindowModeChange(0, 2.0f))
		{
			UE_LOG(LogTemp, Error, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Failed to restore Fullscreen mode"));
		}
		
		PrintWindowMode();
	}
	
	UE_LOG(LogTemp, Log, TEXT("[PassKey] PerformPasskeyAuthenticationWithFullScreen: Completed"));
	return ret;
}