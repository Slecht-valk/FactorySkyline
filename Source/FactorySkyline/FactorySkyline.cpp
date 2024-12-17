// Copyright Epic Games, Inc. All Rights Reserved.

#include "FactorySkyline.h"

#include "FSGameWorldModule.h"

//#include "FSkyline.h"
//#include "..\FSkyline.h"
//#include "FSGameWorldModule.h"
//#include "FSCommandInstance.h"
//#include "Command/ChatCommandLibrary.h"
//#include <..\..\..\..\..\Source\FactoryGame\Public\FGGameInstance.h>
#include "FGGameInstance.h"
//#include "FGNetworkLibrary.h"
//#include "FGBuildableSubsystem.h"
//#include "Buildables/FGBuildable.h"
//#include "Buildables/FGBuildableConveyorBelt.h"
//#include "Buildables/FGBuildableResourceSink.h"
//#include <..\..\..\..\..\Mods\SML\Source\SML\Public\Patching\NativeHookManager.h>
#include "Patching\NativeHookManager.h"


#define LOCTEXT_NAMESPACE "FFactorySkylineModule"

void FFactorySkylineModule::StartupModule()
{

	AFGHologram* HologramManager = GetMutableDefault<AFGHologram>();

	UFGGameInstance* GameManager = GetMutableDefault<UFGGameInstance>();

	//AActor* TEST = GetMutableDefault<UFGGameInstance>();

	//SUBSCRIBE_METHOD_VIRTUAL_AFTER(UFGGameInstance::LoadComplete, GameManager, [](UFGGameInstance* FGGameInstance, const float loadTime, const FString& mapName) {
		//AFSkyline* Skyline = AFSkyline::Get(FGGameInstance);

		/*
		SUBSCRIBE_METHOD_AFTER(AActor::PostLoad, [](AActor* Object) {
			if (AActor* Actor = Cast<AActor>(Object)) {
				UE_LOG(LogTemp, Warning, TEXT("PostLoad called for %s"), *Actor->GetName());
				// Hook logic for actors after load
			}
			});
		*/
		/*
		if (Skyline) {
		}
		*/
		//});

	/*
	Invoke([&]() {
		HookInvoker<decltype(&AFGBuildableSubsystem::BeginSpawnBuildable), &AFGBuildableSubsystem::BeginSpawnBuildable>::InstallHook(L"AFGBuildableSubsystem::BeginSpawnBuildable");

		return HookInvoker<decltype(&AFGBuildableSubsystem::BeginSpawnBuildable), &AFGBuildableSubsystem::BeginSpawnBuildable>::AddHandlerAfter(
			[](AFGBuildable* ret, AFGBuildableSubsystem* subsystem, TSubclassOf<AFGBuildable> inClass, const FTransform& inTransform) {
				// Ensure this lambda captures the `AFSkyline` instance as you intended
				AFSkyline* FSkyline = AFSkyline::Get(subsystem->GetWorld());
				// Additional logic as needed
			}
		);
		});
	*/

	
	//SUBSCRIBE_METHOD_AFTER(AFGBuildableSubsystem::BeginSpawnBuildable, [](AFGBuildable* ptr, AFGBuildableSubsystem* subsystem, TSubclassOf<  AFGBuildable > inClass, const FTransform& inTransform)
	/*
	SUBSCRIBE_METHOD_AFTER(AFGBuildable::BeginPlay, [](AFGBuildable* ptr)
		{
			//ptr->mManagedByLightweightBuildableSubsystem = 0;

			UWorld* World = nullptr;

			World = ptr->GetWorld();

			if (World) {
				AFSkyline* FSkyline = nullptr;
				FSkyline = AFSkyline::Get(World);

				if (FSkyline) {
					FSkyline->Connect(ptr);
				}

			}

		});
		*/
	
	


	/*
	SUBSCRIBE_METHOD_AFTER(AFGBuildableHologram::AreConnectionsAlignedForGuidelines, [](const bool& result, const AFGBuildableHologram* self, UFGConnectionComponent* connection, UFGConnectionComponent* otherConnection, const FVector& connectionOffset, float perpDistanceThreshold, float allowedAngleDeviation)
		{
			AFSkyline* FSkyline = AFSkyline::Get(self->GetWorld());

		});
	*/


	/*
	SUBSCRIBE_METHOD_VIRTUAL_AFTER(AFGHologram::PostHologramPlacement, HologramManager, [](AFGHologram* FGHologram, const FHitResult& hitResult) {
		/AFSkyline* Skyline = AFSkyline::Get(FGHologram);
		//if (Skyline) {
		//}
	}
	*/

	/*
//#if !WITH_EDITOR
	SUBSCRIBE_METHOD_VIRTUAL_AFTER(AFGHologram::PostHologramPlacement, HologramManager, [](AFGHologram* FGHologram, const FHitResult& hitResult) {
		AFSkyline * Skyline = AFSkyline::Get(FGHologram);
		if (Skyline) {
		}
		else {
			Log("Skyline instance missing");
		}
		});
//#endif
*/

	/*
#if !WITH_EDITOR
	SUBSCRIBE_METHOD_VIRTUAL_AFTER(AFGHologram::BeginPlay, HologramManager, [=](AFGHologram* self) {
		AFSkyline* Skyline = AFSkyline::Get(FGHologram);
		if (Skyline) {
		}
		else {
			Log("Skyline instance missing");
		}
		});

#endif
*/

	/*
#if !WITH_EDITOR
	SUBSCRIBE_METHOD_VIRTUAL(AFGHologram::PostHologramPlacement, AFGHologram, [](auto& scope, AFGHologram* self, const FHitResult& hitResult) {
		// do some nice stuff there
	});
#endif
*/

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	//UFGGameInstance* GameManager = GetMutableDefault<UFGGameInstance>();
	//FSSubsystem->SpawnSubsystem(FSSubsystem, AFSkyline::StaticClass(), TEXT("FSSubsystem"));


	//SUBSCRIBE_METHOD_VIRTUAL_AFTER(UFGGameInstance::LoadComplete, [](UFGGameInstance*) {
		//}
	/*
#if WITH_EDITOR
	SUBSCRIBE_METHOD_VIRTUAL_AFTER(UFGGameInstance::LoadComplete, GameManager, [](UFGGameInstance* FGGameInstance, const float loadTime, const FString& mapName) {
		AFSkyline* Skyline = AFSkyline::Get(FGGameInstance);
		if (Skyline) {
		}

	});
#endif
*/
	//SUBSCRIBE_METHOD_VIRTUAL_AFTER(UFGGameInstance::LoadComplete, GameManager, [](UFGGameInstance* FGGameInstance, const float loadTime, const FString& mapName) {
		
		//AFSkyline* Skyline = AFSkyline::Get(FGGameInstance);
		//Log("Starting Mod: Factory Skyline...");
		//if (Skyline) {
			//Log("Skyline instance ok");
			//Skyline->InitFullSetup();
			//AChatCommandSubsystem* Chat = AChatCommandSubsystem::Get(FGGameInstance);
			//if (Chat) {
				//Chat->RegisterCommand(TEXT("FactorySkyline"), AFSCommandInstance::StaticClass());
			//}
		//}
		//else {
			//Log("Skyline instance missing");
		//}
		
		//});

}

void FFactorySkylineModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFactorySkylineModule, FactorySkyline)