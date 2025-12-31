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
    HWND hWnd = nullptr;
    if (GEngine && GEngine->GameViewport)
    {
        TSharedPtr<SWindow> GameWindow = GEngine->GameViewport->GetWindow();
        if (GameWindow.IsValid())
        {
            TSharedPtr<FGenericWindow> NativeWindow = GameWindow->GetNativeWindow();
            if (NativeWindow.IsValid())
            {
                hWnd = static_cast<HWND>(NativeWindow->GetOSWindowHandle());
            }
        }
    }

    if (!hWnd)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get game window handle"));
        FreeLibrary(hWebAuthn);
        return false;
    }

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
