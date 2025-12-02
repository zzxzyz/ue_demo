// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "LuaOverriderInterface.h"
#include "LoginWidget.generated.h"

/**
 * 登录界面Widget
 */
UCLASS()
class LUADEMO_API ULoginWidget : public UUserWidget, public ILuaOverriderInterface
{
	GENERATED_BODY()

public:
	ULoginWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// Lua文件路径
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "slua")
	FString LuaFilePath;

	// 实现 ILuaOverriderInterface 接口
	virtual FString GetLuaFilePath_Implementation() const override;

	// 邮箱输入框
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* EmailTextBox;

	// 密码输入框
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* PasswordTextBox;

	// 登录按钮
	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	// 登录按钮
	UPROPERTY(meta = (BindWidget))
    class UButton* CancelButton;

	// 错误信息文本
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ErrorText;

protected:
	// 登录按钮点击事件
	UFUNCTION()
	void OnLoginButtonClicked();

	// 登录成功后的处理
	void HandleLoginSuccess();

	// 验证邮箱格式
	bool IsValidEmail(const FString& Email) const;

	// 验证密码
	bool IsValidPassword(const FString& Password) const;

	// 显示错误信息
	void ShowError(const FString& ErrorMessage);

	// 清除错误信息
	void ClearError();
};

