// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventureCharacter.h"


#include "base/log.h"
#include "monitor_usage.h"

void TestMonitorUsage() {
	static MonitorUsage::Ptr usage = CreateMonitorUsage();
	static bool b_start = false;
	if (usage != nullptr) {
		if (!b_start) {
			b_start = true;
			usage->Start();
		}
		else {
			b_start = false;
			usage->Stop();
		}
	}
}

// Sets default values
AAdventureCharacter::AAdventureCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	TestMonitorUsage();
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("in advance character"));
	
}

// Called when the game ends or when destroyed
void AAdventureCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	TestMonitorUsage();

	// Log the EndPlay event with reason
	const FString ReasonString = UEnum::GetValueAsString(EndPlayReason);
	LOG_INFO("AdventureCharacter EndPlay - Reason: %s", TCHAR_TO_UTF8(*ReasonString));

	// Display on-screen debug message
	if (GEngine)
	{
		const FString Message = FString::Printf(TEXT("AdventureCharacter EndPlay - Reason: %s"), *ReasonString);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, Message);
	}
}


// Called every frame
void AAdventureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAdventureCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

