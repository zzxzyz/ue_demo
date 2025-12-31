// Fill out your copyright notice in the Description page of Project Settings.

#include "UIHelper.h"
#include "GameFramework/PlayerController.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"

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
