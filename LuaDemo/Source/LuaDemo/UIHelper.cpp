// Fill out your copyright notice in the Description page of Project Settings.

#include "UIHelper.h"
#include "GameFramework/PlayerController.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"

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
