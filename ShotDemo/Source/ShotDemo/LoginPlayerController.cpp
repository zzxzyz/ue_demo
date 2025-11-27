// Fill out your copyright notice in the Description page of Project Settings.

#include "LoginPlayerController.h"
#include "LoginWidget.h"
#include "Blueprint/UserWidget.h"

ALoginPlayerController::ALoginPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ALoginPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 显示登录界面
	ShowLoginWidget();
}

void ALoginPlayerController::ShowLoginWidget()
{
	if (!LoginWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginWidgetClass is not set! Please set it in Blueprint or C++."));
		return;
	}

	if (!LoginWidgetInstance)
	{
		LoginWidgetInstance = CreateWidget<ULoginWidget>(this, LoginWidgetClass);
		if (LoginWidgetInstance)
		{
			LoginWidgetInstance->AddToViewport();
		}
	}
	else
	{
		LoginWidgetInstance->AddToViewport();
	}
}

void ALoginPlayerController::HideLoginWidget()
{
	if (LoginWidgetInstance)
	{
		LoginWidgetInstance->RemoveFromViewport();
	}
}

