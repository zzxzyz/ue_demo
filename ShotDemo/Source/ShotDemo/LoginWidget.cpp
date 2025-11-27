// Fill out your copyright notice in the Description page of Project Settings.

#include "LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 绑定登录按钮点击事件
	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginButtonClicked);
	}

	// 设置密码输入框为密码模式
	if (PasswordTextBox)
	{
		PasswordTextBox->SetIsPassword(true);
	}

	// 初始化时清除错误信息
	ClearError();
}

void ULoginWidget::OnLoginButtonClicked()
{
	if (!EmailTextBox || !PasswordTextBox)
	{
		return;
	}

	FString Email = EmailTextBox->GetText().ToString();
	FString Password = PasswordTextBox->GetText().ToString();

	// 清除之前的错误信息
	ClearError();

	// 验证邮箱
	if (Email.IsEmpty())
	{
		ShowError(TEXT("请输入邮箱地址"));
		return;
	}

	if (!IsValidEmail(Email))
	{
		ShowError(TEXT("邮箱格式不正确"));
		return;
	}

	// 验证密码
	if (Password.IsEmpty())
	{
		ShowError(TEXT("请输入密码"));
		return;
	}

	if (!IsValidPassword(Password))
	{
		ShowError(TEXT("密码长度至少为6位"));
		return;
	}

	// 这里可以添加实际的登录逻辑
	// 例如：调用服务器API、验证用户凭据等
	UE_LOG(LogTemp, Warning, TEXT("Login attempt - Email: %s, Password: %s"), *Email, *Password);

	// 登录成功后的处理（示例）
	// 可以隐藏登录界面、切换到主菜单等
	ShowError(TEXT("登录成功！")); // 临时显示成功消息
}

bool ULoginWidget::IsValidEmail(const FString& Email) const
{
	// 简单的邮箱格式验证
	// 检查是否包含@符号和点号
	int32 AtIndex = Email.Find(TEXT("@"));
	if (AtIndex == INDEX_NONE || AtIndex == 0)
	{
		return false;
	}

	int32 DotIndex = Email.Find(TEXT("."), ESearchCase::CaseSensitive, ESearchDir::FromStart, AtIndex);
	if (DotIndex == INDEX_NONE || DotIndex == AtIndex + 1)
	{
		return false;
	}

	// 确保点号在@符号之后
	if (DotIndex <= AtIndex)
	{
		return false;
	}

	return true;
}

bool ULoginWidget::IsValidPassword(const FString& Password) const
{
	// 密码至少6位
	return Password.Len() >= 6;
}

void ULoginWidget::ShowError(const FString& ErrorMessage)
{
	if (ErrorText)
	{
		ErrorText->SetText(FText::FromString(ErrorMessage));
		ErrorText->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULoginWidget::ClearError()
{
	if (ErrorText)
	{
		ErrorText->SetText(FText::GetEmpty());
		ErrorText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

