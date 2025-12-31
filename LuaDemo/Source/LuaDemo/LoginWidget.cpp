// Fill out your copyright notice in the Description page of Project Settings.

#include "LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ULoginWidget::ULoginWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LuaFilePath = "LoginWidget";
}

FString ULoginWidget::GetLuaFilePath_Implementation() const
{
	return LuaFilePath;
}

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 设置输入模式为 UI 模式，显示鼠标光标
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		// 显示鼠标光标
		PlayerController->bShowMouseCursor = true;
		// 设置输入模式为 Game And UI，允许同时响应游戏和 UI 输入
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
		
		UE_LOG(LogTemp, Log, TEXT("LoginWidget: Set input mode to GameAndUI, mouse cursor visible"));
	}

	// 绑定登录按钮点击事件
	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginButtonClicked);
	}

	// CancelButton 的绑定在 Lua 脚本中处理，不再在 C++ 中绑定

	// 设置密码输入框为密码模式
	if (PasswordTextBox)
	{
		PasswordTextBox->SetIsPassword(true);
	}

	// 调用lua的NativeConstruct函数
	if (IsLuaFunctionExist(TEXT("NativeConstruct")))
	{
		CallLuaFunctionIfExist(TEXT("NativeConstruct"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeConstruct function not found in Lua."));
	}

	// 初始化时清除错误信息
	ClearError();
}

void ULoginWidget::NativeDestruct()
{
	// Widget 被移除时恢复输入模式为游戏模式
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		
		UE_LOG(LogTemp, Log, TEXT("LoginWidget: NativeDestruct - Restored input mode to GameOnly"));
	}
	
	Super::NativeDestruct();
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
	HandleLoginSuccess();
}

void ULoginWidget::HandleLoginSuccess()
{
	// 成功后可以隐藏或销毁登录界面
	ClearError();
	
	UE_LOG(LogTemp, Warning, TEXT("Login success, closing login widget."));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Login success, closing login widget."));
	
	// 可在此触发后续事件，如通知控制器切换关卡、加载主菜单等
	// 注意：输入模式恢复在 NativeDestruct 中统一处理
	RemoveFromParent();
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
