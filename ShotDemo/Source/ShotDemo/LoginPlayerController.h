// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoginPlayerController.generated.h"

class ULoginWidget;

/**
 * 登录界面PlayerController
 */
UCLASS()
class SHOTDEMO_API ALoginPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALoginPlayerController();

	virtual void BeginPlay() override;

protected:
	// 登录Widget类引用
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoginWidgetClass;

	// 登录Widget实例
	UPROPERTY()
	ULoginWidget* LoginWidgetInstance;

	// 显示登录界面
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLoginWidget();

	// 隐藏登录界面
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideLoginWidget();
};

