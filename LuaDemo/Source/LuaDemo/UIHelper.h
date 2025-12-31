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
};
