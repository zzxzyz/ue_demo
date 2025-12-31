// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Misc/FileHelper.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameUserSettings.h"
#include "Containers/Ticker.h"


// read file content
static uint8* ReadFile(IPlatformFile& PlatformFile, FString path, uint32& len) {
	IFileHandle* FileHandle = PlatformFile.OpenRead(*path);
	if (FileHandle) {
		len = (uint32)FileHandle->Size();
		uint8* buf = new uint8[len];

		FileHandle->Read(buf, len);

		// Close the file again
		delete FileHandle;

		return buf;
	}

	return nullptr;
}

UMyGameInstance::UMyGameInstance() : state(nullptr)
{
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		CreateLuaState();
	}
}

void UMyGameInstance::CreateLuaState()
{
    NS_SLUA::LuaState::onInitEvent.AddUObject(this, &UMyGameInstance::LuaStateInitCallback);

	CloseLuaState();
	state = new NS_SLUA::LuaState("SLuaMainState", this);
	state->setLoadFileDelegate([](const char* fn, FString& filepath)->TArray<uint8> {

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		FString path = FPaths::ProjectContentDir();
		FString filename = UTF8_TO_TCHAR(fn);
		path /= "Lua";
		path /= filename.Replace(TEXT("."), TEXT("/"));

		TArray<uint8> Content;
		TArray<FString> luaExts = { UTF8_TO_TCHAR(".lua"), UTF8_TO_TCHAR(".luac") };
		for (auto& it : luaExts) {
			auto fullPath = path + *it;

			// 添加调试日志
			UE_LOG(LogTemp, Log, TEXT("SLua: Trying to load file: %s"), *fullPath);

			FFileHelper::LoadFileToArray(Content, *fullPath);
			if (Content.Num() > 0) {
				filepath = fullPath;
				UE_LOG(LogTemp, Log, TEXT("SLua: Successfully loaded file: %s, size: %d bytes"), *fullPath, Content.Num());
				return MoveTemp(Content);
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("SLua: Failed to load file: %s (tried paths: %s.lua, %s.luac)"), 
			UTF8_TO_TCHAR(fn), *(path + TEXT(".lua")), *(path + TEXT(".luac")));
		return MoveTemp(Content);
	});
	state->init();

	slua::LuaVar v = state->doFile("helloworld");
	// InGameManager.lua 会被 ALuaActor 自动加载，不需要在这里手动调用
	// 只需要确保 Lua 文件可以被找到即可
}

void UMyGameInstance::CloseLuaState()
{
    if (state)
    {
    	state->close();
		delete state;
        state = nullptr;
    }
	
}

// 获取窗口模式的字符串描述
static FString GetWindowModeString(EWindowMode::Type WindowMode)
{
	switch (WindowMode)
	{
	case EWindowMode::Fullscreen:
		return TEXT("全屏模式 (Fullscreen)");
	case EWindowMode::WindowedFullscreen:
		return TEXT("无边框窗口化全屏 (WindowedFullscreen/Borderless)");
	case EWindowMode::Windowed:
		return TEXT("窗口模式 (Windowed)");
	default:
		return TEXT("未知模式 (Unknown)");
	}
}

// 打印当前窗口显示模式（同时输出到日志和屏幕）
static void PrintWindowMode()
{
	if (GEngine && GEngine->GameViewport)
	{
		// 屏幕显示的持续时间（秒）
		const float DisplayTime = 10.0f;
		// 屏幕显示的颜色
		const FColor TitleColor = FColor::Yellow;
		const FColor InfoColor = FColor::Green;
		
		int32 MsgKey = 100; // 消息的唯一标识，用于更新同一位置的消息
		
		// 方式1：从 GameUserSettings 获取
		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		if (UserSettings)
		{
			EWindowMode::Type WindowMode = UserSettings->GetFullscreenMode();
			FIntPoint Resolution = UserSettings->GetScreenResolution();
			
			// 输出到日志
			UE_LOG(LogTemp, Warning, TEXT("========== 窗口显示模式信息 =========="));
			UE_LOG(LogTemp, Warning, TEXT("窗口模式: %s"), *GetWindowModeString(WindowMode));
			UE_LOG(LogTemp, Warning, TEXT("分辨率: %d x %d"), Resolution.X, Resolution.Y);
			UE_LOG(LogTemp, Warning, TEXT("是否使用 VSync: %s"), UserSettings->IsVSyncEnabled() ? TEXT("是") : TEXT("否"));
			UE_LOG(LogTemp, Warning, TEXT("====================================="));
			
			// 输出到屏幕
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, TitleColor, TEXT("========== 窗口显示模式信息 =========="));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口模式: %s"), *GetWindowModeString(WindowMode)));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("分辨率: %d x %d"), Resolution.X, Resolution.Y));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("VSync: %s"), UserSettings->IsVSyncEnabled() ? TEXT("是") : TEXT("否")));
		}
		
		// 方式2：从实际窗口获取
		TSharedPtr<SWindow> GameWindow = GEngine->GameViewport->GetWindow();
		if (GameWindow.IsValid())
		{
			EWindowMode::Type ActualMode = GameWindow->GetWindowMode();
			FVector2D WindowSize = GameWindow->GetSizeInScreen();
			FVector2D WindowPos = GameWindow->GetPositionInScreen();
			
			// 输出到日志
			UE_LOG(LogTemp, Warning, TEXT("========== 实际窗口状态 =========="));
			UE_LOG(LogTemp, Warning, TEXT("实际窗口模式: %s"), *GetWindowModeString(ActualMode));
			UE_LOG(LogTemp, Warning, TEXT("窗口大小: %.0f x %.0f"), WindowSize.X, WindowSize.Y);
			UE_LOG(LogTemp, Warning, TEXT("窗口位置: (%.0f, %.0f)"), WindowPos.X, WindowPos.Y);
			UE_LOG(LogTemp, Warning, TEXT("窗口标题: %s"), *GameWindow->GetTitle().ToString());
			UE_LOG(LogTemp, Warning, TEXT("=================================="));
			
			// 输出到屏幕
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, TitleColor, TEXT("========== 实际窗口状态 =========="));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("实际窗口模式: %s"), *GetWindowModeString(ActualMode)));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口大小: %.0f x %.0f"), WindowSize.X, WindowSize.Y));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口位置: (%.0f, %.0f)"), WindowPos.X, WindowPos.Y));
			GEngine->AddOnScreenDebugMessage(MsgKey++, DisplayTime, InfoColor, FString::Printf(TEXT("窗口标题: %s"), *GameWindow->GetTitle().ToString()));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("无法获取窗口信息：GEngine 或 GameViewport 未初始化"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("无法获取窗口信息：GameViewport 未初始化"));
		}
	}
}

void UMyGameInstance::Init()
{
	Super::Init();
	
	// 使用 FTicker 延迟执行，这在编辑器模式下也能正常工作
	// GetWorld() 在 Init() 阶段可能返回 nullptr，所以不能使用 World 的 TimerManager
	FTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([](float DeltaTime) -> bool
		{
			// 检查 GameViewport 是否已初始化
			if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
			{
				PrintWindowMode();
				return false; // 返回 false 停止 Ticker
			}
			return true; // 返回 true 继续等待
		}),
		0.1f // 每 0.1 秒检查一次
	);
}

void UMyGameInstance::Shutdown()
{
	CloseLuaState();

	Super::Shutdown();
}

static int32 PrintLog(NS_SLUA::lua_State *L)
{
	FString str;
	size_t len;
	const char* s = luaL_tolstring(L, 1, &len);
	if (s) str += UTF8_TO_TCHAR(s);
	NS_SLUA::Log::Log("PrintLog %s", TCHAR_TO_UTF8(*str));
	return 0;
}

void UMyGameInstance::LuaStateInitCallback(NS_SLUA::lua_State* L)
{
	lua_pushcfunction(L, PrintLog);
	lua_setglobal(L, "PrintLog");
}
