


#include "FSGameWorldModule.h"
#include "MyObject.h"
#include "MyClass.h"

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
				/* don't destroy makes game unstable, maybe putting in substitute system would work?
				lightweightSubsystem->Destroy();
				*/
			}

			SubsystemManager->RegisterSubsystemActor(AFSkyline::StaticClass());

			//AFSkyline* Skyline = SubsystemManager->GetSubsystemActor<AFSkyline>();

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