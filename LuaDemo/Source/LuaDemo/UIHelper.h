// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIHelper.generated.h"

/**
 * UI 辅助函数库，提供输入模式设置等功能
 * 用于在 Lua 中调用
 */
UCLASS()
class LUADEMO_API UUIHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 设置输入模式为 Game And UI 模式，同时显示鼠标光标
	 * @param PlayerController 玩家控制器
	 * @param WidgetToFocus 要聚焦的 Widget（可选）
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static void SetInputModeGameAndUI(APlayerController* PlayerController, UWidget* WidgetToFocus = nullptr);

	/**
	 * 设置输入模式为 Game Only 模式，同时隐藏鼠标光标
	 * @param PlayerController 玩家控制器
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static void SetInputModeGameOnly(APlayerController* PlayerController);

	/**
	 * 设置输入模式为 UI Only 模式，同时显示鼠标光标
	 * @param PlayerController 玩家控制器
	 * @param WidgetToFocus 要聚焦的 Widget（可选）
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static void SetInputModeUIOnly(APlayerController* PlayerController, UWidget* WidgetToFocus = nullptr);

	/**
	 * 获取当前窗口模式
	 * @return 窗口模式: 0=全屏, 1=无边框窗口, 2=窗口模式
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static int32 GetWindowMode();

	/**
	 * 设置窗口模式
	 * @param Mode 窗口模式: 0=全屏, 1=无边框窗口, 2=窗口模式
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static void SetWindowMode(int32 Mode);

	/**
	 * 切换窗口模式（在当前模式和无边框窗口之间切换）
	 * @return 切换后的窗口模式: 0=全屏, 1=无边框窗口, 2=窗口模式
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static int32 ToggleWindowMode();

	/**
	 * 获取窗口模式的字符串描述
	 * @param Mode 窗口模式
	 * @return 窗口模式的中文描述
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static FString GetWindowModeString(int32 Mode);

	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static bool PerformPasskeyAuthentication();

	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	static bool PerformPasskeyAuthenticationWithFullScreen();
};

extern void PrintWindowMode();
