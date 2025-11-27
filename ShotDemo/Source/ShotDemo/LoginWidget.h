// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "LoginWidget.generated.h"

/**
 * 登录界面Widget
 */
UCLASS()
class SHOTDEMO_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 邮箱输入框
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* EmailTextBox;

	// 密码输入框
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* PasswordTextBox;

	// 登录按钮
	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	// 错误信息文本
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ErrorText;

protected:
	// 登录按钮点击事件
	UFUNCTION()
	void OnLoginButtonClicked();

	// 验证邮箱格式
	bool IsValidEmail(const FString& Email) const;

	// 验证密码
	bool IsValidPassword(const FString& Password) const;

	// 显示错误信息
	void ShowError(const FString& ErrorMessage);

	// 清除错误信息
	void ClearError();
};

