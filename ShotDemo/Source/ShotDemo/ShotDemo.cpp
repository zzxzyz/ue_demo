// Fill out your copyright notice in the Description page of Project Settings.

#include "ShotDemo.h"
#include "Modules/ModuleManager.h"

#include "base/log.h"

class FShotDemoModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		FDefaultGameModuleImpl::StartupModule();

	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FShotDemoModule, ShotDemo, "ShotDemo");
