


#include "FSGameWorldModule.h"
//#include "MyObject.h"
//#include "MyClass.h"

#include "UObject/NoExportTypes.h"
#include "Subsystem/SubsystemActorManager.h"


//#include "FSkyline.h"
#include "FSGameWorldModule.h"
#include "FactorySkyline/FSCommandInstance.h"
#include "command/ChatCommandLibrary.h"
#include "FGGameInstance.h"
#include "FGNetworkLibrary.h"
#include "FGBuildableSubsystem.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableConveyorBelt.h"
#include "Buildables/FGBuildableResourceSink.h"
#include "Patching/NativeHookManager.h"

#include <iostream>
#include <string>
#include <windows.h>

UFSGameWorldModule::UFSGameWorldModule() {
	bRootModule = true;

	ModSubsystems.Add(AFSkyline::StaticClass());

	//OutputDebugString(_T("UFSGameWorldModule LOADED\n"));
}
void UFSGameWorldModule::DispatchLifecycleEvent(ELifecyclePhase Phase) {
	//UMyObject* UMyObject = NewObject<UMyObject>(this);

#if !WITH_EDITOR
	if (Phase == ELifecyclePhase::POST_INITIALIZATION) {

		//OutputDebugString(_T("UFSGameWorldModule POST_INITIALIZATION \n"));

		if (this->GetWorld() != nullptr) {
			//OutputDebugString(_T("UFSGameWorldModule WORLD INITIALIZED \n"));

			//AFSkyline* Skyline = AFSkyline::Get(this);


			UWorld* World = this->GetWorld();
			USubsystemActorManager* SubsystemManager = World->GetSubsystem<USubsystemActorManager>();

			AFGLightweightBuildableSubsystem* lightweightSubsystem;

			lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(World);

			if (lightweightSubsystem) {
				// don't destroy makes game unstable, maybe putting in substitute system would work?
				//lightweightSubsystem->Destroy();
				
			}

			SubsystemManager->RegisterSubsystemActor(AFSkyline::StaticClass());

			//AFSkyline* Skyline = SubsystemManager->GetSubsystemActor<AFSkyline>();

			//crashes and unstable
			//SUBSCRIBE_METHOD_AFTER(AFGBuildable::BeginPlay, [](AFGBuildable* ptr)
			//doesn't work
			//SUBSCRIBE_METHOD_AFTER(AFGBuildableSubsystem::BeginSpawnBuildable, [](AFGBuildable* ptr, AFGBuildableSubsystem* subsystem, TSubclassOf<  AFGBuildable > inClass, const FTransform& inTransform)
			/*
			SUBSCRIBE_METHOD_AFTER(AFGBuildable::OnConstruction, [](AFGBuildable* ptr, const FTransform& Transform)
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
			AsyncTask(ENamedThreads::GameThread, []() {
				SUBSCRIBE_METHOD_AFTER(AActor::OnConstruction, [](AActor* Actor, const FTransform& Transform) {
					if (Actor) {
						UE_LOG(LogTemp, Warning, TEXT("OnConstruction (main thread) called for %s"), *Actor->GetName());
						// Hook logic here
					}
					});
				});
			*/


			/*
			SUBSCRIBE_METHOD_AFTER(AActor::PostLoad, [](AActor* Object) {
				if (AActor* Actor = Cast<AActor>(Object)) {
					UE_LOG(LogTemp, Warning, TEXT("PostLoad called for %s"), *Actor->GetName());
					// Hook logic for actors after load
				}
				});
			*/

			AFSkyline* Skyline = AFSkyline::Get(this);

			if (Skyline) {

				Skyline->InitFullSetup();

				//SubsystemManager->GetSubsystemActor<AChatCommandSubsystem>();

				AChatCommandSubsystem* Chat = SubsystemManager->GetSubsystemActor<AChatCommandSubsystem>();
				if (Chat) {
					Chat->RegisterCommand(TEXT("FactorySkyline"), AFSCommandInstance::StaticClass());
				}

				//OutputDebugString(_T("UFSGameWorldModule Skyline INITIALIZED \n"));
			}


			//UMyObject* UMyObject = NewObject<UMyObject>(this);

		}
	}

	//UMyObject* UMyObject = NewObject<UMyObject>();
	/*
	int num = 5;
	if (num == 5) {
		MyClass testClass;
	}
	*/
#endif
}