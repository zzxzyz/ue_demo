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
#include "UIHelper.h"

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
				UUIHelper::PrintWindowMode();
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
