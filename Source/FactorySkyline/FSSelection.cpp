// ILikeBanas


#include "FSSelection.h"
#include "FSDesign.h"
#include "FSkyline.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableFoundation.h"
#include "FSBuildableService.h"
#include "FGFactorySettings.h"
#include "FGProductionIndicatorInstanceComponent.h"

//TODO DOES THIS NEED A REPLACEMENT?
//#include "Components/ProxyInstancedStaticMeshComponent.h"

#include "Buildables/FGBuildablePowerPole.h"
#include "Buildables/FGBuildableWire.h"
#include "FGPowerConnectionComponent.h"

#include "Buildables/FGBuildableWidgetSign.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "Buildables/FGBuildablePowerStorage.h"
#include "Buildables/FGBuildableCircuitSwitch.h"

#include "FGRecipe.h"
#include "FGRecipeManager.h"
#include "AbstractInstanceManager.h"

//#include "AbstractInstance/public/InstanceData.h"
//#include "../../../Plugins/AbstractInstance/Source/AbstractInstance/Public/InstanceData.h"

#include "Buildables/FGBuildableBlueprintDesigner.h"
#include "Equipment/FGBuildGunDismantle.h"

#include <iostream>
#include <string>

#include "Kismet/BlueprintFunctionLibrary.h"

#include "FSHologramService.h"

static const uint8 BUILDABLE_COLORS_MAX_SLOTS = 18;

void UFSSelection::Init()
{
	AFSkyline* FSkyline = AFSkyline::Get(this);
	if (!FSkyline) return;
	
	this->ConnectSelectService = FSkyline->ConnectSelectService;
	this->RectSelectService = FSkyline->RectSelectService;
	
	this->GCFlag.Empty();
	this->ISMMapping.Empty();
	this->MaterialMapping.Empty();

	//Hologram = Cast<UMaterialInstanceConstant>(StaticLoadObject(UMaterialInstanceConstant::StaticClass(), this, TEXT("/Game/FactoryGame/Resource/Parts/AluminumPlate/Material/MI_AluminiumSheet_01.MI_AluminiumSheet_01")));
	
	Scanline = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), this, TEXT("/FactorySkyline/Icon/White.White")));

	//SelectMaterial = UMaterialInstanceDynamic::Create(Hologram, this);
	//SelectMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultValidPlacementMaterial, this);
	//SelectMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.0f, 0.0f, 1.0f));
	//SelectMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);

	//SelectMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	//SelectMaterial->SetScalarParameterValue(FName("MinOpacity"), 1.00f);
	//SelectMaterial->SetScalarParameterValue(FName("MaxOpacity"), 1.00f);
	//SelectMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.043735f, 0.201556f, 0.496933f));
	//SelectMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);

	
	SelectMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	SelectMaterial->SetScalarParameterValue(FName("MinOpacity"), 0.05f);
	SelectMaterial->SetScalarParameterValue(FName("MaxOpacity"), 0.25f);
	SelectMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.043735f, 0.201556f, 0.496933f));
	SelectMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);

	InvisibleMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	InvisibleMaterial->SetScalarParameterValue(FName("MinOpacity"), 0.00f);
	InvisibleMaterial->SetScalarParameterValue(FName("MaxOpacity"), 0.00f);
	InvisibleMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.043735f, 0.201556f, 0.496933f));
	InvisibleMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);

	CubeMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	CubeMaterial->SetScalarParameterValue(FName("MinOpacity"), 0.20f);
	CubeMaterial->SetScalarParameterValue(FName("MaxOpacity"), 0.50f);
	CubeMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.043735f, 0.201556f, 0.496933f));
	CubeMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);
	
	//SelectMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultValidPlacementMaterialSimplified, this);

	FocusMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	FocusMaterial->SetScalarParameterValue(FName("MinOpacity"), 0.00f);
	//FocusMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.043735f, 0.201556f, 0.496933f));
	FocusMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);

	/*
	UObjectLibrary* objectLibrary = NewObject<UObjectLibrary>();
	objectLibrary->ObjectBaseClass = UObject::StaticClass();
	objectLibrary->bHasBlueprintClasses = false;
	objectLibrary->UseWeakReferences(false);
	objectLibrary->LoadAssetDataFromPath(TEXT("/Game/FactoryGame"));

	TArray<FAssetData> arrayAssetData;
	objectLibrary->GetAssetDataList(arrayAssetData);

	for (int32 index = 0; index < arrayAssetData.Num(); ++index)
	{
		SML::Logging::info(*arrayAssetData[index].AssetClass.ToString(), TEXT(" "), *arrayAssetData[index].ObjectPath.ToString());
	}*/
}

void UFSSelection::Select(FSBuildable Buildable)
{
	AFSkyline* FSkylin = AFSkyline::Get(this);
	//TSubclassOf< class UFGBuildGunStateDismantle > state = FSkylin->FSCtrl->FGBuildGun->mDismantleStateClass;

	/*
	UFGBuildGunStateDismantle* dismantleState = FSkylin->FSCtrl->FGBuildGun->mDismantleStateClass.GetDefaultObject();
	//FSkylin->FSCtrl->FGBuildGun->GotoDismantleState();
	FSkylin->FSCtrl->GetPlayer()->SetOverrideEquipment(FSkylin->FSCtrl->FGBuildGun);
	AFGEquipment* Equipment = FSkylin->FSCtrl->GetPlayer()->GetEquipmentInSlot(EEquipmentSlot::ES_ARMS);
	FSkylin->FSCtrl->GetPlayer()->EquipEquipment(FSkylin->FSCtrl->FGBuildGun);
	FSkylin->FSCtrl->FGBuildGun->Equip(FSkylin->FSCtrl->GetPlayer());
	FSkylin->FSCtrl->FGBuildGun->GotoStateInternal(EBuildGunState::BGS_DISMANTLE);
	//dismantleState->BeginState_Implementation();
	*/

	//Design->AddElement(Buildable);
	//Design->BuildableMark.Add(Buildable, 1);

	//UFGBuildGunStateDismantle* NewDismantleState = NewObject<UFGBuildGunStateDismantle>();

	//FSkylin->FSCtrl->FGBuildGun->mDismantleStateClass = UFGBuildGunStateDismantle::StaticClass();

	/*
	FSkylin->FSCtrl->FGBuildGun->GotoDismantleState();

	UFGBuildGunStateDismantle* dismantleState = FSkylin->FSCtrl->FGBuildGun->mDismantleStateClass.GetDefaultObject();

	fgcheck(dismantleState);

	//dismantleState->OnStartDismantle();

	Buildable->SetMarkedForDismantle();
	Buildable->TogglePendingDismantleMaterial(true);

	dismantleState->SetMultiDismantleState(true);
	dismantleState->mCurrentlySelectedActor = Cast<AActor>(Buildable);
	dismantleState->AddPendingDismantleActor(Cast<AActor>(Buildable));
	dismantleState->UpdateHighlightedActors();

	FSkylin->FSCtrl->GetPlayer()->GetOutline()->ShowDismantlePendingMaterial(Cast<AActor>(Buildable));

	//FSkylin->FSCtrl->GetPlayer()->GetOutline()->ShowOutline(Buildable, EOutlineColor::OC_HOLOGRAM);
	TArray<AActor*> NewArr;
	for (AFGBuildable* Build : Design->BuildableArray) {
		NewArr.Add(Build);
	}

	FSkylin->FSCtrl->GetPlayer()->GetOutline()->ShowDismantlePendingMaterial(NewArr);
	*/

	//FSkylin->FSCtrl->GetPlayer()->GetOutline()->ShowMultiActorOutline(NewArr, EOutlineColor::OC_HOLOGRAM);

	// fatal error crash
	//dismantleState->SetMultiDismantleState(true);

	//dismantleState->AddPendingDismantleActor(Cast<AActor>(Buildable));

	// passes so static class is assigned
	//fgcheck(FSkylin->FSCtrl->FGBuildGun->mDismantleStateClass);

	//state->AddPendingDismantleActor(Cast<AActor>(Buildable));

	//FSkylin->FSCtrl->FGBuildGun->GotoState(EBuildGunState::BGS_DISMANTLE);

	//UFGBuildGunStateDismantle* state = Cast< UFGBuildGunStateDismantle>(FSkylin->FSCtrl->FGBuildGun->mDismantleStateClass);

	//crashes
	//state->mCurrentlySelectedActor = Cast<AActor>(Buildable);

	//FSkylin->FSCtrl->FGBuildGun->GotoState(EBuildGunState::BGS_DISMANTLE);

	//fgcheck(state);

	// crashes
	//state->OnStartDismantle();

	// crashes
	//state->SetMultiDismantleState(true);

	// crashes
	//state->AddPendingDismantleActor(Cast<AActor>(Buildable));

	//for (AFGBuildable* Build : Design->BuildableArray) {
		//state->AddPendingDismantleActor(Build);
	//}

	//return;

	/*
	AFSkyline* FSkylin = AFSkyline::Get(this);
	FSkylin->FSCtrl->GetPlayer()->GetOutline()->ShowOutline(Buildable, EOutlineColor::OC_HOLOGRAM);
	TArray<AActor*> NewArr;
	for (AFGBuildable* Build : Design->BuildableArray) {
		NewArr.Add(Build);
	}
	FSkylin->FSCtrl->GetPlayer()->GetOutline()->ShowMultiActorOutline(NewArr, EOutlineColor::OC_HOLOGRAM);

	return;
	*/
	
	if (!Buildable.Buildable) {
		if (Design->IsElementSelected(Buildable)) {
			Design->RemoveElement(Buildable);
			Design->BuildableMark.Add(Buildable, 0);
			DisableHightLight(Buildable);
		}
		else {
			Design->AddElement(Buildable);
			Design->BuildableMark.Add(Buildable, 1);
			EnableHightLight(Buildable, SelectMaterial);
		}
	} else {

		if (Design->IsElementSelected(Buildable)) {
			Design->RemoveElement(Buildable);
			Design->BuildableMark.Add(Buildable, 0);

			/*
			FSMaterialHandle** handleHolder = SelectedMap.Find(Buildable);
			if (handleHolder) {
				FSMaterialHandle handle = **handleHolder;
				if (handle.hologramVariation) {
					//Buildable->ToggleInstanceVisibility(true);
					//SelectedMap.Remove(Buildable);
					//ActorList.Remove(Buildable);
					//return;
				}
			}
			*/

			DisableHightLight(Buildable);
			if (Buildable.Buildable->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
				AFGBuildableWidgetSign* BuildableSign = Cast<AFGBuildableWidgetSign>(Buildable.Buildable);
				BuildableSign->PasteSettings_Implementation(BuildableSign->CopySettings_Implementation());
			}

			if (Buildable.Buildable->GetName().Contains("Build_PowerPole")) {
				AFGBuildablePowerPole* PowerPole = Cast<AFGBuildablePowerPole>(Buildable.Buildable);
				if (!PowerPole) return;
				UFGPowerConnectionComponent* ConnectionComponent = PowerPole->GetPowerConnection(0);
				if (!ConnectionComponent) return;

				TArray<AFGBuildableWire*> ConnectionWires;
				ConnectionComponent->GetWires(ConnectionWires);

				for (AFGBuildableWire* TargetWire : ConnectionWires) {
					AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
					FSBuildable Buildable;
					Buildable.Buildable = Wire;
					Design->RemoveElement(Buildable);
					Design->BuildableMark.Add(Buildable, 0);

					//FSBuildable Buildable;
					//Buildable.Buildable = Wire;
					DisableHightLight(Buildable);
				}
			}

			if (Buildable.Buildable->GetName().Contains("Build_LightsControlPanel") || Cast<AFGBuildableCircuitSwitch>(Buildable.Buildable)) {
				//AFGBuildableLightsControlPanel* lightControl = Cast<AFGBuildableLightsControlPanel>(Buildable);
				AFGBuildableCircuitBridge* bridge = Cast<AFGBuildableCircuitBridge>(Buildable.Buildable);
				TArray<UFGCircuitConnectionComponent*> connections = bridge->mConnections;

				for (int i = 0; i < connections.Num(); i++) {
					UFGCircuitConnectionComponent* connection = connections[i];
					TArray<AFGBuildableWire*> ConnectionWires;
					connection->GetWires(ConnectionWires);

					for (AFGBuildableWire* TargetWire : ConnectionWires) {
						AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
						FSBuildable Buildable;
						Buildable.Buildable = Wire;
						Design->RemoveElement(Buildable);
						Design->BuildableMark.Add(Buildable, 0);

						//FSBuildable Buildable;
						//Buildable.Buildable = Wire;
						DisableHightLight(Buildable);
					}
				}

			}

			if (Cast<AFGBuildableLightSource>(Buildable.Buildable) || Cast<AFGBuildablePowerStorage>(Buildable.Buildable)) {
				TArray<UActorComponent*> ReplicatedComponents = Buildable.Buildable->ReplicatedComponents;
				for (int i = 0; i < ReplicatedComponents.Num(); i++) {
					if (Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i])) {
						UFGPowerConnectionComponent* ConnectionComponent;
						ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i]);
						if (!ConnectionComponent) return;

						TArray<AFGBuildableWire*> ConnectionWires;
						ConnectionComponent->GetWires(ConnectionWires);

						for (AFGBuildableWire* TargetWire : ConnectionWires) {
							AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
							FSBuildable Buildable;
							Buildable.Buildable = Wire;
							Design->RemoveElement(Buildable);
							Design->BuildableMark.Add(Buildable, 0);

							//FSBuildable Buildable;
							//Buildable.Buildable = Wire;
							DisableHightLight(Buildable);
						}
					}
				}
				/*
				if (ReplicatedComponents.Num() >= 2) {
					UFGPowerConnectionComponent* ConnectionComponent;
					ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[1]);
					if (Cast<AFGBuildablePowerStorage>(Buildable)) {
						ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[0]);
					}
						if (!ConnectionComponent) return;

						TArray<AFGBuildableWire*> ConnectionWires;
						ConnectionComponent->GetWires(ConnectionWires);

						for (AFGBuildableWire* TargetWire : ConnectionWires) {
							AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
							Design->AddElement(Wire);
							Design->BuildableMark.Add(Wire, 1);
							EnableHightLight(Wire, SelectMaterial);
						}

				}
				*/
			}

			if (Buildable.Buildable->GetName().Contains("Build_ConveyorLiftMk")) {
				if (Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[0]) {
					FSBuildable Buildable;
					Buildable.Buildable = Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[0];
					Design->RemoveElement(Buildable);
					Design->BuildableMark.Add(Buildable, 0);

					DisableHightLight(Buildable);
				}
				if (Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[1]) {
					FSBuildable Buildable;
					Buildable.Buildable = Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[1];
					Design->RemoveElement(Buildable);
					Design->BuildableMark.Add(Buildable, 0);

					DisableHightLight(Buildable);
				}
			}

			//SML::Logging::info(TEXT("Remove "), *Buildable->GetName());
		}
		else {
			//Buildable->ToggleInstanceVisibility(false);
			bool foundCustomSelection = false;
			// see if we are selecting a passThrough
			if (Buildable.Buildable->GetName().Contains("Build_FoundationPassthrough_Lift")) {
				// for passThroughs we need to determine if they have connections to them because we use the lift operators to make them instead

				/* DEPRECATED CHECK, TODO NEED THIS ACTUALLY FOR ANYTHING?
				bool attachedLifts = Cast<AFGBuildablePassthrough>(Buildable)->HasAnyAttachedBuildings();
				if (attachedLifts) {
					//foundCustomSelection = true;
				}
				*/
			}

			if (Buildable.Buildable->GetName().Contains("Build_PowerPole")) {
				foundCustomSelection = true;

				Design->AddElement(Buildable);
				Design->BuildableMark.Add(Buildable, 1);


				EnableHightLight(Buildable, SelectMaterial);

				AFGBuildablePowerPole* PowerPole = Cast<AFGBuildablePowerPole>(Buildable.Buildable);
				if (!PowerPole) return;
				UFGPowerConnectionComponent* ConnectionComponent = PowerPole->GetPowerConnection(0);
				if (!ConnectionComponent) return;

				TArray<AFGBuildableWire*> ConnectionWires;
				ConnectionComponent->GetWires(ConnectionWires);

				for (AFGBuildableWire* TargetWire : ConnectionWires) {
					AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);

					FSBuildable Buildable;
					Buildable.Buildable = Wire;
					Design->AddElement(Buildable);
					Design->BuildableMark.Add(Buildable, 1);

					EnableHightLight(Buildable, SelectMaterial);
				}
			}

			if (Buildable.Buildable->GetName().Contains("Build_LightsControlPanel") || Cast<AFGBuildableCircuitSwitch>(Buildable.Buildable)) {
				AFGBuildableLightsControlPanel* lightControl = Cast<AFGBuildableLightsControlPanel>(Buildable.Buildable);
				AFGBuildableCircuitBridge* bridge = Cast<AFGBuildableCircuitBridge>(Buildable.Buildable);
				TArray<UFGCircuitConnectionComponent*> connections = bridge->mConnections;
				//lightControl->OnLightControlPanelStateChanged.Broadcast(true);
				//lightControl->mDidFirstTimeUse = true;

				for (int i = 0; i < connections.Num(); i++) {
					UFGCircuitConnectionComponent* connection = connections[i];
					TArray<AFGBuildableWire*> ConnectionWires;
					connection->GetWires(ConnectionWires);

					for (AFGBuildableWire* TargetWire : ConnectionWires) {
						AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);

						FSBuildable Buildable;
						Buildable.Buildable = Wire;
						Design->AddElement(Buildable);
						Design->BuildableMark.Add(Buildable, 1);

						EnableHightLight(Buildable);
					}
				}

			}

			if (Cast<AFGBuildableLightSource>(Buildable.Buildable) || Cast<AFGBuildablePowerStorage>(Buildable.Buildable)) {
				TArray<UActorComponent*> ReplicatedComponents = Buildable.Buildable->ReplicatedComponents;
				for (int i = 0; i < ReplicatedComponents.Num(); i++) {
					if (Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i])) {
						UFGPowerConnectionComponent* ConnectionComponent;
						ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i]);
						if (!ConnectionComponent) return;

						TArray<AFGBuildableWire*> ConnectionWires;
						ConnectionComponent->GetWires(ConnectionWires);

						for (AFGBuildableWire* TargetWire : ConnectionWires) {
							AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);

							FSBuildable Buildable;
							Buildable.Buildable = Wire;
							Design->AddElement(Buildable);
							Design->BuildableMark.Add(Buildable, 1);

							EnableHightLight(Buildable, SelectMaterial);
						}
					}
				}
				/*
				if (ReplicatedComponents.Num() >= 2) {
					UFGPowerConnectionComponent* ConnectionComponent;
					ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[1]);
					if (Cast<AFGBuildablePowerStorage>(Buildable)) {
						ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[0]);
					}
						if (!ConnectionComponent) return;

						TArray<AFGBuildableWire*> ConnectionWires;
						ConnectionComponent->GetWires(ConnectionWires);

						for (AFGBuildableWire* TargetWire : ConnectionWires) {
							AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
							Design->AddElement(Wire);
							Design->BuildableMark.Add(Wire, 1);
							EnableHightLight(Wire, SelectMaterial);
						}

				}
				*/
			}

			if (Buildable.Buildable->GetName().Contains("Build_ConveyorLiftMk")) {

				if (Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[0]) {
					FSBuildable Buildable;
					Buildable.Buildable = Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[0];
					Design->AddElement(Buildable);
					Design->BuildableMark.Add(Buildable, 1);

					EnableHightLight(Buildable, SelectMaterial);
				}
				if (Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[1]) {
					FSBuildable Buildable;
					Buildable.Buildable = Cast<AFGBuildableConveyorLift>(Buildable.Buildable)->mSnappedPassthroughs[1];
					Design->AddElement(Buildable);
					Design->BuildableMark.Add(Buildable, 1);

					EnableHightLight(Buildable, SelectMaterial);
				}
			}

			if (Buildable.Buildable->GetClass()->IsChildOf<AFGBuildablePipeBase>()) {
				AFGBuildablePipeline* pipeBuildable = Cast<AFGBuildablePipeline>(Buildable.Buildable);

				// because pipes are more of a special remove case as they can also have a seperate buildable attached to them that needs to be removed as well otherwise
				// it'll linger in the world
				if (pipeBuildable) {
					AFGBuildablePipelineFlowIndicator* Indicator = pipeBuildable->GetFlowIndicator();
					if (Indicator) {
						//IFGDismantleInterface::Execute_Dismantle(Indicator);

						/*
						FSBuildable Buildable;
						Buildable.Buildable = Cast<AFGBuildable>(Indicator);
						Design->AddElement(Buildable);
						Design->BuildableMark.Add(Buildable, 1);

						EnableHightLight(Buildable, SelectMaterial);
						*/

					}
				}

			}

			if (!foundCustomSelection) {
				Design->AddElement(Buildable);
				Design->BuildableMark.Add(Buildable, 1);
				EnableHightLight(Buildable, SelectMaterial);
			}

			//SML::Logging::info(TEXT("Add "), *Buildable->GetName(), TEXT(" "), *Buildable->GetTransform().ToString());
		}

	}



	//AFSkyline* FSkyline = AFSkyline::Get(this);
	//FSkyline->FSCtrl->GetPlayer()->GetOutline()->ShowOutline(Buildable, EOutlineColor::OC_HOLOGRAM);
	//TArray<AActor*> NewArr;
	//for (AFGBuildable* Build : Design->BuildableArray) {
		//NewArr.Add(Build);
	//}

	/*
	if (Buildable->GetClass()->GetName().Equals("Build_FoundationPassthrough_Lift_C")) {
		std::string  str = TCHAR_TO_UTF8(*Buildable->GetName());
		str.append("\n");
		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugStringW(wideString);
	}
	*/



	//FSkyline->FSCtrl->GetPlayer()->GetOutline()->ShowMultiActorOutline(NewArr, EOutlineColor::OC_HOLOGRAM);

	return;
	if (Cube != nullptr) {
		Cube->Destroy();
	}

	AFSkyline* FSkyline = AFSkyline::Get(this);

	FActorSpawnParameters* parameters = new FActorSpawnParameters();

	FTransform cubeTransform = FTransform();
	FTransform BuildableTransform = Buildable.Buildable->GetTransform();
	FVector BuildablePoint = BuildableTransform.GetTranslation();
	cubeTransform.SetTranslation(BuildablePoint);

	Cube = (AMyActor*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AMyActor::StaticClass(), cubeTransform, *parameters);
	FBoxSphereBounds baseBounds = Cube->GetRootComponent()->Bounds;
	//Cube->SetActorScale3D(FVector(8, 8, 8));
	//Cube->SetActorScale3D(FVector(16, 8, 8));
	//Cube->SetActorScale3D(FVector(24, 8, 8));
	//Cube->SetActorScale3D(FVector(32, 8, 8));
	//Cube->SetActorScale3D(FVector(40, 8, 8));
	//Cube->SetActorScale3D(FVector(48, 8, 8));

	//Cube->SetActorScale3D(FVector(56, 8, 8));
	//Cube->SetActorScale3D(FVector(48, 8, 8));
	//Cube->SetActorScale3D(FVector(20, 20, 20));

	//crashes the game I think?
	//Cube->SetActorScale3D(FVector(2000, 2000, 2000));
	Cube->SetActorScale3D(FVector(10000, 10000, 10000));

	Cube->SetActorEnableCollision(false);
	FBoxSphereBounds bounds1 = Cube->GetRootComponent()->Bounds;


	FTransform transform = Cube->GetTransform();

	FVector translate = transform.GetTranslation();
	//FVector newPoint = FVector(translate.X, translate.Y + (bounds1.BoxExtent.Y / 2) + baseBounds.BoxExtent.Y - 250, translate.Z);
	//FVector newPoint = FVector(translate.X, translate.Y + (bounds1.BoxExtent.Y / 2) + baseBounds.BoxExtent.Y - 50, translate.Z);
	//FVector newPoint = FVector(translate.X, translate.Y+(bounds1.BoxExtent.Y/2)+baseBounds.BoxExtent.Y+150, translate.Z);
	//FVector newPoint = FVector(translate.X, translate.Y + (bounds1.BoxExtent.Y / 2) + baseBounds.BoxExtent.Y + 350, translate.Z);
	//FVector newPoint = FVector(translate.X, translate.Y + (bounds1.BoxExtent.Y / 2) + baseBounds.BoxExtent.Y + 550, translate.Z);
	//FVector newPoint = FVector(translate.X, translate.Y + (bounds1.BoxExtent.Y / 2) + baseBounds.BoxExtent.Y + 750, translate.Z);

	/*
	int xScaleFactorStart = 48;
	xScaleFactorStart -= 8;
	int Adjustment = (xScaleFactorStart / 8)*(400);
	FVector newPoint = FVector(translate.X, translate.Y + Adjustment, translate.Z);

	transform.SetTranslation(newPoint);
	Cube->SetActorTransform(transform);
	*/

	//FVector newPoint = FVector(translate.X + (bounds1.BoxExtent.X / 2) + baseBounds.BoxExtent.X + 750, translate.Y, translate.Z);


	int xScaleFactorStart = 48;
	xScaleFactorStart -= 8;
	int Adjustment = (xScaleFactorStart / 8)*(400);
	FVector newPoint = FVector(translate.X + Adjustment, translate.Y, translate.Z);


	//transform.SetTranslation(newPoint);
	//Cube->SetActorTransform(transform);

	TInlineComponentArray<UMeshComponent*> MeshComps(Cube);
	for (const auto& MeshComponent : MeshComps) {
		int num = MeshComponent->GetNumMaterials();
		if (num) {
			for (int i = 0; i < num; i++) {
				MeshComponent->SetMaterial(i, CubeMaterial);
			}
		}
	}

	// TODO REWORK THIS DO WE STILL NEED?
	/*
	for (TObjectIterator<AFGBuildable> Worker; Worker; ++Worker) {
		AFGBuildable* buildablePtr = *Worker;

		InstanceHandles = buildablePtr->mInstanceHandles;

		if (InstanceHandles.Num() > 0) {
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				
				bool foundCollision = false;

				TArray<int32> instances;
				instances = InstanceHandles[i]->GetInstanceComponent()->GetInstancesOverlappingBox(bounds1.GetBox(), true);


				if (instances.Num() > 0) {


					for (int q = 0; q < instances.Num(); q++) {
						if (InstanceHandles[i]->GetHandleID() == instances[q]) {
							Design->AddElement(buildablePtr);
							Design->BuildableMark.Add(buildablePtr, 1);

							FSBuildable Buildable;
							Buildable.Buildable = buildablePtr;
							EnableHightLight(Buildable, SelectMaterial);
							foundCollision = true;
							break;
						}
					}
				}

				if (foundCollision) break;

			}
		} else {
			FBoxSphereBounds bounds2 = buildablePtr->GetRootComponent()->Bounds;
			bool Overlap = FBoxSphereBounds::BoxesIntersect(bounds1, bounds2);
			if (Overlap) {
				Design->AddElement(buildablePtr);
				Design->BuildableMark.Add(buildablePtr, 1);

				FSBuildable Buildable;
				Buildable.Buildable = buildablePtr;
				EnableHightLight(Buildable, SelectMaterial);
			}
		}
	}
	*/

}

bool UFSSelection::ConnectSelect(AFGBuildable* Buildable)
{
	if (!ConnectSelectService->Ready()) return false;

	// TODO REWORK THIS
	/*
	if (Design->IsElementSelected(Buildable)) {
		if (ConnectSelectService->SubmitConnectSelectTask(Design, Buildable, 2)) {
			LastSelectMode = 2;
			return true;
		}
	}
	else {
		if (ConnectSelectService->SubmitConnectSelectTask(Design, Buildable, 1)) {
			LastSelectMode = 1;
			return true;
		}
	}
	*/
	return false;
}

bool UFSSelection::ConnectSelectCheckReady()
{
	if (!ConnectSelectService->Ready()) return false;

	TArray<FSBuildable>* Result;
	if (!ConnectSelectService->GetLastResult(Result)) return false;

	//TArray<FSBuildable*> Result;


	if (Result->Num() == 0) return true; //some error

	SetAutoRebuildTreeAll(false);

	if (LastSelectMode == 1) {
		for (FSBuildable& Ptr : *Result) {
			//AFGBuildable* Buildable = Ptr.Get();
			if (Ptr != BuildableEmpty) {
				EnableHightLight(Ptr, SelectMaterial);
				Design->BuildableSet.Add(Ptr);
			}
		}
		Design->BuildableMark.Add((*Result)[0], 2);
	}
	else {
		for (FSBuildable& Ptr : *Result) {
			//AFGBuildable* Buildable = Ptr.Get();
			if (&Ptr) {
				//FSBuildable Buildable;
				//Buildable.Buildable = Ptr.Get();
				DisableHightLight(Ptr);
				Design->BuildableSet.Remove(Ptr);
			}
		}
		Design->BuildableMark.Add((*Result)[0], 0);
	}

	SetAutoRebuildTreeAll(true);

	return true;
}

bool UFSSelection::RectSelectStart(const FVector2D& Start, bool SelectPositive)
{
	if (RectSelectService->StartRectSelect(Design, Start, SelectPositive)) {
		RectSelectBuffer.Empty();
		return true;
	}
	return false;
}

bool UFSSelection::RectSelectCheckReady()
{
	if (!RectSelectService->Ready()) return false;

	for (TObjectIterator<UHierarchicalInstancedStaticMeshComponent> Itr; Itr; ++Itr)
	{
		UHierarchicalInstancedStaticMeshComponent* Component = *Itr;
		if (!Component->IsTreeFullyBuilt()) return false;
	}

	TArray<TWeakObjectPtr<AFGBuildable> >* Positive;
	TArray<TWeakObjectPtr<AFGBuildable> >* Nagetive;
	if (!RectSelectService->GetLastResult(Positive, Nagetive)) return false;

	SetAutoRebuildTreeAll(false);

	for (TWeakObjectPtr<AFGBuildable>& Ptr : *Positive) {
		//AFGBuildable* Buildable = Ptr.Get();
		if (Ptr.Get()) {
			FSBuildable Buildable;
			Buildable.Buildable = Ptr.Get();
			EnableHightLight(Buildable, SelectMaterial);
		}
		int *Value = RectSelectBuffer.Find(Ptr);
		if (!Value) RectSelectBuffer.Add(Ptr, 1);
		else (*Value)++;
	}

	for (TWeakObjectPtr<AFGBuildable>& Ptr : *Nagetive) {
		//AFGBuildable* Buildable = Ptr.Get();
		FSBuildable Buildable;
		Buildable.Buildable = Ptr.Get();
		DisableHightLight(Buildable);
		int* Value = RectSelectBuffer.Find(Ptr);
		if (!Value) RectSelectBuffer.Add(Ptr, -1);
		else (*Value)--;
	}

	SetAutoRebuildTreeAll(true);
	return true;
}

bool UFSSelection::RectSelectUpdate(FVector2D Cursor)
{
	if (!RectSelectService->Ready()) return false;

	return RectSelectService->QueryRectSelect(Cursor.X, Cursor.Y);
}

void UFSSelection::RectSelectExit(bool Valid)
{
	RectSelectService->TerminalCurrentTask();
	if (Valid) {

		// TODO REWORK THIS
		/*
		for (TPair<TWeakObjectPtr<AFGBuildable>, int>& Pair : RectSelectBuffer) {
			AFGBuildable* Buildable = Pair.Key.Get();
			if (!Buildable) continue;
			if (Pair.Value == -1) {
				Design->RemoveElement(Buildable);
				Design->BuildableMark.Remove(Buildable);
			}
			else if (Pair.Value == 1) {
				Design->AddElement(Buildable);
			}
		}
		*/
	}
	else {
		SetAutoRebuildTreeAll(false);

		// TODO REWORK THIS
		/*
		for (TPair<TWeakObjectPtr<AFGBuildable>, int>& Pair : RectSelectBuffer) {
			//AFGBuildable* Buildable = Pair.Key.Get();
			if (Pair.Value == -1 && Pair.Key.Get()) {
				if (Pair.Key.Get() == Design->Anchor.Get()) {
					FSBuildable Buildable;
					Buildable.Buildable = Pair.Key.Get();
					EnableHightLight(Buildable, FocusMaterial);
				} else {
					FSBuildable Buildable;
					Buildable.Buildable = Pair.Key.Get();
					EnableHightLight(Buildable, SelectMaterial);
				}
			} else if (Pair.Value == 1) {
				FSBuildable Buildable;
				Buildable.Buildable = Pair.Key.Get();
				DisableHightLight(Buildable);
			}
		}
		*/
		SetAutoRebuildTreeAll(true);
	}
	RectSelectBuffer.Empty();
}

void UFSSelection::Load(UFSDesign* Design)
{
	this->Design = Design;
	ActorMaterialCache.Empty();
	DynamicInstanceSet.Empty();
	CurrentFocus = FSBuildable();
}

void UFSSelection::LoadSelect(UFSDesign* Design)
{
	this->Design = Design;
	this->Design->RecheckNullptr();

	ActorMaterialCache.Empty();
	DynamicInstanceSet.Empty();


	//SetAutoRebuildTreeAll(false);
	

	// TODO REWORK THIS
	
	if (!Design->BuildableSet.IsEmpty()) {
		for (FSBuildable BuildablePtr : Design->BuildableSet) {
			//if (BuildablePtr.Get()) {
			//FSBuildable Buildable;
			//Buildable.Buildable = BuildablePtr.Get();
			EnableHightLight(BuildablePtr);
		}

		if (Design->Anchor != BuildableEmpty) {
			//FSBuildable Buildable;
			//Buildable.Buildable = Design->Anchor.Get();
			EnableHightLight(Design->Anchor, this->FocusMaterial);
		}
	}
	
	
	//SetAutoRebuildTreeAll(true);


	//UFSHologramService* HologramService = NewObject<UFSHologramService>(this);
	//HologramService->Init();

	ConnectSelectService->RefreshRecipe();
	CurrentFocus = FSBuildable();
}

void UFSSelection::LoadSetAnchor(UFSDesign* Design)
{
	this->Design = Design;
	this->Design->RecheckNullptr();

	ActorMaterialCache.Empty();
	DynamicInstanceSet.Empty();

	SetAutoRebuildTreeAll(false);

	// TODO REWORK THIS
	
	for (FSBuildable BuildablePtr : Design->BuildableSet) {
		if (BuildablePtr != FSBuildable()) {
			//FSBuildable Buildable;
			//Buildable.Buildable = BuildablePtr.Get();
			EnableHightLight(BuildablePtr);
		}
	}
	
	SetAutoRebuildTreeAll(true);

	CurrentFocus = FSBuildable();
}

void UFSSelection::SetAutoRebuildTreeAll(bool Auto)
{
	for (TObjectIterator<UHierarchicalInstancedStaticMeshComponent> Itr; Itr; ++Itr)
	{
		UHierarchicalInstancedStaticMeshComponent* Component = *Itr;
		if (Auto && Component->IsRenderStateDirty())
			Component->BuildTreeIfOutdated(true, false);
		Component->bAutoRebuildTreeOnInstanceChanges = Auto;
	}
}

void UFSSelection::Unload()
{
	this->ConnectSelectService->TerminalCurrentTask();
	this->RectSelectService->TerminalCurrentTask();
	this->Design = nullptr;

	SetAutoRebuildTreeAll(false);
	DisableAll();
	SetAutoRebuildTreeAll(true);

	ActorMaterialCache.Empty();
	DynamicInstanceSet.Empty();
	CurrentFocus = FSBuildable();
	//SelectedMap.Empty();
}

void UFSSelection::BeginDestroy()
{
	if (ISMMapping.Num()) {
		for (TPair<TPair<UFGColoredInstanceManager*, UMaterialInterface* >, FSISMNode*> Pair : ISMMapping) {
			Pair.Value->Handles.Empty();
			delete Pair.Value;
		}
		ISMMapping.Empty();
	}
	Super::BeginDestroy();
}

bool UFSSelection::IsHightLight(FSBuildable Buildable)
{
	return ActorMaterialCache.Contains(Buildable);
}

void UFSSelection::EnableHightLight(FSBuildable Buildable)
{
	FSActorMaterial& Cache = ActorMaterialCache.FindOrAdd(Buildable);
	FSActorMaterial& Cache2 = ActorMaterialCache2.FindOrAdd(Buildable);
	if (Cache.Init) return;
	Cache.Buildable = Buildable.Buildable;
	EnableHightLight(Cache, Buildable, SelectMaterial);
}

void UFSSelection::EnableHightLight(FSBuildable Buildable, UMaterialInterface* Material)
{
 	FSActorMaterial& Cache = ActorMaterialCache.FindOrAdd(Buildable);
	if (!Cache.Init) Cache.Buildable = Buildable.Buildable;
	EnableHightLight(Cache, Buildable, Material);
}	

void UFSSelection::HideHologram(AActor* Actor, FSActorMaterial& ActorInfo)
{
	USceneComponent* sceneComponent = Actor->GetRootComponent();
	if (sceneComponent) {

	}
	else {
	}
	//FSMaterialHandle* handle = SelectedMap.Find(Actor);
	if (Actor && sceneComponent) {
		AFGBuildable* buildable = nullptr;
		buildable = Cast<AFGBuildable>(Actor);
		//auto field = UMeshComponent::StaticClass();
		//TArray<UActorComponent*> components = Actor->GetComponentsByClass(field);
		//int max = components.Max();
		if (buildable) {
			if (!buildable->GetIsDismantled()) {
				//TInlineComponentArray<UMeshComponent*> MeshComps(Actor);
				for (const auto& ActorComp : Actor->K2_GetComponentsByClass(UMeshComponent::StaticClass())) {
					//for (auto& ActorComp : components) {
					UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
						//
					if (MeshComponent) {
						FString Name = MeshComponent->GetName();
						if (!buildable->GetClass()->IsChildOf<AFGBuildableWire>()) {
							if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
						}
						// retrieve materials for this mesh from the cache for this actor
						auto SavedMaterialInterfacePtr = ActorInfo.SavedMaterialInterfaces.Find(MeshComponent);
						if (SavedMaterialInterfacePtr) {
							auto SavedMaterialInterface = *SavedMaterialInterfacePtr;

							// restore all materials on this mesh
							int Mats = SavedMaterialInterface.MaterialInterfaces.Num();
							for (int i = 0; i < Mats; i++) {
								MeshComponent->SetMaterial(i, SavedMaterialInterface.MaterialInterfaces[i]);
							}
						}
					}
				}
			}
		}
	}
	/*
	AFSkyline* FSkyline = AFSkyline::Get(this);
	// hack to refresh static mesh - This is needed for power indicators and meshes to update properly.
	auto OutlineComp = UFGOutlineComponent::Get(FSkyline->FSCtrl->World);
	OutlineComp->ShowDismantlePendingMaterial(Actor);
	//OutlineComp->HideAllDismantlePendingMaterial();
	*/
}

void UFSSelection::BuildStaticMeshOrigin(AFGBuildable* Buildable, UStaticMesh* StaticMesh, UMaterialInterface* Material)
{
	/*
	AFSkyline* FSkyline = AFSkyline::Get(this);
	//FBoxSphereBounds ExtendedBounds = StaticMesh->ExtendedBounds;
	FBoxSphereBounds ExtendedBounds = StaticMesh->GetBounds();
	FTransform BuildableTransform = Buildable->GetTransform();
	FVector BuildableTranslate = Buildable->GetTransform().GetTranslation();
	FVector StaticMeshOrigin = ExtendedBounds.Origin;
	FVector StaticMeshWorldPoint = FVector(StaticMeshOrigin.X + BuildableTranslate.X, StaticMeshOrigin.Y + BuildableTranslate.Y, StaticMeshOrigin.Z + BuildableTranslate.Z);
	//FVector StaticMeshWorldPoint = FVector(BuildableTranslate.X, BuildableTranslate.Y, BuildableTranslate.Z - CustomAdjustment);
	BuildableTransform.SetTranslation(StaticMeshWorldPoint);

	Cube = (AMyActor*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AMyActor::StaticClass(), BuildableTransform, *(new FActorSpawnParameters()));
	Cube->InitializeTest(StaticMesh);
	Cube->SetActorEnableCollision(false);

	TInlineComponentArray<UMeshComponent*> MeshComps(Cube);
	for (const auto& MeshComponent : MeshComps) {
		int num = MeshComponent->GetNumMaterials();
		if (num) {
			for (int i = 0; i < num; i++) {
				MeshComponent->SetMaterial(i, SelectMaterial);
			}
		}
	}
	*/
}

void UFSSelection::TestMethod(AFGBuildable* Buildable) {

	AFSkyline* FSkyline = AFSkyline::Get(this);

	FSActorMaterial Value = FSActorMaterial();

	InstanceHandles = Buildable->mInstanceHandles;
	if (InstanceHandles.Num() > 0) {
		for (int i = 0; i < InstanceHandles.Num(); i++) {
			InstanceHandles[i]->HideInstance(true);

			if (Value.HologramHelper == nullptr) {
				Value.HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Buildable->GetActorTransform(), *(new FActorSpawnParameters()));
			}

			UStaticMeshComponent* comp2 = NewObject<UStaticMeshComponent>(Value.HologramHelper);

			FTransform InstanceTransform;
			InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
			FTransform InstanceRelativeTransform = InstanceHandles[i]->GetInstanceComponent()->GetRelativeTransform();
			const FTransform InstanceSpawnLocation = InstanceRelativeTransform * Buildable->GetActorTransform();

			comp2->SetStaticMesh(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh());

			FInstanceData instanceData = FInstanceData();
			instanceData.OverridenMaterials.Add(this->SelectMaterial);

			comp2->OverrideMaterials = instanceData.OverridenMaterials;
			comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			comp2->SetGenerateOverlapEvents(false);
			comp2->SetMobility(InstanceHandles[i]->GetInstanceComponent()->Mobility);
			comp2->AttachToComponent(Value.HologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			comp2->SetRelativeTransform(FTransform::Identity);
			comp2->SetVisibility(true);
			//comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
			comp2->RegisterComponent();

			//comp2->AddInstanceWorldSpace(InstanceTransform);

			//TArray<UActorComponent*> ActorComp;

			//AHologramHelper* HologramHelper;
			//ActorComp = HologramHelper->K2_GetComponentsByClass(UMeshComponent::StaticClass());

			//ActorComp = Cast<AHologramHelper>(Value.HologramHelper).

			for (const auto& ActorComp : Value.HologramHelper->K2_GetComponentsByClass(UMeshComponent::StaticClass())) {
				UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
				if (MeshComponent) {
					int num = MeshComponent->GetNumMaterials();
					if (num) {
						for (int i = 0; i < num; i++) {

							MeshComponent->SetMaterial(i, this->SelectMaterial);
						}
					}
				}
			}

			/*
			if (Value.HologramHelper == nullptr) {
				Value.HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Buildable->GetActorTransform(), *(new FActorSpawnParameters()));
			}

			UHierarchicalInstancedStaticMeshComponent* comp2 = NewObject<UHierarchicalInstancedStaticMeshComponent>(Value.HologramHelper);

			FTransform InstanceTransform;
			InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
			FTransform InstanceRelativeTransform = InstanceHandles[i]->GetInstanceComponent()->GetRelativeTransform();
			const FTransform InstanceSpawnLocation = InstanceRelativeTransform * Buildable->GetActorTransform();

			comp2->SetStaticMesh(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh());

			FInstanceData instanceData = FInstanceData();
			instanceData.OverridenMaterials.Add(this->SelectMaterial);

			comp2->OverrideMaterials = instanceData.OverridenMaterials;
			comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			comp2->SetGenerateOverlapEvents(false);
			comp2->SetMobility(InstanceHandles[i]->GetInstanceComponent()->Mobility);
			comp2->AttachToComponent(Value.HologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			comp2->SetRelativeTransform(FTransform::Identity);
			comp2->SetVisibility(true);
			comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
			comp2->RegisterComponent();

			comp2->AddInstanceWorldSpace(InstanceTransform);

			for (const auto& ActorComp : Value.HologramHelper->GetComponentsByClass(UMeshComponent::StaticClass())) {
				UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
				if (MeshComponent) {
					int num = MeshComponent->GetNumMaterials();
					if (num) {
						for (int i = 0; i < num; i++) {
							MeshComponent->SetMaterial(i, this->SelectMaterial);
						}
					}
				}
			}
			*/

		}
	}
}

UInstancedStaticMeshComponent* UFSSelection::MakeOutlineProxyMeshComponent(AActor* Buildable, UStaticMesh* StaticMesh) {

	UInstancedStaticMeshComponent* OutlineProxyMeshComponent = NewObject<UInstancedStaticMeshComponent>(Buildable);

	OutlineProxyMeshComponent->AttachToComponent(Buildable->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	OutlineProxyMeshComponent->SetStaticMesh(StaticMesh);
	OutlineProxyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Disable shadow, disable LODs and disable Nanite for meshes that have it enabled
	//OutlineProxyMeshComponent->SetCastShadow(false);
	//OutlineProxyMeshComponent->SetForcedLodModel(0);
	//OutlineProxyMeshComponent->bDisallowNanite = true;

	// Do not render the component in the main or depth pass, but render it in custom depth pass to show the outline
	OutlineProxyMeshComponent->SetRenderInDepthPass(false);
	OutlineProxyMeshComponent->SetRenderInMainPass(false);
	//UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);

	OutlineProxyMeshComponent->RegisterComponent();
	return OutlineProxyMeshComponent;
}

void UFSSelection::EnableHightLight(FSActorMaterial& Cache, FSBuildable Buildable, UMaterialInterface* Material)
{

	AFSkyline* FSkyline = AFSkyline::Get(this);
	bool CacheExist = Cache.Init;

	if (Buildable.BuildableClass == nullptr) {
		if (CacheExist && !Cache.Buildable.Get()) {
			DisableHightLight(Buildable);
			return;
		}
	}
	bool initializeCacheSaved = CacheExist;

	bool UseDefaultOutline = false;

	if (FSkyline->FSCtrl->Etc->GetBool("UseDefaultHologram")) {
		UseDefaultOutline = false;
	}

	//initializeCacheSaved = SelectedMap.Contains(Buildable);

	/*
	for (TPair<AActor*, FSelectedActorInfo>& pair : SelectedMap) {
		AActor* actor1 = pair.Key;
		FString Name1 = actor1->GetName();
		FString Name2 = Buildable->GetName();
		if (Name1.Equals(Name2)) {
			initializeCacheSaved = true;
			break;
		}
	}
	*/

	//FSelectedActorInfo* handleHolder = SelectedMap.Find(Buildable);

	//FSelectedActorInfo Value = FSelectedActorInfo();


	//ActorList.AddUnique(Buildable);

	bool foundCustomStaticMesh = false;
	float CustomAdjustment = 0;
	bool useNormalHologram = false;

	//Manager = AAbstractInstanceManager::GetInstanceManager(FSkyline->World);

	//testBuildable = Buildable.Buildable;

	//isValid = false;

	//if (Buildable.Buildable.IsValid()) {
	//	isValid = true;
	//}


	if (!Buildable.Buildable) {

		if (!initializeCacheSaved) {
			if (!UseDefaultOutline) {
				AFSkyline* FSkyline = AFSkyline::Get(this);
				AAbstractInstanceManager* Manager = AAbstractInstanceManager::GetInstanceManager(FSkyline->World);

				FInstanceHandle Handle = Buildable.Handle;

				AFGLightweightBuildableSubsystem* lightweightSubsystem;

				lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(FSkyline->World);


				/*
				TSubclassOf< AFGBuildable > BuildableClass = Buildable.BuildableClass;
				FMaterialPair& MaterialPair = AbstractMaterialMap.FindOrAdd(BuildableClass);


				if (Material == SelectMaterial) {
					if (!MaterialPair.SelectActor) {
						FTransform ft;
						MaterialPair.SelectActor = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), ft, *(new FActorSpawnParameters()));
						FTransform InstanceTransform;
						Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform, true);


						UHierarchicalInstancedStaticMeshComponent* OriginalHISMC = Handle.GetInstanceComponent();


						comp2 = NewObject<UHierarchicalInstancedStaticMeshComponent>(MaterialPair.SelectActor);


						FTransform InstanceRelativeTransform = Handle.GetInstanceComponent()->GetRelativeTransform();
						const FTransform InstanceSpawnLocation = InstanceRelativeTransform * MaterialPair.SelectActor->GetActorTransform();

						comp2->SetStaticMesh(Handle.GetInstanceComponent()->GetStaticMesh());

						FInstanceData instanceData = FInstanceData();
						instanceData.OverridenMaterials.Add(Material);

						//comp2->OverrideMaterials = instanceData.OverridenMaterials;
						comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						comp2->SetGenerateOverlapEvents(false);
						comp2->SetMobility(Handle.GetInstanceComponent()->Mobility);
						comp2->AttachToComponent(MaterialPair.SelectActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

						comp2->SetRelativeTransform(FTransform::Identity);
						comp2->SetVisibility(true);
						//comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
						comp2->RegisterComponent();

						FTransform InstanceTransform2;
						Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform2, true);

						comp2->AddInstance(InstanceTransform2, true);

						MaterialPair.Selectcomp2 = comp2;

						Handle.HideInstance(true);
					}
					else {

						FTransform InstanceTransform2;
						Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform2, true);

						MaterialPair.Selectcomp2->AddInstance(InstanceTransform2, true);

						Handle.HideInstance(true);

					}
				}
				if (Material == FocusMaterial) {
					if (!MaterialPair.FocusActor) {
						FTransform ft;
						MaterialPair.FocusActor = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), ft, *(new FActorSpawnParameters()));
						FTransform InstanceTransform;
						Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform, true);


						UHierarchicalInstancedStaticMeshComponent* OriginalHISMC = Handle.GetInstanceComponent();


						comp2 = NewObject<UHierarchicalInstancedStaticMeshComponent>(MaterialPair.FocusActor);


						FTransform InstanceRelativeTransform = Handle.GetInstanceComponent()->GetRelativeTransform();
						const FTransform InstanceSpawnLocation = InstanceRelativeTransform * MaterialPair.FocusActor->GetActorTransform();

						comp2->SetStaticMesh(Handle.GetInstanceComponent()->GetStaticMesh());

						FInstanceData instanceData = FInstanceData();
						instanceData.OverridenMaterials.Add(Material);

						//comp2->OverrideMaterials = instanceData.OverridenMaterials;
						comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						comp2->SetGenerateOverlapEvents(false);
						comp2->SetMobility(Handle.GetInstanceComponent()->Mobility);
						comp2->AttachToComponent(MaterialPair.FocusActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

						comp2->SetRelativeTransform(FTransform::Identity);
						comp2->SetVisibility(true);
						//comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
						comp2->RegisterComponent();

						FTransform InstanceTransform2;
						Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform2, true);

						comp2->AddInstance(InstanceTransform2, true);

						MaterialPair.Focuscomp2 = comp2;

						Handle.HideInstance(true);
					}
					else {

						//FTransform InstanceTransform2;
						//Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform2, true);

						//MaterialPair.Focuscomp2->AddInstance(InstanceTransform2, true);

						//Handle.HideInstance(true);

						
						//UHierarchicalInstancedStaticMeshComponent* HISMC = MaterialPair.Focuscomp2;
						//AsyncTask(ENamedThreads::GameThread, [HISMC, InstanceTransform2, Buildable]() {
							//HISMC->AddInstance(InstanceTransform2, true);
							//FInstanceHandle Handle = Buildable.Handle;
							//Handle.HideInstance(true);
							//});
						

						
						//UHierarchicalInstancedStaticMeshComponent* HISMC = MaterialPair.Focuscomp2;
						//TArray<FTransform> Transforms;
						//Transforms.Add(InstanceTransform2);
						//TArray<FInstancedStaticMeshInstanceData>& InstanceData = HISMC->PerInstanceSMData;
						//ParallelFor(Transforms.Num(), [&](int32 Index) {
							//FTransform Transform = Transforms[Index];
							//FInstancedStaticMeshInstanceData NewData;
							//NewData.Transform = Transform.ToMatrixWithScale();
							//InstanceData.Add(NewData);
							//});
						
						//Handle.HideInstance(true);
						//HISMC->UpdateInstanceBuffer();

						//Handle.GetInstanceComponent()->SetCustomDataValue(Handle.GetHandleID(), 0, 0.0f, true);


					}
				}
				*/

				if (Cache.HologramHelper == nullptr) {
					FTransform ft;
					Cache.HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), ft, *(new FActorSpawnParameters()));
				}

				FTransform InstanceTransform;
				Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform, true);

				if (!Cache.copiedComponents.Contains(Handle.GetInstanceComponent()->GetStaticMesh())) {


					UHierarchicalInstancedStaticMeshComponent* OriginalHISMC = Handle.GetInstanceComponent();


					comp2 = NewObject<UHierarchicalInstancedStaticMeshComponent>(Cache.HologramHelper);


					FTransform InstanceRelativeTransform = Handle.GetInstanceComponent()->GetRelativeTransform();
					const FTransform InstanceSpawnLocation = InstanceRelativeTransform * Cache.HologramHelper->GetActorTransform();

					comp2->SetStaticMesh(Handle.GetInstanceComponent()->GetStaticMesh());

					FInstanceData instanceData = FInstanceData();
					instanceData.OverridenMaterials.Add(Material);

					comp2->OverrideMaterials = instanceData.OverridenMaterials;
					comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					comp2->SetGenerateOverlapEvents(false);
					comp2->SetMobility(Handle.GetInstanceComponent()->Mobility);
					comp2->AttachToComponent(Cache.HologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

					comp2->SetRelativeTransform(FTransform::Identity);
					comp2->SetVisibility(true);
					//comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
					comp2->RegisterComponent();

					Cache.copiedComponents.Add(Handle.GetInstanceComponent()->GetStaticMesh(), comp2);

				}



				//}

				FTransform InstanceTransform2;
				Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform2, true);

				//comp2->AddInstance(InstanceTransform2, true);

				UStaticMesh* staticmesh = Handle.GetInstanceComponent()->GetStaticMesh();

				//Cache.copiedComponents.Find(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())

				UHierarchicalInstancedStaticMeshComponent* instance = *Cache.copiedComponents.Find(staticmesh);
				instance->AddInstance(InstanceTransform, true);

				//Cache.InstanceHandles.Add(Handle);
				Cache.Handle = Handle;


				Cache.Handle.HideInstance(true);
			}
			else {

				Cache.UseDefaultOutline = UseDefaultOutline;

				FInstanceHandle Handle = Buildable.Handle;

				//FTransform InstanceTransform;
				//Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform, true);

				//if (!OutlineProxyData.Contains(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())) {

				if (Cache.HologramHelper == nullptr) {
					Cache.HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Buildable.Transform, *(new FActorSpawnParameters()));
				}

				UInstancedStaticMeshComponent* OutlineProxyMeshComponent = this->MakeOutlineProxyMeshComponent(Cache.HologramHelper, Handle.GetInstanceComponent()->GetStaticMesh());

				if (Material == SelectMaterial) {
					UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);
				}
				if (Material == FocusMaterial) {
					UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
				}

				Cache.OutlineProxyData.Add(Handle.GetInstanceComponent()->GetStaticMesh(), OutlineProxyMeshComponent);

				FTransform InstanceTransform;
				Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform, true);
				int32 instanceID = OutlineProxyMeshComponent->AddInstance(InstanceTransform, true);


				//Value.OutlineProxyData.FindChecked(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())->AddInstance(InstanceTransform, true);
				//OutlineProxyMeshComponent->ClearInstances();
				//OutlineProxyMeshComponent->RemoveInstance(instanceID);
				//}

			}
			
		}
		else {
			if (!Cache.UseDefaultOutline) {
				for (TPair<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*>& Pair : Cache.copiedComponents) {
					useNormalHologram = false;

					TArray<UMaterialInterface*> OverridenMaterials;
					OverridenMaterials.Add(Material);
					Pair.Value->OverrideMaterials = OverridenMaterials;
					Pair.Value->MarkRenderTransformDirty();

					/*
					for (const auto& ActorComp : Cache.HologramHelper->GetComponentsByClass(UMeshComponent::StaticClass())) {
						UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
						if (MeshComponent) {
							int num = MeshComponent->GetNumMaterials();
							if (num) {
								for (int i = 0; i < num; i++) {
									MeshComponent->SetMaterial(i, Material);
								}
							}
						}
					}
					*/
				}
			}
			if (Cache.UseDefaultOutline) {
				if (Cache.HologramHelper) {
					for (const auto& ActorComp : Cache.HologramHelper->K2_GetComponentsByClass(UMeshComponent::StaticClass())) {
						UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
						if (MeshComponent) {
							if (Material == SelectMaterial) {
								UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_HOLOGRAM);
							}
							if (Material == FocusMaterial) {
								UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
							}
						}
					}
				}
			}
		}

			//
	}//

	if (Buildable.Buildable) {
		InstanceHandles = Buildable.Buildable->mInstanceHandles;


		//Data = Buildable.Buildable->GetLightweightInstanceData();
		//TArray<FInstanceData> Instances;

		//TODO IMPLEMENT UI TOGGLE to use this more optimized outline methods if the user wants

		// this is to handle outline holograms for buildables

		if (!initializeCacheSaved) {
			if (UseDefaultOutline) {
				Cache.UseDefaultOutline = UseDefaultOutline;


				for (const auto& ActorComp : Buildable.Buildable->K2_GetComponentsByClass(UMeshComponent::StaticClass())) {
					UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);

					if (MeshComponent) {

						FString Name = MeshComponent->GetName();
						if (!Buildable.Buildable->GetClass()->IsChildOf<AFGBuildableWire>()) {
							if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
						}
						if (Material == SelectMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_HOLOGRAM);
						}
						if (Material == FocusMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
						}

						const UFGColoredInstanceMeshProxy* MeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComponent);
						if (MeshProxy && MeshProxy->mInstanceHandle.IsInstanced())
						{

							//UInstancedStaticMeshComponent** InstancedStaticMeshComponent = OutlineProxyData.Find(MeshProxy->GetStaticMesh());

							//if (!InstancedStaticMeshComponent) {

							if (!CacheExist) {

								if (Cache.HologramHelper == nullptr) {
									Cache.HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Buildable.Buildable->GetActorTransform(), *(new FActorSpawnParameters()));
								}

								UInstancedStaticMeshComponent* OutlineProxyMeshComponent = this->MakeOutlineProxyMeshComponent(Cache.HologramHelper, MeshProxy->GetStaticMesh());

								if (Material == SelectMaterial) {
									UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);
								}
								if (Material == FocusMaterial) {
									UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
								}

								Cache.OutlineProxyData.Add(MeshProxy->GetStaticMesh(), OutlineProxyMeshComponent);

								int32 instanceID = OutlineProxyMeshComponent->AddInstance(MeshProxy->GetComponentTransform(), true);


								//Value.OutlineProxyData.FindChecked(MeshProxy->GetStaticMesh())->AddInstance(MeshProxy->GetComponentTransform(), true);
							}
						}

					}

				}



				if (InstanceHandles.Num() > 0) {
					//for (int j = 0; j < 20; j++) {
					for (int i = 0; i < InstanceHandles.Num(); i++) {

						//if (!OutlineProxyData.Contains(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())) {

						if (Cache.HologramHelper == nullptr) {
							Cache.HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Buildable.Buildable->GetActorTransform(), *(new FActorSpawnParameters()));
						}

						UInstancedStaticMeshComponent* OutlineProxyMeshComponent = this->MakeOutlineProxyMeshComponent(Cache.HologramHelper, InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh());

						if (Material == SelectMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);
						}
						if (Material == FocusMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
						}

						Cache.OutlineProxyData.Add(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh(), OutlineProxyMeshComponent);

						FTransform InstanceTransform;
						InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
						int32 instanceID = OutlineProxyMeshComponent->AddInstance(InstanceTransform, true);


						//Value.OutlineProxyData.FindChecked(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())->AddInstance(InstanceTransform, true);
						//OutlineProxyMeshComponent->ClearInstances();
						//OutlineProxyMeshComponent->RemoveInstance(instanceID);
						//}

					}
					//}
				}


			}
		}
		else {
			//if (handleHolder) {
			if (Cache.UseDefaultOutline) {

				//FSkyline->FSCtrl->GetPlayer()->GetOutline()->HideActorOutline(Buildable);

				for (const auto& ActorComp : Buildable.Buildable->K2_GetComponentsByClass(UMeshComponent::StaticClass())) {
					UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);

					if (MeshComponent) {

						FString Name = MeshComponent->GetName();
						if (!Buildable.Buildable->GetClass()->IsChildOf<AFGBuildableWire>()) {
							if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
						}
						if (Material == SelectMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_HOLOGRAM);
						}
						if (Material == FocusMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
						}

						const UFGColoredInstanceMeshProxy* MeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComponent);
						if (MeshProxy && MeshProxy->mInstanceHandle.IsInstanced()) {
							if (CacheExist) {

								UInstancedStaticMeshComponent* OutlineProxyMeshComponent = *Cache.OutlineProxyData.Find(MeshProxy->GetStaticMesh());;

								if (Material == SelectMaterial) {
									UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);
								}
								if (Material == FocusMaterial) {
									UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
								}

							}
						}

					}
				}

				/*
				if (InstanceHandles.Num() > 0) {
					for (int i = 0; i < InstanceHandles.Num(); i++) {

						//if (!OutlineProxyData.Contains(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())) {

						UInstancedStaticMeshComponent* OutlineProxyMeshComponent = *Cache.OutlineProxyData.Find((InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh()));
						//UInstancedStaticMeshComponent* OutlineProxyMeshComponent = this->MakeOutlineProxyMeshComponent(Buildable, InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh());

						if (Material == SelectMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);
						}
						if (Material == FocusMaterial) {
							UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
						}

						//Value.OutlineProxyData.Add(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh(), OutlineProxyMeshComponent);

						FTransform InstanceTransform;
						InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
						int32 instanceID = OutlineProxyMeshComponent->AddInstance(InstanceTransform, true);


						//Value.OutlineProxyData.FindChecked(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())->AddInstance(InstanceTransform, true);
						//OutlineProxyMeshComponent->ClearInstances();
						//OutlineProxyMeshComponent->RemoveInstance(instanceID);
						//}
					}
				}
				*/

				if (Cache.HologramHelper) {
					for (const auto& ActorComp : Cache.HologramHelper->K2_GetComponentsByClass(UMeshComponent::StaticClass())) {
						UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
						if (MeshComponent) {
							if (Material == SelectMaterial) {
								UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_HOLOGRAM);
							}
							if (Material == FocusMaterial) {
								UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
							}
						}
					}
				}

				/*
				for (TPair<UStaticMesh*, UInstancedStaticMeshComponent*>& Pair : Cache.OutlineProxyData) {
					UInstancedStaticMeshComponent* OutlineProxyMeshComponent = Pair.Value;

					if (Material == SelectMaterial) {
						UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);
					}
					if (Material == FocusMaterial) {
						UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_INVALIDHOLOGRAM);
					}

				}
				*/
			}
			//}
		}
		//return;
		/*
		for (const auto& ActorComp : Buildable->GetComponentsByClass(UMeshComponent::StaticClass())) {
			UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);

			if (MeshComponent) {

				FString Name = MeshComponent->GetName();
				if (!Buildable->GetClass()->IsChildOf<AFGBuildableWire>()) {
					if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
				}

				UFGBlueprintFunctionLibrary::ShowOutline(MeshComponent, EOutlineColor::OC_HOLOGRAM);

				const UFGColoredInstanceMeshProxy* MeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComponent);
				if (MeshProxy && MeshProxy->mInstanceHandle.IsInstanced())
				{

					//UInstancedStaticMeshComponent** InstancedStaticMeshComponent = OutlineProxyData.Find(MeshProxy->GetStaticMesh());

					//if (!InstancedStaticMeshComponent) {

					if (!CacheExist) {

						UInstancedStaticMeshComponent* OutlineProxyMeshComponent = NewObject<UInstancedStaticMeshComponent>(Buildable);

						OutlineProxyMeshComponent->SetupAttachment(Buildable->GetRootComponent());
						OutlineProxyMeshComponent->SetStaticMesh(MeshProxy->GetStaticMesh());
						OutlineProxyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

						// Disable shadow, disable LODs and disable Nanite for meshes that have it enabled
						OutlineProxyMeshComponent->SetCastShadow(false);
						OutlineProxyMeshComponent->SetForcedLodModel(0);
						OutlineProxyMeshComponent->bDisallowNanite = true;

						// Do not render the component in the main or depth pass, but render it in custom depth pass to show the outline
						OutlineProxyMeshComponent->SetRenderInDepthPass(false);
						OutlineProxyMeshComponent->SetRenderInMainPass(false);
						UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);

						OutlineProxyMeshComponent->RegisterComponent();
						OutlineProxyData.Add(MeshProxy->GetStaticMesh(), OutlineProxyMeshComponent);

						OutlineProxyData.FindChecked(MeshProxy->GetStaticMesh())->AddInstance(MeshProxy->GetComponentTransform(), true);
					}
					//} else {
						//OutlineProxyData.FindChecked(MeshProxy->GetStaticMesh())->AddInstance(MeshProxy->GetComponentTransform(), true);
					//}

					//continue;
				}

			}

		}
		if (InstanceHandles.Num() > 0) {
			for (int i = 0; i < InstanceHandles.Num(); i++) {

				//if (!OutlineProxyData.Contains(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())) {

					UInstancedStaticMeshComponent* OutlineProxyMeshComponent = NewObject<UInstancedStaticMeshComponent>(Buildable);

					OutlineProxyMeshComponent->SetupAttachment(Buildable->GetRootComponent());
					OutlineProxyMeshComponent->SetStaticMesh(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh());
					OutlineProxyMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

					// Disable shadow, disable LODs and disable Nanite for meshes that have it enabled
					OutlineProxyMeshComponent->SetCastShadow(false);
					OutlineProxyMeshComponent->SetForcedLodModel(0);
					OutlineProxyMeshComponent->bDisallowNanite = true;

					// Do not render the component in the main or depth pass, but render it in custom depth pass to show the outline
					OutlineProxyMeshComponent->SetRenderInDepthPass(false);
					OutlineProxyMeshComponent->SetRenderInMainPass(false);
					UFGBlueprintFunctionLibrary::ShowOutline(OutlineProxyMeshComponent, EOutlineColor::OC_HOLOGRAM);

					OutlineProxyMeshComponent->RegisterComponent();
					OutlineProxyData.Add(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh(), OutlineProxyMeshComponent);

					FTransform InstanceTransform;
					InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);

					OutlineProxyData.FindChecked(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())->AddInstance(InstanceTransform, true);
				//}

			}
		}
		//return;
		*/

		/*
		if (Buildable->Implements<UAbstractInstanceInterface>())
		{
			const FTransform ActorTransform = IAbstractInstanceInterface::Execute_GetLightweightInstanceActorTransform(Actor);

			for (const FInstanceData& InstanceData : IAbstractInstanceInterface::Execute_GetActorLightweightInstanceData(Actor))
			{
				if (InstanceData.StaticMesh != nullptr)
				{
					// World transform is the actor transform with the relative instance transform applied on top
					AddProxyOutlineMeshInstance(InstanceData.StaticMesh, InstanceData.RelativeTransform * ActorTransform);
				}
			}
		}
		*/


		// works and stable only for simple things like foundations, doesnt work with lifts, etc yet

		if (InstanceHandles.Num() > 0) {

			//TArray<uint32> handleIDs;
			//TArray<UHierarchicalInstancedStaticMeshComponent*> copiedComponents;


			for (int i = 0; i < InstanceHandles.Num(); i++) {

				if (!initializeCacheSaved) {

					if (!UseDefaultOutline) {

						//handleIDs.Add(InstanceHandles[i]->GetHandleID());

						if (Cache.HologramHelper == nullptr) {
							Cache.HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Buildable.Buildable->GetActorTransform(), *(new FActorSpawnParameters()));
						}

						FTransform InstanceTransform;
						InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);

						if (!Cache.copiedComponents.Contains(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())) {

							//comp2 = DuplicateObject <UHierarchicalInstancedStaticMeshComponent>(InstanceHandles[i]->GetInstanceComponent(), Manager);
							comp2 = NewObject<UHierarchicalInstancedStaticMeshComponent>(Cache.HologramHelper);

							//comp2->ClearInstances();

							//FTransform InstanceTransform;
							//InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);

							//std::string  str = TCHAR_TO_UTF8(*InstanceHandles[i]->GetInstanceComponent()->GetName());

							//OutputDebugStringW(wideString);


							//InstanceCount.Add(comp2->GetInstanceCount());

							// both of these dont work 100% of the time?
							comp2->SetStaticMesh(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh());
							//comp2->SetStaticMesh(Instances[i].StaticMesh);

							FInstanceData instanceData = FInstanceData();

							instanceData.OverridenMaterials.Add(Material);

							comp2->OverrideMaterials = instanceData.OverridenMaterials;
							comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							comp2->SetGenerateOverlapEvents(false);
							comp2->SetMobility(InstanceHandles[i]->GetInstanceComponent()->Mobility);
							comp2->AttachToComponent(Cache.HologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
							comp2->SetRelativeTransform(FTransform::Identity);
							comp2->SetVisibility(true);
							comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
							comp2->RegisterComponent();

							//comp2->AddInstance(InstanceTransform, true);
							//comp2->AddInstanceWorldSpace(InstanceTransform);

							//copiedComponents.Add(comp2);
							//Cache.copiedComponents.Add(comp2);
							Cache.copiedComponents.Add(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh(), comp2);

						}

						UStaticMesh* staticmesh = InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh();

						//Cache.copiedComponents.Find(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh())

						UHierarchicalInstancedStaticMeshComponent* instance = *Cache.copiedComponents.Find(staticmesh);
						instance->AddInstance(InstanceTransform, true);

						InstanceHandles[i]->HideInstance(true);
					}

				}
				else {

					//if (handleHolder) {
						//FSMaterialHandle handle = **handleHolder;


					//}



				}
			}

			if (!initializeCacheSaved) {
				if (!UseDefaultOutline) {

					/*
					for (const auto& ActorComp : Cache.HologramHelper->GetComponentsByClass(UMeshComponent::StaticClass())) {
						UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
						if (MeshComponent) {
							int num = MeshComponent->GetNumMaterials();
							if (num) {
								for (int i = 0; i < num; i++) {
									MeshComponent->SetMaterial(i, Material);
								}
							}
						}
					}
					*/

				}
			}
			else {

				if (!Cache.UseDefaultOutline) {
					for (TPair<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*>& Pair : Cache.copiedComponents) {
						useNormalHologram = false;

						TArray<UMaterialInterface*> OverridenMaterials;
						OverridenMaterials.Add(Material);
						Pair.Value->OverrideMaterials = OverridenMaterials;
						Pair.Value->MarkRenderTransformDirty();

						/*
						for (const auto& ActorComp : Cache.HologramHelper->GetComponentsByClass(UMeshComponent::StaticClass())) {
							UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
							if (MeshComponent) {
								int num = MeshComponent->GetNumMaterials();
								if (num) {
									for (int i = 0; i < num; i++) {
										MeshComponent->SetMaterial(i, Material);
									}
								}
							}
						}
						*/
					}

				}

				/*
				for (const auto& ActorComp : Cache.HologramHelper->GetComponentsByClass(UMeshComponent::StaticClass())) {
					UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
					if (MeshComponent) {
						int num = MeshComponent->GetNumMaterials();
						if (num) {
							for (int i = 0; i < num; i++) {
								MeshComponent->SetMaterial(i, Material);
							}
						}
					}
				}
				*/
			}

		}


		bool isInstanced = true;






		if (Buildable.Buildable->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
			isInstanced = useNormalHologram = true;
		}
		else {
			if (!UseDefaultOutline) {
				for (const auto& ActorComp : Buildable.Buildable->K2_GetComponentsByClass(UMeshComponent::StaticClass())) {

					UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
					//
					if (MeshComponent) {

						FString Name = MeshComponent->GetName();
						if (!Buildable.Buildable->GetClass()->IsChildOf<AFGBuildableWire>()) {
							if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
						}

						if (!Name.Contains(TEXT("VisibilityMesh"))) {
							//isInstanced = false;
							//continue;
						}

						UFGColoredInstanceMeshProxy* MeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComponent);

						if (!initializeCacheSaved) {
							FSavedMaterialInterfaces Value2 = FSavedMaterialInterfaces();
							Value2.MaterialInterfaces = MeshComponent->GetMaterials();
							Cache.SavedMaterialInterfaces.Add(MeshComponent, Value2);
						}

						FSMeshMaterial* MeshMaterial = CacheExist ? nullptr : &Cache.MeshList.AddDefaulted_GetRef();

						/*
						if (!CacheExist) {
							TArray< UMaterialInterface*> interfaces = MeshComponent->GetMaterials();
							MeshMaterial->MaterialInterfaceList2 = interfaces;
						}
						*/

						int num = MeshComponent->GetNumMaterials();
						if (num) {
							for (int i = 0; i < num; i++) {
								MeshComponent->SetMaterial(i, Material);
							}

							//auto interfaces2Ptr = ActorInfo2.SavedMaterialInterfaces.Find(MeshComponent);
							//auto interfaces2 = interfaces2Ptr;

							// prototype one disable highlight as soon as it is set
							//this passed
							//FSMaterialHandle** comp = MaterialMapping2.Find(MeshComponent);
							//FSMaterialHandle* comp2 = *comp;
							//TArray < UMaterialInterface*> list = (*comp)->MaterialInterfaceList;
							//int num2 = MeshMaterial->MaterialInterfaceList2.Num();
							//for (int i = 0; i < num2; i++) {
								// this works
								//MeshComponent->SetMaterial(i, interfaces[i]);
								//this works
								//MeshComponent->SetMaterial(i, list[i]);
								// this works
								//MeshComponent->SetMaterial(i, MeshMaterial->MaterialInterfaceList2[i]);
								// doesn't work
								//MeshComponent->SetMaterial(i, interfaces2->MaterialInterfaces[i]);
							//}

							
							if (!CacheExist) {
								MeshMaterial->MeshComponent = MeshComponent;
								MeshMaterial->MeshProxy = MeshProxy;
								MeshMaterial->MaterialInterfaceList.Empty();
							}
							if (MeshProxy && MeshProxy->mInstanceManager != nullptr) {
								FSMaterialHandle** LocalCache = MaterialMapping.Find(MeshProxy);
								if (LocalCache) {
									RemoveInstance(MeshProxy, *LocalCache);
								}
								else {
									RemoveInstance(MeshProxy, MeshProxy->mInstanceHandle.ColorIndex);
								}
								AddInstance(MeshProxy, Material);
							}
							else {
								for (int i = 0; i < num; i++) {
									if (!CacheExist) {
										UMaterialInterface* Interface = MeshComponent->GetMaterial(i);
										MeshMaterial->MaterialInterfaceList.Add(Interface);

										UMaterialInstanceDynamic* Dynamic = Cast<UMaterialInstanceDynamic>(Interface);
										if (Dynamic) DynamicInstanceSet.Add(Dynamic);
									}
									//MeshComponent->SetMaterial(i, Material);
								}
							}
							
						}
					}
				}
			}
		}
		if (isInstanced && useNormalHologram) {
			//Buildable->ToggleInstanceVisibility(false);
			Cache.hologramVariation = true;

			AFGHologram* Hologram = nullptr;
			if (!initializeCacheSaved) {
				if (!UseDefaultOutline) {
					Cache.hologramVariation = true;
					UFSBuildableOperator* BuildableOperator = FSkyline->OperatorFactory->AcquireOperator(&Buildable);
					FTransform RelativeTransformVar;
					FSkyline->SplineHologramFactory->Load();
					Hologram = BuildableOperator->HologramCopy(RelativeTransformVar);
					Cache.Hologram = Hologram;
				}
			}
			else {
				//FSelectedActorInfo* handleHolder = SelectedMap.Find(Buildable);
				//if (handleHolder) {
				if (!Cache.UseDefaultOutline)
					//FSMaterialHandle handle = **handleHolder;
					if (Cache.Hologram == nullptr) {
						//handle.Hologram->Destroy();
						UFSBuildableOperator* BuildableOperator = FSkyline->OperatorFactory->AcquireOperator(&Buildable);
						FTransform RelativeTransformVar;
						FSkyline->SplineHologramFactory->Load();
						Hologram = BuildableOperator->HologramCopy(RelativeTransformVar);
						Cache.Hologram = Hologram;
					}
					else {
						Hologram = Cache.Hologram;
					}
				//Value->Hologram = Hologram;

				//Hologram = handle.Hologram;
				//Hologram = (*handleHolder)->Hologram;
			//}
			}

			if (Hologram != nullptr) {
				Hologram->SetActorHiddenInGame(false);
				Hologram->SetActorTransform(Buildable.Buildable->GetTransform());
				if (Material == SelectMaterial) {
					Hologram->SetPlacementMaterialState(EHologramMaterialState::HMS_OK);
				}
				if (Material == FocusMaterial) {
					Hologram->SetPlacementMaterialState(EHologramMaterialState::HMS_ERROR);
				}

				FTransform HologramTransform = Buildable.Buildable->GetTransform();
				//HologramTransform.SetScale3D(FVector(1.01, 1.01, 1.01));
				//HologramTransform.SetScale3D(FVector(1.005, 1.005, 1.005));
				//HologramTransform.SetScale3D(FVector(1.0025, 1.0025, 1.0025));
				//HologramTransform.SetScale3D(FVector(1.00125, 1.00125, 1.00125));
				HologramTransform.SetScale3D(FVector(1.000625, 1.000625, 1.000625));
				Hologram->SetActorTransform(HologramTransform);

				TInlineComponentArray<UMeshComponent*> MeshComps(Hologram);
				for (const auto& MeshComponent : MeshComps) {
					int num = MeshComponent->GetNumMaterials();
					if (num) {
						for (int i = 0; i < num; i++) {
							if (Material == FocusMaterial) {
								//MeshComponent->SetMaterial(i, Material);
							}
						}
					}
				}
			}

		}
	}


	/*
	if (!initializeCacheSaved) {
		SelectedMap.Add(Buildable, Value);
	}
	*/

	Cache.Init = true;
	//start deeper debugging here
	/*
	SML::Logging::info(*Buildable->GetFullName());
	for (UActorComponent* ActorComponent : Buildable->GetComponents()) {
		SML::Logging::info(TEXT("    "), *ActorComponent->GetFullName());
		UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComponent);
		if (MeshComponent) {
			int num = MeshComponent->GetNumMaterials();
			SML::Logging::info(TEXT("    "), num);
			for (int i = 0; i < num; i++) {
				SML::Logging::info(TEXT("        "), *MeshComponent->GetMaterial(i)->GetFullName());
				UMaterialInstanceDynamic* Dynamic = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(i));
				if (Dynamic) {
					SML::Logging::info(TEXT("        "), *Dynamic->GetMaterial()->GetFullName());
					
					TArray<FMaterialParameterInfo> OutParameterInfo;
					TArray<FGuid> OutParameterIds;
					Dynamic->GetAllScalarParameterInfo(OutParameterInfo, OutParameterIds);
					for (FMaterialParameterInfo& Info : OutParameterInfo) {
						float Value;
						Dynamic->GetScalarParameterValue(Info, Value);
						SML::Logging::info(TEXT("            "), *Info.Name.ToString(), TEXT(" = "), Value);
					}
					Dynamic->GetAllVectorParameterInfo(OutParameterInfo, OutParameterIds);
					for (FMaterialParameterInfo& Info : OutParameterInfo) {
						FLinearColor Value;
						Dynamic->GetVectorParameterValue(Info, Value);
						SML::Logging::info(TEXT("            "), *Info.Name.ToString(), TEXT(" = "), *Value.ToString());
					}
					Dynamic->GetAllTextureParameterInfo(OutParameterInfo, OutParameterIds);
					for (FMaterialParameterInfo& Info : OutParameterInfo) {
						UTexture* Value;
						Dynamic->GetTextureParameterValue(Info, Value);
						SML::Logging::info(TEXT("            "), *Info.Name.ToString(), TEXT(" = "), *Value->GetFullName());
					}
				}
			}
		}
	}*/
	//end deeper debugging here
}

void UFSSelection::EnableHightLightFocus(FSBuildable Buildable)
{
	if (Buildable == BuildableEmpty) return;
	if (CurrentFocus != Buildable) {
		if (CurrentFocus != BuildableEmpty) DisableHightLightFocus();
		CurrentFocus = Buildable;
	}
	EnableHightLight(Buildable, FocusMaterial);
}

void UFSSelection::DisableHightLightFocus()
{
	if (CurrentFocus == BuildableEmpty) return;
	if (Design->IsElementSelected(CurrentFocus)) {
		if (CurrentFocus != Design->Anchor) {
			EnableHightLight(CurrentFocus, SelectMaterial);
		}
	}
	else DisableHightLight(CurrentFocus);
	CurrentFocus = FSBuildable();
}

void UFSSelection::DisableHightLight(FSBuildable Buildable)
{
	FSActorMaterial* Cache = ActorMaterialCache.Find(Buildable);
	AFSkyline* FSkylin = AFSkyline::Get(this);

	if (!Cache) return;

	FString FullPathName = Buildable.Buildable->GetPathName();

	//if (FullPathName.Contains("Build_AssemblerMk1") || FullPathName.Contains("Build_ManufacturerMk1") || FullPathName.Contains("Build_OilRefinery") || FullPathName.Contains("Build_HadronCollider")) {
	//}
	//else {

	//FSelectedActorInfo* handleHolder = SelectedMap.Find(Buildable);
	//if (handleHolder) {
		if (Cache->UseDefaultOutline) {

			// TODO NEED TO FIX THIS FOR DEFAULT OUTLINE LOGIC
			//FSkylin->FSCtrl->GetPlayer()->GetOutline()->HideActorOutline(Buildable);

			if (Cache->HologramHelper) {
				Cache->HologramHelper->Destroy();
				Cache->HologramHelper = nullptr;
			}

			for (TPair<UStaticMesh*, UInstancedStaticMeshComponent*>& Pair : Cache->OutlineProxyData) {
				Pair.Value->ClearInstances();
				Pair.Value->UnregisterComponent();
				Pair.Value->DestroyComponent();
				Pair.Value->FixGarbageOrPendingKillInternalObjectFlags(EInternalObjectFlags::Garbage);
				Pair.Value->MarkAsGarbage();
				//Pair.Key->MarkAsGarbage();
				//Pair.Key = nullptr;
				Pair.Value = nullptr;
			}
			Cache->OutlineProxyData.Empty();

		} else {
			if (Cache->HologramHelper) {
				Cache->HologramHelper->Destroy();
				Cache->HologramHelper = nullptr;
				for (TPair<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*>& Pair : Cache->copiedComponents) {
					Pair.Value->ClearInstances();
				}

				if (!Buildable.Buildable) {
					//Buildable.Handle.UnHideInstance(true);
					//Buildable.Handle.HideInstance(false);

					Cache->Handle.UnHideInstance(true);
				}

				if (Buildable.Buildable) {
					InstanceHandles = Buildable.Buildable->mInstanceHandles;
					for (int i = 0; i < InstanceHandles.Num(); i++) {
						InstanceHandles[i]->UnHideInstance(true);
					}
				}

				// also regular meshes can exist with the same buildable with instancing as well so we need to do hologram logic for those meshes as well
				if (Buildable.Buildable) {
					FSActorMaterial& handle = *Cache;
					HideHologram(Buildable.Buildable, handle);
				}

			} else {

				FSActorMaterial& handle = *Cache;
				//if (!handle.hologramVariation) {
				if (Buildable.Buildable) {
					HideHologram(Buildable.Buildable, handle);

					if (Buildable.Buildable->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
						AFGBuildableWidgetSign* BuildableSign = Cast<AFGBuildableWidgetSign>(Buildable.Buildable);
						BuildableSign->PasteSettings_Implementation(BuildableSign->CopySettings_Implementation());
					}
				}
				//}
				if (handle.hologramVariation) {
					//Buildable->ToggleInstanceVisibility(true);
					if (handle.Hologram) {
						handle.Hologram->SetDisabled(true);
						handle.Hologram->Destroy();
						handle.Hologram = nullptr;
						Cache->Hologram = nullptr;
						//FSMaterialHandle* Value = new FSMaterialHandle();
						//Value->Hologram = handle.Hologram;
						//SelectedMap.Add(Buildable, Value);
					}
				}
			}
		}

	//}

	//SelectedMap.Remove(Buildable);

	//ActorList.Remove(Buildable);

	// partial work need another solution
	/*
	TInlineComponentArray<UMeshComponent*> MeshComps(Buildable);
	for (const auto& MeshComponent : MeshComps) {
		//
		if (MeshComponent) {
			FString Name = MeshComponent->GetName();
			if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
			FSMaterialHandle** comp = MaterialMapping2.Find(MeshComponent);
			TArray < UMaterialInterface*> list = (*comp)->MaterialInterfaceList;
			int num = MeshComponent->GetNumMaterials();
			for (int i = 0; i < num; i++) {
				MeshComponent->SetMaterial(i, list[i]);
			}
			//MaterialMapping2.Remove(MeshComponent);
		}
	}
	*/

	/*
	TInlineComponentArray<UMeshComponent*> MeshComps(Buildable);
	for (const auto& MeshComponent : MeshComps) {
		//
		if (MeshComponent) {
			FString Name = MeshComponent->GetName();
			if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
			int num = MeshComponent->GetNumMaterials();
			for (int i = 0; i < num; i++) {
				MeshComponent->SetMaterial(i, list[i]);
			}
			//MaterialMapping2.Remove(MeshComponent);
		}
	}
	*/

	// partial work need another solution
	/*
	TInlineComponentArray<UMeshComponent*> MeshComps2(Buildable);
	for (const auto& MeshComponent : MeshComps2) {
		//
		if (MeshComponent) {
			FString Name = MeshComponent->GetName();
			if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
			MaterialMapping2.Remove(MeshComponent);
		}
	}
	*/

	/*
	FSelectedActorInfo* ActorInfo = SelectedMap.Find(Buildable);
	for (auto& ActorComp : Buildable->GetComponentsByClass(UMeshComponent::StaticClass())) {
		auto MeshComp = Cast<UMeshComponent>(ActorComp);
		auto SavedMaterialInterfacePtr = ActorInfo->SavedMaterialInterfaces.Find(MeshComp);
		if (SavedMaterialInterfacePtr) {
			if (SavedMaterialInterfacePtr) {
				auto SavedMaterialInterface = *SavedMaterialInterfacePtr;
				int Mats = SavedMaterialInterface.MaterialInterfaces.Num();
				for (int i = 0; i < Mats; i++) {
					MeshComp->SetMaterial(i, SavedMaterialInterface.MaterialInterfaces[i]);
				}
			}
		}
	}
	*/
	/*
	for (FSMeshMaterial& MeshMaterial : Cache->MeshList) {
		UMeshComponent* MeshComponent = MeshMaterial.MeshComponent.Get();
		int num = MeshComponent->GetNumMaterials();
		for (int i = 0; i < num; i++) {
			MeshComponent->SetMaterial(i, MeshMaterial.MaterialInterfaceList[i]);
		}
	}
	*/

	
		for (FSMeshMaterial& MeshMaterial : Cache->MeshList) {
			UMeshComponent* MeshComponent = MeshMaterial.MeshComponent.Get();
			UFGColoredInstanceMeshProxy* MeshProxy = MeshMaterial.MeshProxy;

			// doesn't work
			//int num = MeshComponent->GetNumMaterials();
			//for (int i = 0; i < num; i++) {
				//MeshComponent->SetMaterial(i, MeshMaterial.MaterialInterfaceList2[i]);
			//}

			if (MeshProxy && MeshProxy->mInstanceManager != nullptr) {

				FSMaterialHandle** LocalCache = MaterialMapping.Find(MeshProxy);
				if (LocalCache) {
					RemoveInstance(MeshProxy, *LocalCache);
					if (MeshComponent && Cache->Buildable.Get()) {
						//SML::Logging::info(MeshProxy->mInstanceHandle.colorIndex);
						if (MeshProxy->mInstanceHandle.ColorIndex < BUILDABLE_COLORS_MAX_SLOTS) {
							int32 intNumberTest = int32(MeshProxy->mInstanceManager->mCachedNumCustomData);
							MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, MeshProxy->mInstanceHandle.ColorIndex, int32(MeshProxy->mInstanceManager->mCachedNumCustomData));
						}
						else {
							// this attempt didnt exactly work
							//MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, 0, int32(MeshProxy->mInstanceManager->mCachedNumCustomData));
							// this fix worked, remove the entire check for color max slots as it might not be needed in u6 maybe?
							MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, MeshProxy->mInstanceHandle.ColorIndex, int32(MeshProxy->mInstanceManager->mCachedNumCustomData));
						}
					}
				}
			}

		}
		
		//FSBuildable Buildable;

		ActorMaterialCache.Remove(Buildable);
		
	//}
	if (CurrentFocus == Buildable) CurrentFocus = FSBuildable();
}

void UFSSelection::DisableAll()
{
	AFSkyline* FSkylin = AFSkyline::Get(this);

	for (TPair<FSBuildable, FSActorMaterial>& pair : ActorMaterialCache){
		FSBuildable actor = pair.Key;
		FSActorMaterial handle = pair.Value;

		bool exists = true;
		/* As some buildables are pure abstracts now this test isnt very useful
		*  Maybe refactor in anythere way?
		int num = ActorList.Num();
		AActor* actor2 = nullptr;
		bool exists = true;
		for (int i = 0; i < num; i++) {
			if (ActorList[i] != nullptr) {
				if (actor == ActorList[i]) {
					exists = true;
					break;
				}
			}
		}
		*/

		//AActor* actor2 = ActorList.Find(actor);
		if (exists) {
			if (!pair.Value.UseDefaultOutline) {
				if (pair.Value.HologramHelper) {
					pair.Value.HologramHelper->Destroy();
					pair.Value.HologramHelper = nullptr;
					for (TPair<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*>& Pair : pair.Value.copiedComponents) {
						Pair.Value->ClearInstances();
					}

					if (!actor.Buildable) {
						//Buildable.Handle.UnHideInstance(true);
						//Buildable.Handle.HideInstance(false);

						handle.Handle.UnHideInstance(true);
						//actor.Handle.UnHideInstance(true);
					}

					if (actor.Buildable) {
						InstanceHandles = Cast<AFGBuildable>(actor.Buildable)->mInstanceHandles;
						for (int i = 0; i < InstanceHandles.Num(); i++) {
							InstanceHandles[i]->UnHideInstance(true);
						}
					}

					// also regular meshes can exist with the same buildable with instancing as well so we need to do hologram logic for those meshes as well
					if (actor.Buildable) {
						HideHologram(actor.Buildable, handle);
					}

					continue;
				}
			}

			if (pair.Value.UseDefaultOutline) {

				// TODO WE NEED TO FIX FOR DEFAULT OUTLINES
				//FSkylin->FSCtrl->GetPlayer()->GetOutline()->HideActorOutline(actor);

				/*

				for (const auto& ActorComp : actor->GetComponentsByClass(UMeshComponent::StaticClass())) {
					UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);

					if (MeshComponent) {

						FString Name = MeshComponent->GetName();
						if (!actor->GetClass()->IsChildOf<AFGBuildableWire>()) {
							if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
						}

						UFGBlueprintFunctionLibrary::HideOutline(MeshComponent);

					}
				}
				*/

				
				if (pair.Value.HologramHelper) {
					pair.Value.HologramHelper->Destroy();
					pair.Value.HologramHelper = nullptr;
				}

				for (TPair<UStaticMesh*, UInstancedStaticMeshComponent*>& Pair : pair.Value.OutlineProxyData) {

					Pair.Value->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

					UFGBlueprintFunctionLibrary::ShowOutline(Pair.Value, EOutlineColor::OC_NONE);

					Pair.Value->ClearInstances();
					Pair.Value->UnregisterComponent();
					Pair.Value->DestroyComponent();
					Pair.Value->FixGarbageOrPendingKillInternalObjectFlags(EInternalObjectFlags::Garbage);
					Pair.Value->MarkAsGarbage();
					//Pair.Key->MarkAsGarbage();
					//Pair.Key = nullptr;
					Pair.Value = nullptr;
				}
				pair.Value.OutlineProxyData.Empty();
				

			} else {
				if (actor.Buildable) {
					HideHologram(actor.Buildable, handle);
					AFGBuildable* buildableActor = Cast<AFGBuildable>(actor.Buildable);
					//buildableActor->ToggleInstanceVisibility(true);
					if (buildableActor->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
						AFGBuildableWidgetSign* BuildableSign = Cast<AFGBuildableWidgetSign>(buildableActor);
						BuildableSign->PasteSettings_Implementation(BuildableSign->CopySettings_Implementation());
					}
				}
			}
		}
		if (handle.hologramVariation) {
			//Buildable->ToggleInstanceVisibility(true);
			if (handle.Hologram) {
				handle.Hologram->SetDisabled(true);
				handle.Hologram->Destroy();
				//FSMaterialHandle* Value = new FSMaterialHandle();
				//FSMaterialHandle* handle = pair.Value;
				//pair.Value = nullptr;
				pair.Value.Hologram = nullptr;
				//SelectedMap.Add(actor, Value);
			}
		}
	}
	for (TPair<FSBuildable, FSActorMaterial >& Pair : ActorMaterialCache)
		//AFGBuildable* buildable = Pair.Value.Buildable.Get();
		for (FSMeshMaterial& MeshMaterial : Pair.Value.MeshList) {
			UMeshComponent* MeshComponent = MeshMaterial.MeshComponent.Get();
			UFGColoredInstanceMeshProxy* MeshProxy = MeshMaterial.MeshProxy;
			if (MeshProxy && MeshProxy->mInstanceManager != nullptr) {
				FSMaterialHandle** LocalCache = MaterialMapping.Find(MeshProxy);
				if (LocalCache) {
					RemoveInstance(MeshProxy, *LocalCache);

					AFGBuildable* buildable = Pair.Value.Buildable.Get();
					if (MeshComponent && Pair.Value.Buildable.Get()) {
						if (MeshProxy->mInstanceHandle.ColorIndex < BUILDABLE_COLORS_MAX_SLOTS) {
							//MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, MeshProxy->mInstanceHandle.ColorIndex);
							MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, MeshProxy->mInstanceHandle.ColorIndex, int32(MeshProxy->mInstanceManager->mCachedNumCustomData));
						}else {
							// Call Our custom AddInstance here, this doesn't call the other AddInstance because who knows why
							//MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, 0);
							MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, MeshProxy->mInstanceHandle.ColorIndex, int32(MeshProxy->mInstanceManager->mCachedNumCustomData));
						}
					}
				}
			}

			// legacy code dont use
			/*
			else if (MeshComponent && Pair.Value.Buildable.Get()) {
				for (int i = 0; i < MeshMaterial.MaterialInterfaceList.Num(); i++)
					MeshComponent->SetMaterial(i, MeshMaterial.MaterialInterfaceList[i]);
			}
			*/
		}
}

FSISMNode* UFSSelection::GetISM(UFGColoredInstanceMeshProxy* MeshProxy, UMaterialInterface* Material)
{
	FSISMNode** NodePtr = ISMMapping.Find(TPair<UFGColoredInstanceManager*, UMaterialInterface* >(MeshProxy->mInstanceManager, Material));
	if (NodePtr) return *NodePtr;

	FSISMNode* Node = new FSISMNode();
	ISMMapping.Add(TPair<UFGColoredInstanceManager*, UMaterialInterface* >(MeshProxy->mInstanceManager, Material), Node);

	UHierarchicalInstancedStaticMeshComponent* HISMComponent = MeshProxy->mInstanceManager->mInstanceComponent;
	Node->ISMComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(HISMComponent->GetAttachmentRootActor());
	//Node->ISMComponent->AttachTo(HISMComponent->GetAttachmentRootActor()->GetRootComponent());
	Node->ISMComponent->SetStaticMesh(HISMComponent->GetStaticMesh());
	Node->ISMComponent->BodyInstance = HISMComponent->BodyInstance;
	Node->ISMComponent->MinLOD = 2;
	Node->ISMComponent->bOverrideMinLOD = true;
	Node->ISMComponent->RegisterComponent();

	int Num = Node->ISMComponent->GetNumMaterials();
	for (int i = 0; i < Num; i++) Node->ISMComponent->SetMaterial(i, Material);
	
	Node->Handles.Empty();
	GCFlag.Add(Node->ISMComponent);
	return Node;
}

void UFSSelection::AddInstance(UFGColoredInstanceMeshProxy* MeshProxy, UMaterialInterface* Material)
{
	if (MeshProxy->mInstanceManager != nullptr) {
		FSISMNode* NodePtr = GetISM(MeshProxy, Material);
		NodePtr->ISMComponent->AddInstance(MeshProxy->GetComponentTransform());
		FSMaterialHandle* Value = new FSMaterialHandle();
		Value->Handle = NodePtr->Handles.Add(Value);
		Value->Material = Material;
		MaterialMapping.Add(MeshProxy, Value);
	}
}

void UFSSelection::RemoveInstance(UFGColoredInstanceMeshProxy* MeshProxy, FSMaterialHandle* Node)
{
	FSISMNode* NodePtr = GetISM(MeshProxy, Node->Material);
	int Index = Node->Handle;
	NodePtr->ISMComponent->RemoveInstance(Index);
	NodePtr->Handles.RemoveAtSwap(Index);
	if (Index < NodePtr->Handles.Num()) NodePtr->Handles[Index]->Handle = Index;
	MaterialMapping.Remove(MeshProxy);
	delete Node;
}

void UFSSelection::AddInstance(UFGColoredInstanceMeshProxy* MeshProxy, uint8 Slot)
{
	UHierarchicalInstancedStaticMeshComponent* HISMComponent = MeshProxy->mInstanceManager->mInstanceComponent;
	HISMComponent->AddInstance(MeshProxy->GetComponentTransform());
	TArray <UFGColoredInstanceManager::FInstanceHandle*>& NewHandlesArray = MeshProxy->mInstanceManager->mHandles;
	MeshProxy->mInstanceHandle.HandleID = NewHandlesArray.Add(&MeshProxy->mInstanceHandle);
}

void UFSSelection::RemoveInstance(UFGColoredInstanceMeshProxy* MeshProxy, uint8 Slot)
{
	if (MeshProxy->mInstanceManager != nullptr) {
		UHierarchicalInstancedStaticMeshComponent* HISMComponent = MeshProxy->mInstanceManager->mInstanceComponent;
		int32 Index = MeshProxy->mInstanceHandle.HandleID;
		if (Index < 0) return;

		MeshProxy->mInstanceHandle.HandleID = INDEX_NONE;
		HISMComponent->RemoveInstance(Index);
		TArray <UFGColoredInstanceManager::FInstanceHandle*>& HandlesArray = MeshProxy->mInstanceManager->mHandles;
		HandlesArray.RemoveAtSwap(Index);
		if (Index >= 0 && Index < HandlesArray.Num()) HandlesArray[Index]->HandleID = Index;
	}
}

void UFSSelection::SpawnInitialAreaBox(AFGBuildable* Buildable) {

	if (Cube != nullptr) {
		if (Cube->IsValidLowLevel()) {
			Cube->Destroy();
			Cube = nullptr;
		}
	}

	AFSkyline* FSkyline = AFSkyline::Get(this);

	FActorSpawnParameters* parameters = new FActorSpawnParameters();

	FTransform cubeTransform = FTransform();
	FTransform BuildableTransform = Buildable->GetTransform();
	FVector BuildablePoint = BuildableTransform.GetTranslation();
	cubeTransform.SetTranslation(BuildablePoint);

	Cube = (AMyActor*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AMyActor::StaticClass(), cubeTransform, *parameters);
	FBoxSphereBounds baseBounds = Cube->GetRootComponent()->Bounds;
	Cube->SetActorScale3D(FVector(8, 8, 8));

	Cube->SetActorEnableCollision(false);

	TInlineComponentArray<UMeshComponent*> MeshComps(Cube);
	for (const auto& MeshComponent : MeshComps) {
		int num = MeshComponent->GetNumMaterials();
		if (num) {
			for (int i = 0; i < num; i++) {
				MeshComponent->SetMaterial(i, CubeMaterial);
			}
		}
	}

}

void UFSSelection::ChangeAreaBox(float Size) {
	if (Cube != nullptr) {
		//Cube->SetActorScale3D(FVector(16, 16, 16));
		float num1 = Size;
		float num2 = num1;
		if (num1 > 8) {
			num1 -= 8;
			num2 += num1;
		}
		Cube->SetActorScale3D(FVector(num2, num2, num2));
	}
}

void UFSSelection::SelectBuildablesInAreaBox() {

	if (Cube != nullptr) {

		AFSkyline* FSkyline = AFSkyline::Get(this);
		this->Design = FSkyline->FSCtrl->Design;

		FBoxSphereBounds bounds1 = Cube->GetRootComponent()->Bounds;

		//const float Range = CVarDisplayLightWeightInstanceDebugRange.GetValueOnAnyThread();

		TArray<UHierarchicalInstancedStaticMeshComponent*> filterList;

		for (TObjectIterator<AFGBuildable> Worker; Worker; ++Worker) {
			AFGBuildable* buildablePtr = *Worker;

			if (!IsValid(buildablePtr)) {
				continue;
			}

			AFGBuildablePipelineFlowIndicator* Indicator = Cast<AFGBuildablePipelineFlowIndicator>(buildablePtr);

			if (Indicator) {
				//don't want to select any indicators that are already owned as they will be spawned by the pipe instead.
				if (Indicator->mOwningPipeline) {
					continue;
				}
			}

			InstanceHandles = buildablePtr->mInstanceHandles;

			bool foundInstanceCollision = false;

			if (InstanceHandles.Num() > 0) {
				for (int i = 0; i < InstanceHandles.Num(); i++) {

					bool foundCollision = false;

					TArray<int32> instances;
					instances = InstanceHandles[i]->GetInstanceComponent()->GetInstancesOverlappingBox(bounds1.GetBox(), true);


					if (instances.Num() > 0) {


						for (int q = 0; q < instances.Num(); q++) {
							if (InstanceHandles[i]->GetHandleID() == instances[q]) {

								//filterList.Add(InstanceHandles[i]->GetInstanceComponent());

								//Design->AddElement(buildablePtr);
								//Design->BuildableMark.Add(buildablePtr, 1);


								FSBuildable Buildable;
								Buildable.Buildable = buildablePtr;

								Design->AddElement(Buildable);
								Design->BuildableMark.Add(Buildable, 1);
								EnableHightLight(Buildable, SelectMaterial);
								foundCollision = true;
								break;
							}
						}
					}

					if (foundCollision) {
						foundInstanceCollision = true;
						break;
					}

				}
			}

			if (InstanceHandles.Num() > 0 && foundInstanceCollision) {
				for (int i = 0; i < InstanceHandles.Num(); i++) {
					filterList.Add(InstanceHandles[i]->GetInstanceComponent());
				}
			}

			else {
				FBoxSphereBounds bounds2;
				if (buildablePtr->GetRootComponent() == nullptr) {
					continue;
				}
				else {
					bounds2 = buildablePtr->GetRootComponent()->Bounds;
				}
				bool Overlap = FBoxSphereBounds::BoxesIntersect(bounds1, bounds2);
				if (Overlap) {
					//Design->AddElement(buildablePtr);
					//Design->BuildableMark.Add(buildablePtr, 1);

					FSBuildable Buildable;
					//Buildable.Buildable = buildablePtr;
					Buildable.Buildable = buildablePtr;

					Design->AddElement(Buildable);
					Design->BuildableMark.Add(Buildable, 1);
					EnableHightLight(Buildable, SelectMaterial);
				}else {

					TArray<UStaticMeshComponent*> MeshComponents;
					buildablePtr->GetComponents<UStaticMeshComponent>(MeshComponents);

					for (UStaticMeshComponent* MeshComponent : MeshComponents) {

						FBox MeshBounds = MeshComponent->CalcBounds(MeshComponent->GetComponentTransform()).GetBox();

						// Get the bounding box of the cube
						FBox CubeBounds = Cube->GetRootComponent()->Bounds.GetBox();

						// Check for intersection between the bounding boxes
						bool bIntersects = MeshBounds.Intersect(CubeBounds);

						if (bIntersects) {

							//Design->AddElement(buildablePtr);
							//Design->BuildableMark.Add(buildablePtr, 1);

							FSBuildable Buildable;
							Buildable.Buildable = buildablePtr;

							//Select(Buildable);

							Design->AddElement(Buildable);
							Design->BuildableMark.Add(Buildable, 1);
							EnableHightLight(Buildable, SelectMaterial);

							break;

						}

					}

				}
			}
		}

		

		AAbstractInstanceManager* Manager = AAbstractInstanceManager::GetInstanceManager(FSkyline->World);

		for (const TTuple< FName, FInstanceComponentData >& InstanceData : Manager->InstanceMap)
		{
			FString Name = InstanceData.Key.ToString();
			for (const auto& Handle : InstanceData.Value.InstanceHandles)
			{

				bool foundCollision = false;

				TArray<int32> instances;
				instances = Handle->GetInstanceComponent()->GetInstancesOverlappingBox(bounds1.GetBox(), true);

				bool foundFilter = false;

				for (int j = 0; j < filterList.Num(); j++) {
					UHierarchicalInstancedStaticMeshComponent* filterMesh = filterList[j];
					UHierarchicalInstancedStaticMeshComponent* HandleMesh = Handle->GetInstanceComponent();
					if (filterMesh == HandleMesh) {
						foundFilter = true;
						break;
					}
				}

				if (foundFilter) {
					continue;
				}

				if (instances.Num() > 0) {

					for (int q = 0; q < instances.Num(); q++) {
						if (Handle->GetHandleID() == instances[q]) {

							//Handle->HideInstance(true);

							FLightweightBuildableInstanceRef buildableRef;
							AFGLightweightBuildableSubsystem::ResolveLightweightInstance(*Handle, buildableRef);
							TSubclassOf< AFGBuildable > buildableClass = buildableRef.GetBuildableClass();
							FRuntimeBuildableInstanceData* runtimeData = const_cast<FRuntimeBuildableInstanceData*>(buildableRef.ResolveBuildableInstanceData());

							//FRuntimeBuildableInstanceData data;
							//UFGLightweightBuildableBlueprintLibrary::ResolveLightweightCustomizationData(buildableRef, data);


							FRuntimeBuildableInstanceData RuntimeData;

							if (runtimeData) {
								RuntimeData.Transform = runtimeData->Transform;
								RuntimeData.CustomizationData = runtimeData->CustomizationData;
								RuntimeData.BuiltWithRecipe = runtimeData->BuiltWithRecipe;
								RuntimeData.BlueprintProxy = runtimeData->BlueprintProxy;
								RuntimeData.Handles = runtimeData->Handles;

								FSBuildable Buildable;
								Buildable.Abstract = true;
								Buildable.Handle = *Handle;
								Buildable.RuntimeData = RuntimeData;
								Buildable.BuildableClass = buildableClass;
								Buildable.Transform = RuntimeData.Transform;
								//Buildable.Buildable = Building;

								//Select(Buildable);

								Design->AddElement(Buildable);
								Design->BuildableMark.Add(Buildable, 1);
								EnableHightLight(Buildable, SelectMaterial);

							}

							//Design->AddElement(Buildable);
							//Design->BuildableMark.Add(Buildable, 1);
							//EnableHightLight(Buildable, SelectMaterial);

							foundCollision = true;
							//break;
						}
					}

				}
				//if (foundCollision) break;

				//Handle->HideInstance(true);

				int32 HandleId = Handle->HandleID;
				//FTransform T;
				//InstanceData.Value.InstancedStaticMeshComponent->GetInstanceTransform(HandleId, T, true/* already in world space.*/);
			}
		}

		/*
		for (TObjectIterator<AFGBuildable> Worker; Worker; ++Worker) {
			AFGBuildable* buildablePtr = *Worker;

			InstanceHandles = buildablePtr->mInstanceHandles;

			if (InstanceHandles.Num() > 0) {
				for (int i = 0; i < InstanceHandles.Num(); i++) {

					bool foundCollision = false;

					TArray<int32> instances;
					instances = InstanceHandles[i]->GetInstanceComponent()->GetInstancesOverlappingBox(bounds1.GetBox(), true);


					if (instances.Num() > 0) {


						for (int q = 0; q < instances.Num(); q++) {
							if (InstanceHandles[i]->GetHandleID() == instances[q]) {
								Design->AddElement(buildablePtr);
								Design->BuildableMark.Add(buildablePtr, 1);


								FSBuildable Buildable;
								Buildable.Buildable = buildablePtr;
								EnableHightLight(Buildable, SelectMaterial);
								foundCollision = true;
								break;
							}
						}
					}

					if (foundCollision) break;

				}
			}
			else {
				FBoxSphereBounds bounds2 = buildablePtr->GetRootComponent()->Bounds;
				bool Overlap = FBoxSphereBounds::BoxesIntersect(bounds1, bounds2);
				if (Overlap) {
					Design->AddElement(buildablePtr);
					Design->BuildableMark.Add(buildablePtr, 1);

					FSBuildable Buildable;
					Buildable.Buildable = buildablePtr;
					EnableHightLight(Buildable, SelectMaterial);
				}else {

					TArray<UStaticMeshComponent*> MeshComponents;
					buildablePtr->GetComponents<UStaticMeshComponent>(MeshComponents);

					for (UStaticMeshComponent* MeshComponent : MeshComponents) {

						FBox MeshBounds = MeshComponent->CalcBounds(MeshComponent->GetComponentTransform()).GetBox();

						// Get the bounding box of the cube
						FBox CubeBounds = Cube->GetRootComponent()->Bounds.GetBox();

						// Check for intersection between the bounding boxes
						bool bIntersects = MeshBounds.Intersect(CubeBounds);

						if (bIntersects) {

							Design->AddElement(buildablePtr);
							Design->BuildableMark.Add(buildablePtr, 1);

							FSBuildable Buildable;
							Buildable.Buildable = buildablePtr;
							EnableHightLight(Buildable, SelectMaterial);

							break;

						}

					}

				}
			}
		}

		*/

		
	}

}
void UFSSelection::UpdateHighlightedActors() {

	// TODO DO WE EVEN NEED THIS ANYMORE?
	/*
	AFSkyline* FSkylin = AFSkyline::Get(this);
	UFGBuildGunStateDismantle* dismantleState = FSkylin->FSCtrl->FGBuildGun->mDismantleStateClass.GetDefaultObject();
	dismantleState->UpdateHighlightedActors();
	*/
}