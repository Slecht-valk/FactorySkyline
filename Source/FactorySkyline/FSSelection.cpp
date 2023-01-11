// ILikeBanas


#include "FSSelection.h"
#include "FSDesign.h"
#include "FSkyline.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableFoundation.h"
#include "FSBuildableService.h"
#include "FGFactorySettings.h"
#include "FGProductionIndicatorInstanceComponent.h"
#include "Components/ProxyInstancedStaticMeshComponent.h"

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

#include "Buildables/FGBuildableBlueprintDesigner.h"

#include <iostream>
#include <string>

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

void UFSSelection::Select(AFGBuildable* Buildable)
{
	if (Design->IsElementSelected(Buildable)) {
		Design->RemoveElement(Buildable);
		Design->BuildableMark.Add(Buildable, 0);

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

		DisableHightLight(Buildable);
		if (Buildable->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
			AFGBuildableWidgetSign* BuildableSign = Cast<AFGBuildableWidgetSign>(Buildable);
			BuildableSign->PasteSettings_Implementation(BuildableSign->CopySettings_Implementation());
		}
		
		if (Buildable->GetName().Contains("Build_PowerPole")) {
			AFGBuildablePowerPole* PowerPole = Cast<AFGBuildablePowerPole>(Buildable);
			if (!PowerPole) return;
			UFGPowerConnectionComponent* ConnectionComponent = PowerPole->GetPowerConnection(0);
			if (!ConnectionComponent) return;

			TArray<AFGBuildableWire*> ConnectionWires;
			ConnectionComponent->GetWires(ConnectionWires);

			for (AFGBuildableWire* TargetWire : ConnectionWires) {
				AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
				Design->RemoveElement(Wire);
				Design->BuildableMark.Add(Wire, 0);
				DisableHightLight(Wire);
			}
		}

		if (Buildable->GetName().Contains("Build_LightsControlPanel") || Cast<AFGBuildableCircuitSwitch>(Buildable)) {
			//AFGBuildableLightsControlPanel* lightControl = Cast<AFGBuildableLightsControlPanel>(Buildable);
			AFGBuildableCircuitBridge* bridge = Cast<AFGBuildableCircuitBridge>(Buildable);
			TArray<UFGCircuitConnectionComponent*> connections = bridge->mConnections;
			
			for (int i = 0; i < connections.Num(); i++) {
				UFGCircuitConnectionComponent* connection = connections[i];
				TArray<AFGBuildableWire*> ConnectionWires;
				connection->GetWires(ConnectionWires);

				for (AFGBuildableWire* TargetWire : ConnectionWires) {
					AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
					Design->RemoveElement(Wire);
					Design->BuildableMark.Add(Wire, 0);
					DisableHightLight(Wire);
				}
			}
			
		}

		if (Cast<AFGBuildableLightSource>(Buildable) || Cast<AFGBuildablePowerStorage>(Buildable)) {
			TArray<UActorComponent*> ReplicatedComponents = Buildable->ReplicatedComponents;
			for (int i = 0; i < ReplicatedComponents.Num(); i++) {
				if (Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i])) {
					UFGPowerConnectionComponent* ConnectionComponent;
					ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i]);
					if (!ConnectionComponent) return;

					TArray<AFGBuildableWire*> ConnectionWires;
					ConnectionComponent->GetWires(ConnectionWires);

					for (AFGBuildableWire* TargetWire : ConnectionWires) {
						AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
						Design->RemoveElement(Wire);
						Design->BuildableMark.Add(Wire, 0);
						DisableHightLight(Wire);
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

		if (Buildable->GetName().Contains("Build_ConveyorLiftMk")) {
			if (Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0]) {
				Design->RemoveElement(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0]);
				Design->BuildableMark.Add(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0], 0);
				DisableHightLight(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0]);
			}
			if (Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1]) {
				Design->RemoveElement(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1]);
				Design->BuildableMark.Add(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1], 0);
				DisableHightLight(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1]);
			}
		}

		//SML::Logging::info(TEXT("Remove "), *Buildable->GetName());
	}
	else {
	    //Buildable->ToggleInstanceVisibility(false);
		bool foundCustomSelection = false;
		// see if we are selecting a passThrough
		if (Buildable->GetName().Contains("Build_FoundationPassthrough_Lift")) {
			// for passThroughs we need to determine if they have connections to them because we use the lift operators to make them instead
			bool attachedLifts = Cast<AFGBuildablePassthrough>(Buildable)->HasAnyAttachedBuildings();
			if (attachedLifts) {
				//foundCustomSelection = true;
			}
		}

		if (Buildable->GetName().Contains("Build_PowerPole")) {
			foundCustomSelection = true;

			Design->AddElement(Buildable);
			Design->BuildableMark.Add(Buildable, 1);
			EnableHightLight(Buildable, SelectMaterial);

			AFGBuildablePowerPole* PowerPole = Cast<AFGBuildablePowerPole>(Buildable);
			if (!PowerPole) return;
			UFGPowerConnectionComponent* ConnectionComponent = PowerPole->GetPowerConnection(0);
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

		if (Buildable->GetName().Contains("Build_LightsControlPanel") || Cast<AFGBuildableCircuitSwitch>(Buildable)) {
			AFGBuildableLightsControlPanel* lightControl = Cast<AFGBuildableLightsControlPanel>(Buildable);
			AFGBuildableCircuitBridge* bridge = Cast<AFGBuildableCircuitBridge>(Buildable);
			TArray<UFGCircuitConnectionComponent*> connections = bridge->mConnections;
			//lightControl->OnLightControlPanelStateChanged.Broadcast(true);
			//lightControl->mDidFirstTimeUse = true;

			for (int i = 0; i < connections.Num(); i++) {
				UFGCircuitConnectionComponent* connection = connections[i];
				TArray<AFGBuildableWire*> ConnectionWires;
				connection->GetWires(ConnectionWires);

				for (AFGBuildableWire* TargetWire : ConnectionWires) {
					AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
					Design->AddElement(Wire);
					Design->BuildableMark.Add(Wire, 1);
					EnableHightLight(Wire);
				}
			}

		}

		if (Cast<AFGBuildableLightSource>(Buildable) || Cast<AFGBuildablePowerStorage>(Buildable)) {
			TArray<UActorComponent*> ReplicatedComponents = Buildable->ReplicatedComponents;
			for (int i = 0; i < ReplicatedComponents.Num(); i++) {
				if (Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i])) {
					UFGPowerConnectionComponent* ConnectionComponent;
					ConnectionComponent = Cast< UFGPowerConnectionComponent>(ReplicatedComponents[i]);
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

		if (Buildable->GetName().Contains("Build_ConveyorLiftMk")) {
			if (Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0]) {
				Design->AddElement(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0]);
				Design->BuildableMark.Add(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0], 1);
				EnableHightLight(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[0], SelectMaterial);
			}
			if (Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1]) {
				Design->AddElement(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1]);
				Design->BuildableMark.Add(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1], 1);
				EnableHightLight(Cast<AFGBuildableConveyorLift>(Buildable)->mSnappedPassthroughs[1], SelectMaterial);
			}
		}

		if (!foundCustomSelection) {
			Design->AddElement(Buildable);
			Design->BuildableMark.Add(Buildable, 1);
			EnableHightLight(Buildable, SelectMaterial);
		}

		//SML::Logging::info(TEXT("Add "), *Buildable->GetName(), TEXT(" "), *Buildable->GetTransform().ToString());
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
}

bool UFSSelection::ConnectSelect(AFGBuildable* Buildable)
{
	if (!ConnectSelectService->Ready()) return false;

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
	return false;
}

bool UFSSelection::ConnectSelectCheckReady()
{
	if (!ConnectSelectService->Ready()) return false;

	TArray<TWeakObjectPtr<AFGBuildable> >* Result;
	if (!ConnectSelectService->GetLastResult(Result)) return false;

	if (Result->Num() == 0) return true; //some error

	SetAutoRebuildTreeAll(false);

	if (LastSelectMode == 1) {
		for (TWeakObjectPtr<AFGBuildable>& Ptr : *Result) {
			AFGBuildable* Buildable = Ptr.Get();
			if (Buildable) {
				EnableHightLight(Buildable, SelectMaterial);
				Design->BuildableSet.Add(Buildable);
			}
		}
		Design->BuildableMark.Add((*Result)[0], 2);
	}
	else {
		for (TWeakObjectPtr<AFGBuildable>& Ptr : *Result) {
			AFGBuildable* Buildable = Ptr.Get();
			if (Buildable) {
				DisableHightLight(Buildable);
				Design->BuildableSet.Remove(Buildable);
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
		AFGBuildable* Buildable = Ptr.Get();
		if (Buildable) {
			EnableHightLight(Buildable, SelectMaterial);
		}
		int *Value = RectSelectBuffer.Find(Ptr);
		if (!Value) RectSelectBuffer.Add(Ptr, 1);
		else (*Value)++;
	}

	for (TWeakObjectPtr<AFGBuildable>& Ptr : *Nagetive) {
		AFGBuildable* Buildable = Ptr.Get();
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
	}
	else {
		SetAutoRebuildTreeAll(false);
		for (TPair<TWeakObjectPtr<AFGBuildable>, int>& Pair : RectSelectBuffer) {
			AFGBuildable* Buildable = Pair.Key.Get();
			if (Pair.Value == -1 && Buildable) {
				if (Buildable == Design->Anchor.Get()) {
					EnableHightLight(Buildable, FocusMaterial);
				}
				else {
					EnableHightLight(Buildable, SelectMaterial);
				}
			}
			else if (Pair.Value == 1) DisableHightLight(Buildable);
		}
		SetAutoRebuildTreeAll(true);
	}
	RectSelectBuffer.Empty();
}

void UFSSelection::Load(UFSDesign* Design)
{
	this->Design = Design;
	ActorMaterialCache.Empty();
	DynamicInstanceSet.Empty();
	CurrentFocus = nullptr;
}

void UFSSelection::LoadSelect(UFSDesign* Design)
{
	this->Design = Design;
	this->Design->RecheckNullptr();

	ActorMaterialCache.Empty();
	DynamicInstanceSet.Empty();

	SetAutoRebuildTreeAll(false);
	for (TWeakObjectPtr<AFGBuildable> BuildablePtr : Design->BuildableSet) if (BuildablePtr.Get()) {
		EnableHightLight(BuildablePtr.Get());
	}
	if (Design->Anchor.Get()) {
		EnableHightLight(Design->Anchor.Get(), this->FocusMaterial);
	}
	SetAutoRebuildTreeAll(true);

	ConnectSelectService->RefreshRecipe();
	CurrentFocus = nullptr;
}

void UFSSelection::LoadSetAnchor(UFSDesign* Design)
{
	this->Design = Design;
	this->Design->RecheckNullptr();

	ActorMaterialCache.Empty();
	DynamicInstanceSet.Empty();

	SetAutoRebuildTreeAll(false);
	for (TWeakObjectPtr<AFGBuildable> BuildablePtr : Design->BuildableSet) if (BuildablePtr.Get()) {
		EnableHightLight(BuildablePtr.Get());
	}
	SetAutoRebuildTreeAll(true);

	CurrentFocus = nullptr;
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
	CurrentFocus = nullptr;
	SelectedMap.Empty();
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

bool UFSSelection::IsHightLight(AFGBuildable* Buildable)
{
	return ActorMaterialCache.Contains(Buildable);
}

void UFSSelection::EnableHightLight(AFGBuildable* Buildable)
{
	FSActorMaterial& Cache = ActorMaterialCache.FindOrAdd(Buildable);
	FSActorMaterial& Cache2 = ActorMaterialCache2.FindOrAdd(Buildable);
	if (Cache.Init) return;
	Cache.Buildable = Buildable;
	EnableHightLight(Cache, Buildable, SelectMaterial);
}

void UFSSelection::EnableHightLight(AFGBuildable* Buildable, UMaterialInterface* Material)
{
	FSActorMaterial& Cache = ActorMaterialCache.FindOrAdd(Buildable);
	if (!Cache.Init) Cache.Buildable = Buildable;
	EnableHightLight(Cache, Buildable, Material);
}

void UFSSelection::HideHologram(AActor* Actor, FSMaterialHandle& ActorInfo)
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
				TInlineComponentArray<UMeshComponent*> MeshComps(Actor);
				for (const auto& MeshComponent : MeshComps) {
					//for (auto& ActorComp : components) {
						//auto MeshComponent = Cast<UMeshComponent>(ActorComp);
						//
					if (MeshComponent) {
						FString Name = MeshComponent->GetName();
						if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;
						// retrieve materials for this mesh from the cache for this actor
						auto SavedMaterialInterfacePtr = ActorInfo.MaterialMapping.Find(MeshComponent);
						if (SavedMaterialInterfacePtr) {
							auto SavedMaterialInterface = *SavedMaterialInterfacePtr;

							// restore all materials on this mesh
							int Mats = SavedMaterialInterface->MaterialInterfaceList.Num();
							for (int i = 0; i < Mats; i++) {
								MeshComponent->SetMaterial(i, SavedMaterialInterface->MaterialInterfaceList[i]);
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

void UFSSelection::EnableHightLight(FSActorMaterial& Cache, AFGBuildable* Buildable, UMaterialInterface* Material)
{

	AFSkyline* FSkyline = AFSkyline::Get(this);
	bool CacheExist = Cache.Init;
	if (CacheExist && !Cache.Buildable.Get()) {
		DisableHightLight(Buildable);
		return;
	}
	bool initializeCacheSaved = false;
	//initializeCacheSaved = SelectedMap.Contains(Buildable);
	for (TPair<AActor*, FSMaterialHandle*>& pair : SelectedMap) {
		AActor* actor1 = pair.Key;
		FString Name1 = actor1->GetName();
		FString Name2 = Buildable->GetName();
		if (Name1.Equals(Name2)) {
			initializeCacheSaved = true;
			break;
		}
	}

	FSMaterialHandle* Value = new FSMaterialHandle();
	if (!initializeCacheSaved) {
		SelectedMap.Add(Buildable, Value);
	}

	ActorList.AddUnique(Buildable);

	bool foundCustomStaticMesh = false;
	float CustomAdjustment = 0;
	bool useNormalHologram = false;

	Manager = AAbstractInstanceManager::GetInstanceManager(FSkyline->World);

	InstanceHandles = Buildable->mInstanceHandles;


	Data = Buildable->GetLightweightInstanceData();
	TArray<FInstanceData> Instances;
	if (!initializeCacheSaved) {
		if (Data != nullptr) {
			//TArray<FInstanceData> Instances = Data->GetInstanceData();
			//Instances = Data->Instances;
			//Instances[0].OverridenMaterials.Empty();
			//Instances[0].OverridenMaterials.Add(SelectMaterial);


			//Instances[0].RelativeTransform = FTransform::Identity;


			for (int i = 0; i < InstanceHandles.Num(); i++) {
				useNormalHologram = false;

				FInstanceData instanceData = FInstanceData();
				UStaticMesh* mesh = InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh();
				instanceData.StaticMesh = mesh;
				instanceData.RelativeTransform = InstanceHandles[i]->GetInstanceComponent()->GetRelativeTransform();
				//instanceData.RelativeTransform = Data->Instances[0].RelativeTransform;
				//instanceData.StaticMesh = Buildable->GetLightweightInstanceData()->Instances[0].StaticMesh;
				instanceData.OverridenMaterials.Add(Material);
				instanceData.DefaultPerInstanceCustomData = InstanceHandles[i]->GetInstanceComponent()->PerInstanceSMCustomData;
				instanceData.Mobility = InstanceHandles[i]->GetInstanceComponent()->Mobility;
				instanceData.NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
				Instances.Add(instanceData);

				FInstanceHandle* Handle = new FInstanceHandle();
				FTransform InstanceTransform;
				//InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
				//Manager->SetInstanced(Buildable, InstanceTransform, instanceData, Handle);
				Value->InstanceHandles.Add(Handle);
				//InstanceHandles[i]->HideInstance(true);
				MeshFromHandle = mesh;
			}
			//MeshFromInstanceData = Buildable->GetLightweightInstanceData()->Instances[0].StaticMesh;

		} else {
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				useNormalHologram = false;

				FInstanceData instanceData = FInstanceData();
				UStaticMesh* mesh = InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh();
				instanceData.StaticMesh = mesh;
				instanceData.RelativeTransform = InstanceHandles[i]->GetInstanceComponent()->GetRelativeTransform();
				//instanceData.RelativeTransform = Data->Instances[0].RelativeTransform;
				//instanceData.StaticMesh = Buildable->GetLightweightInstanceData()->Instances[0].StaticMesh;
				instanceData.OverridenMaterials.Add(Material);
				instanceData.DefaultPerInstanceCustomData = InstanceHandles[i]->GetInstanceComponent()->PerInstanceSMCustomData;
				instanceData.Mobility = InstanceHandles[i]->GetInstanceComponent()->Mobility;
				instanceData.NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
				Instances.Add(instanceData);

				FInstanceHandle* Handle = new FInstanceHandle();
				FTransform InstanceTransform;
				//InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
				//Manager->SetInstanced(Buildable, InstanceTransform, instanceData, Handle);
				Value->InstanceHandles.Add(Handle);
				//InstanceHandles[i]->HideInstance(true);

				MeshFromHandle = mesh;
			}
			//MeshFromInstanceData = Buildable->GetLightweightInstanceData()->Instances[0].StaticMesh;
		}
	} else {
		/*
		FSMaterialHandle** handleHolder = SelectedMap.Find(Buildable);
		if (handleHolder) {
			FSMaterialHandle handle = **handleHolder;
			for (int i = 0; i < (*handleHolder)->InstanceHandles.Num(); i++) {
				useNormalHologram = false;

				TArray<UMaterialInterface*> OverridenMaterials;
				OverridenMaterials.Add(Material);
				(*handleHolder)->InstanceHandles[i]->GetInstanceComponent()->OverrideMaterials = OverridenMaterials;
				(*handleHolder)->InstanceHandles[i]->UnHideInstance(true);
			}
		}
		*/

		useNormalHologram = false;
		FSMaterialHandle** handleHolder = SelectedMap.Find(Buildable);
		if (handleHolder) {
			/*
			for (int i = 0; i < (*handleHolder)->InstanceHandles.Num(); i++) {
				Manager->RemoveInstance((*handleHolder)->InstanceHandles[i]);
			}
			*/
			//(*handleHolder)->InstanceHandles.Empty();
			
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				useNormalHologram = false;

				FInstanceData instanceData = FInstanceData();
				UStaticMesh* mesh = InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh();
				instanceData.StaticMesh = mesh;
				instanceData.RelativeTransform = InstanceHandles[i]->GetInstanceComponent()->GetRelativeTransform();
				//instanceData.RelativeTransform = Data->Instances[0].RelativeTransform;
				//instanceData.StaticMesh = Buildable->GetLightweightInstanceData()->Instances[0].StaticMesh;
				instanceData.OverridenMaterials.Add(Material);
				instanceData.DefaultPerInstanceCustomData = InstanceHandles[i]->GetInstanceComponent()->PerInstanceSMCustomData;
				instanceData.Mobility = InstanceHandles[i]->GetInstanceComponent()->Mobility;
				instanceData.NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
				Instances.Add(instanceData);

				FInstanceHandle* Handle = new FInstanceHandle();
				FTransform InstanceTransform;
				InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
				//Manager->SetInstanced(Buildable, InstanceTransform, instanceData, Handle);
				//(*handleHolder)->InstanceHandles.Add(Handle);
				//InstanceHandles[i]->HideInstance(true);
				MeshFromHandle = mesh;
			}
			

		}

	}

	// works and stable only for simple things like foundations, doesnt work with lifts, etc yet
	
	if (InstanceHandles.Num() > 0) {

		//this->HandleID = InstanceHandles[0]->GetHandleID();
		//if (Cube != nullptr) {
			//return;
		//}

		TArray<uint32> handleIDs;
		TArray<UHierarchicalInstancedStaticMeshComponent*> copiedComponents;
		//TArray<int32> InstanceCount;

		for (int i = 0; i < InstanceHandles.Num(); i++) {
			this->HandleID = InstanceHandles[i]->GetHandleID();
			this->HandleID = InstanceHandles[i]->GetHandleID();
			handleIDs.Add(InstanceHandles[i]->GetHandleID());
			this->HandleID = InstanceHandles[i]->GetHandleID();
			this->HandleID = InstanceHandles[i]->GetHandleID();
		}
		
		for (int i = 0; i < InstanceHandles.Num(); i++) {
			
			if (!initializeCacheSaved) {

				handleIDs.Add(InstanceHandles[i]->GetHandleID());

				if (Value->HologramHelper == nullptr) {
					Value->HologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Buildable->GetActorTransform(), *(new FActorSpawnParameters()));
				}

				//comp2 = DuplicateObject <UHierarchicalInstancedStaticMeshComponent>(InstanceHandles[i]->GetInstanceComponent(), Manager);
				comp2 = NewObject<UHierarchicalInstancedStaticMeshComponent>(Value->HologramHelper);

				//comp2->ClearInstances();

				FTransform InstanceTransform;
				InstanceHandles[i]->GetInstanceComponent()->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), InstanceTransform, true);
				FTransform InstanceRelativeTransform = Instances[i].RelativeTransform;
				const FTransform InstanceSpawnLocation = InstanceRelativeTransform * Buildable->GetActorTransform();

				FVector Translate = InstanceTransform.GetTranslation();
				//std::string  str = TCHAR_TO_UTF8(*InstanceHandles[i]->GetInstanceComponent()->GetName());
				std::string  str = TCHAR_TO_UTF8(*InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh()->GetName());
				str.append("\n");
				std::string  str2 = "x position:" + std::to_string(Translate.X) + " y position:" + std::to_string(Translate.Y) + " z position:" + std::to_string(Translate.Z);
				str.append(str2);
				str.append("\n");


				std::wstring temp = std::wstring(str.begin(), str.end());
				LPCWSTR wideString = temp.c_str();
				OutputDebugStringW(wideString);


				InstanceCount.Add(comp2->GetInstanceCount());

				// both of these dont work 100% of the time?
				comp2->SetStaticMesh(InstanceHandles[i]->GetInstanceComponent()->GetStaticMesh());
				//comp2->SetStaticMesh(Instances[i].StaticMesh);

				comp2->OverrideMaterials = Instances[i].OverridenMaterials;
				comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				comp2->SetGenerateOverlapEvents(false);
				comp2->SetMobility(Instances[i].Mobility);
				comp2->AttachToComponent(Value->HologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				comp2->SetRelativeTransform(FTransform::Identity);
				comp2->SetVisibility(true);
				comp2->NumCustomDataFloats = Instances[i].NumCustomDataFloats;
				comp2->RegisterComponent();

				//comp2->AddInstance(InstanceTransform);
				comp2->AddInstanceWorldSpace(InstanceTransform);

				copiedComponents.Add(comp2);
				Value->copiedComponents.Add(comp2);

				for (const auto& ActorComp : Value->HologramHelper->GetComponentsByClass(UMeshComponent::StaticClass())) {
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

				InstanceHandles[i]->HideInstance(true);
				
			} else {
				
				FSMaterialHandle** handleHolder = SelectedMap.Find(Buildable);
				if (handleHolder) {
					FSMaterialHandle handle = **handleHolder;
					for (int i = 0; i < (*handleHolder)->copiedComponents.Num(); i++) {
						useNormalHologram = false;

						TArray<UMaterialInterface*> OverridenMaterials;
						OverridenMaterials.Add(Material);
						(*handleHolder)->copiedComponents[i]->OverrideMaterials = OverridenMaterials;
						(*handleHolder)->copiedComponents[i]->MarkRenderTransformDirty();

						for (const auto& ActorComp : (*handleHolder)->HologramHelper->GetComponentsByClass(UMeshComponent::StaticClass())) {
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

					}
				}
				
				
				
			}
			
			//comp2->ClearInstances();

			//FTransform Transform;
			//copiedComponents[i]->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), Transform, true);

			// test removes the current instance lift that is selected
			/*
			comp2->RemoveInstance(InstanceHandles[i]->GetHandleID());
			for (int j = 0; j < InstanceHandles.Num()-4;k j++) {
				comp2->RemoveInstance(InstanceHandles[j]->GetHandleID());
			}
			*/

			/*
			comp2->RemoveInstance(InstanceHandles[i]->GetHandleID()+1);
			for (int j = 0; j < InstanceHandles.Num(); j++) {
				comp2->RemoveInstance(InstanceHandles[j]->GetHandleID()+1);
			}
			*/
 
		}
		
		// number of instances to remove
		// test removes the current instance lift that is selected
		//for (int k = 0; k < 2; k++) {
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				//copiedComponents[i]->RemoveInstance(InstanceHandles[i]->GetHandleID());
				for (int j = 0; j < InstanceHandles.Num()-4; j++) {
					//copiedComponents[i]->RemoveInstance(InstanceHandles[j]->GetHandleID());
				}
			}
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				//copiedComponents[i]->RemoveInstance(InstanceHandles[i]->GetHandleID());
				for (int j = 0; j < InstanceHandles.Num() - 4; j++) {
					//copiedComponents[i]->RemoveInstance(InstanceHandles[j]->GetHandleID());
				}
			}

			// removes two lifts
			/*
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				copiedComponents[i]->RemoveInstance(InstanceHandles[i]->GetHandleID());
				for (int j = 0; j < InstanceHandles.Num() - 2; j++) {
					copiedComponents[i]->RemoveInstance(0);
				}
			}
			*/

			// removes two lifts
			
			
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				//FTransform Transform;
				//copiedComponents[i]->GetInstanceTransform(InstanceHandles[i]->GetHandleID(), Transform, true);
				//copiedComponents[i]->RemoveInstance(InstanceHandles[i]->GetHandleID());
			}
			
			

		//}

			for (TObjectIterator<AFGBuildableConveyorLift> Worker; Worker; ++Worker) {
				AFGBuildableConveyorLift* buildablePtr = *Worker;

				TArray<FInstanceHandle*> handles = buildablePtr->mInstanceHandles;

				//for (int i = 0; i < copiedComponents.Num(); i++) {
					for (int j = 0; j < handles.Num(); j++) {
						//if (copiedComponents[i]->GetName().Equals(handles[j]->GetInstanceComponent()->GetName())) {
							//copiedComponents[i]->RemoveInstance(handles[j]->GetHandleID());
						//}
					}
				//}
			}

		for (int j = 0; j < 2; j++) {
			//for (int i = 0; i < copiedComponents.Num(); i++) {
				//copiedComponents[i]->RemoveInstance(handleIDs[i]);
			//}
		}

		//for (int i = 0; i < copiedComponents.Num(); i++) {
			//this->HandleID = InstanceHandles[i]->GetHandleID();
			//handleIDs.RemoveAtSwap(InstanceHandles[i]->GetHandleID());
			bool ShouldRemapIndex = InstanceHandles.Num() > (int32)HandleID;
			if (ShouldRemapIndex) {
				//InstanceHandles[HandleID]->HandleID = HandleID;
			}
			//Manager->RemoveInstance(InstanceHandles[i]);
			//this->HandleID = InstanceHandles[i]->GetHandleID();
			//copiedComponents[i]->RemoveInstance(handleIDs[i]);

			// test removes the current instance lift that is selected
			

		//}

		//for (int i = 0; i < handleIDs.Num(); i++) {
			//copiedComponents[i]->RemoveInstance(handleIDs[i]);
			for (int j = 0; j < 4; j++) {
				//copiedComponents[i]->RemoveInstance(handleIDs[i]);
			}
		//}

		for (int i = 0; i < InstanceHandles.Num(); i++) {
			if (InstanceHandles[i]) {
				//InstanceHandles[i]->GetInstanceComponent()->OverrideMaterials = Instances[0].OverridenMaterials;

			/*
				UHierarchicalInstancedStaticMeshComponent* comp2 = DuplicateObject <UHierarchicalInstancedStaticMeshComponent>(InstanceHandles[i]->GetInstanceComponent(), Manager);
				comp2->OverrideMaterials = Instances[i].OverridenMaterials;
				comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				comp2->SetGenerateOverlapEvents(false);
				comp2->SetMobility(Instances[i].Mobility);
				comp2->AttachToComponent(Manager->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				comp2->SetRelativeTransform(FTransform::Identity);
				comp2->SetVisibility(true);
				comp2->NumCustomDataFloats = Instances[i].NumCustomDataFloats;
				comp2->RegisterComponent();
			*/

				/*
				UHierarchicalInstancedStaticMeshComponent* comp = NewObject <UHierarchicalInstancedStaticMeshComponent>(Manager);

				//Manager->RemoveInstance(InstanceHandles[i]);

				
				FTransform InstanceRelativeTransform = Instances[i].RelativeTransform;
				const FTransform InstanceSpawnLocation = InstanceRelativeTransform * Buildable->GetActorTransform();
				comp->SetStaticMesh(Instances[i].StaticMesh);
				comp->OverrideMaterials = Instances[i].OverridenMaterials;
				comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				comp->SetGenerateOverlapEvents(false);
				comp->SetMobility(Instances[i].Mobility);
				comp->AttachToComponent(Manager->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				comp->SetRelativeTransform(FTransform::Identity);
				comp->SetVisibility(true);
				comp->NumCustomDataFloats = Instances[i].NumCustomDataFloats;
				comp->RegisterComponent();
				comp->AddInstanceWorldSpace(InstanceSpawnLocation);
				*/

				//Manager->RemoveInstance(InstanceHandles[i]);

				
				//FTransform InstanceRelativeTransform = Instances[i].RelativeTransform;
				//const FTransform InstanceSpawnLocation = InstanceRelativeTransform * Buildable->GetActorTransform();
				//comp2 = DuplicateObject <UHierarchicalInstancedStaticMeshComponent>(InstanceHandles[i]->GetInstanceComponent(), Manager);

				// test removes the current instance lift that is selected
				/*
				comp2->RemoveInstance(InstanceHandles[i]->GetHandleID());
				for (int j = 0; j < InstanceHandles.Num()-4; j++) {
					comp2->RemoveInstance(InstanceHandles[j]->GetHandleID());
				}
				*/
				/*
				InstanceHandles.RemoveAtSwap(InstanceHandles[i]->GetHandleID());
				bool ShouldRemapIndex = InstanceHandles.Num() > (int32)HandleID;
				if (ShouldRemapIndex) {
					InstanceHandles[HandleID]->HandleID = HandleID;
				}
				*/

				//comp2->ClearInstances();
				/*
				comp2->OverrideMaterials = Instances[i].OverridenMaterials;
				comp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				comp2->SetGenerateOverlapEvents(false);
				comp2->SetMobility(Instances[i].Mobility);
				comp2->AttachToComponent(Cube->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				comp2->SetRelativeTransform(FTransform::Identity);
				comp2->SetVisibility(true);
				comp2->NumCustomDataFloats = Instances[i].NumCustomDataFloats;
				comp2->RegisterComponent();
				*/
				//comp2->AddInstanceWorldSpace(InstanceSpawnLocation);
				

				/*
				comp2->RemoveInstance(InstanceHandles[i]->GetHandleID());
				comp2->RemoveInstance(InstanceHandles[i]->GetHandleID());
				comp2->RemoveInstance(InstanceHandles[i]->GetHandleID());
				comp2->RemoveInstance(InstanceHandles[i]->GetHandleID());
				comp2->RemoveInstance(InstanceHandles[i]->GetHandleID());
				*/
				//comp2->RemoveInstance(InstanceHandles[i]->GetHandleID() - 1);
				//comp2->RemoveInstance(InstanceHandles[i]->GetHandleID() - 2);

				/*
				for (int i = 0; i < comp2->NumBuiltInstances; i++) {
					comp2->RemoveInstance(i);
				}
				*/
				//comp2->RemoveInstance(0);
				//comp2->RemoveInstance(0);
				//comp2->RemoveInstance(0);
				//comp2->RemoveInstance(0);
				//comp2->RemoveInstance(0);

			}
		}
		
		/*
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

		/*
		TArray<UStaticMeshComponent*> children;
		Cube->GetComponents<UStaticMeshComponent>(children);
		for (int j = 0; j < children.Num(); j++) {
			UMeshComponent* MeshComponent = Cast<UMeshComponent>(children[j]);
			if (MeshComponent) {
				int num = MeshComponent->GetNumMaterials();
				if (num) {
					for (int i = 0; i < num; i++) {
						MeshComponent->SetMaterial(i, SelectMaterial);
					}
				}
			}
		}
		*/
		

		//InstanceHandles.Empty();
		FTransform ActorTransform = Buildable->GetActorTransform();

		for (int i = 0; i < InstanceHandles.Num(); i++) {
			//Manager->SetInstanced(Buildable, ActorTransform, Instances[i], InstanceHandles[i]);
		}

		if (Cast<AFGBuildableConveyorLift>(Buildable)) {
			//Cast<AFGBuildableConveyorLift>(Buildable)->mTopTransform = Cast<AFGBuildableConveyorLift>(Buildable)->mTopTransform;
			//Buildable->PlayDismantleEffects();
		}

			//for (const auto& AbstractInstance : Instances)
			//{
				//for (int i = 0; i < InstanceHandles.Num(); i++) {

					//FInstanceHandle* Handle = new FInstanceHandle();
					//Manager->SetInstanced(Buildable, ActorTransform, AbstractInstance, InstanceHandles[i]);
					//InstanceHandles.Add(Handle);
				//}

				
				//FInstanceHandle* Handle2 = new FInstanceHandle();
				//Manager->SetInstanceFromDataStatic(Buildable, ActorTransform, AbstractInstance, Handle2);
				//InstanceHandles.Add(Handle2);
				//Manager->SetInstancedStatic(Buildable, ActorTransform, Data, InstanceHandles);
				
			//}

		//return;
	}
	

	/*
	if (Instances.Num() > 0) {
		
		FInstanceData InstanceData = Instances[0];
		Data->Instances[0].OverridenMaterials.Add(SelectMaterial);
		Data->Instances[0].StaticMesh = nullptr;
		foundCustomStaticMesh = true;
		return;
	}
	*/

	bool isInstanced = true;

	/*
	UFSBuildableOperator* PlaceHolderOperator = FSkyline->OperatorFactory->AcquireOperator(Buildable);
	FTransform PlaceHolderRelativeTransformVar;
	FSkyline->SplineHologramFactory->Load();
	AFGHologram* PlaceHolderHologram = PlaceHolderOperator->HologramCopy(PlaceHolderRelativeTransformVar);
	*/

	/*
	TArray<UActorComponent*> children;
	Buildable->GetComponents<UActorComponent>(children);
	for (int j = 0; j < children.Num(); j++) {
		UMeshComponent* MeshComponent = Cast<UMeshComponent>(children[j]);
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
	if (Buildable->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
		isInstanced = useNormalHologram = true;
	} else {
		for (const auto& ActorComp : Buildable->GetComponentsByClass(UMeshComponent::StaticClass())) {

			UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComp);
			//
			if (MeshComponent) {

				FString Name = MeshComponent->GetName();
				if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;

				if (!Name.Contains(TEXT("VisibilityMesh"))) {
					//isInstanced = false;
					//continue;
				}

				UFGColoredInstanceMeshProxy* MeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComponent);

				// prototype highlight code
				// works
				/*
				TArray< UMaterialInterface*> interfaces = MeshComponent->GetMaterials();
				FSMaterialHandle* Value = new FSMaterialHandle();
				Value->MaterialInterfaceList = interfaces;
				MaterialMapping2.Add(MeshComponent, Value);
				*/
				if (!initializeCacheSaved) {
					FSMeshMaterial* Value2 = new FSMeshMaterial();
					Value2->MaterialInterfaceList = MeshComponent->GetMaterials();
					Value->MaterialMapping.Add(MeshComponent, Value2);
				}

				// ignore all of this horrible game crashing code
				//FSavedMaterialInterfaces SavedMaterialInterface = FSavedMaterialInterfaces();
				//SavedMaterialInterfaces2.Add(MeshComponent, interfaces);
				//auto interfaces2 = SavedMaterialInterfaces2.Find(MeshComponent);
				//SavedMaterialInterface.MaterialInterfaces = MeshComponent->GetMaterials();
				//ActorInfo.SavedMaterialInterfaces.Add(MeshComponent, SavedMaterialInterface);
				//auto ActorInfo = SelectedMap.Find(Buildable);
				//TArray< UMaterialInterface*> interfaces2 = ActorInfo->SavedMaterialInterfaces.Find(MeshComponent).
				//FComponentSavedInterfaces interfaces2 = FComponentSavedInterfaces();
				/*
				//TArray< UMaterialInterface*> interfaces2 = MeshComponent->GetMaterials();
				FSavedMaterialInterfaces SavedMaterialInterface = FSavedMaterialInterfaces();
				SavedMaterialInterface.MaterialInterfaces = MeshComponent->GetMaterials();
				ActorInfo.SavedMaterialInterfaces.Add(MeshComponent, SavedMaterialInterface);
				*/

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
	if (isInstanced && useNormalHologram) {
		//Buildable->ToggleInstanceVisibility(false);
		Value->hologramVariation = true;
		
		AFGHologram* Hologram = nullptr;
		if (!initializeCacheSaved) {
			Value->hologramVariation = true;
			UFSBuildableOperator* BuildableOperator = FSkyline->OperatorFactory->AcquireOperator(Buildable);
			FTransform RelativeTransformVar;
			FSkyline->SplineHologramFactory->Load();
			Hologram = BuildableOperator->HologramCopy(RelativeTransformVar);
			Value->Hologram = Hologram;
		} else {
			FSMaterialHandle** handleHolder = SelectedMap.Find(Buildable);
			if (handleHolder) {
				FSMaterialHandle handle = **handleHolder;
				if (handle.Hologram == nullptr) {
					//handle.Hologram->Destroy();
					UFSBuildableOperator* BuildableOperator = FSkyline->OperatorFactory->AcquireOperator(Buildable);
					FTransform RelativeTransformVar;
					FSkyline->SplineHologramFactory->Load();
					Hologram = BuildableOperator->HologramCopy(RelativeTransformVar);
					(*handleHolder)->Hologram = Hologram;
				} else {
					Hologram = (*handleHolder)->Hologram;
				}
				//Value->Hologram = Hologram;
				
				//Hologram = handle.Hologram;
				//Hologram = (*handleHolder)->Hologram;
			}

		}
		if (Hologram != nullptr) {
			Hologram->SetActorHiddenInGame(false);
			Hologram->SetActorTransform(Buildable->GetTransform());
			if (Material == SelectMaterial) {
				Hologram->SetPlacementMaterialState(EHologramMaterialState::HMS_OK);
			}
			if (Material == FocusMaterial) {
				Hologram->SetPlacementMaterialState(EHologramMaterialState::HMS_ERROR);
			}

			FTransform HologramTransform = Buildable->GetTransform();
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

	/*
	FSelectedActorInfo* ActorInfo2 = SelectedMap.Find(Buildable);
	if (ActorInfo2) {
		FSelectedActorInfo ActorInfo3 = *ActorInfo2;
		HideHologram(Buildable, ActorInfo3);
	}
	*/
	/*
	FSMaterialHandle** handleHolder = SelectedMap.Find(Buildable);
	if (handleHolder) {
		FSMaterialHandle handle = **handleHolder;
		HideHologram(Buildable, handle);
	}
	*/
	//FSMaterialHandle handle = **SelectedMap.Find(Buildable);
	//HideHologram(Buildable, handle);

	FString FullPathName = Buildable->GetPathName();
	/*
	if (FullPathName.Contains("Build_AssemblerMk1") || FullPathName.Contains("Build_ManufacturerMk1") || FullPathName.Contains("Build_OilRefinery") || FullPathName.Contains("Build_HadronCollider")) {
		for (const auto& ActorComp : Buildable->GetComponentsByClass(UMeshComponent::StaticClass())) {
			UMeshComponent* MeshComp = Cast<UMeshComponent>(ActorComp);
			int Mats = MeshComp->GetNumMaterials();
			if (Mats) {
				for (int i = 0; i < Mats; i++) {
					//MeshComp->SetMaterial(i, FSkyline->SelectedMaterial);
					MeshComp->SetMaterial(i, SelectMaterial);
				}
				// turn off instancing if it's a special mesh
				auto StaticMeshProxy = Cast<UProxyInstancedStaticMeshComponent>(MeshComp);
				if (StaticMeshProxy) {
					StaticMeshProxy->SetInstanced(false);
				}else {
					auto ColoredMeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComp);
					if (ColoredMeshProxy) {
						ColoredMeshProxy->SetInstanced(true);
						UFGColoredInstanceMeshProxy* MeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComp);
						if (MeshProxy) {
							if (MeshProxy->mInstanceManager != nullptr) {
								//AddInstance(MeshProxy, SelectMaterial);
							}
						}
					} else {
						auto ProdIndInst = Cast<UFGProductionIndicatorInstanceComponent>(MeshComp);
						if (ProdIndInst) {
							ProdIndInst->SetInstanced(true);
						}
					}
				}
			}
		}
	}
	*/
	

	/*
	for (UActorComponent* ActorComponent : Buildable->GetComponents()) {
		UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComponent);
		if (MeshComponent) {
			FString Name = MeshComponent->GetName();
			if (Name.Contains(TEXT("FogPlane")) || Name.Contains(TEXT("Smoke")) || Name.Contains(TEXT("StaticMeshComponent"))) continue;

			UFGColoredInstanceMeshProxy* MeshProxy = Cast<UFGColoredInstanceMeshProxy>(MeshComponent);
			int num = MeshComponent->GetNumMaterials();
			if (num) {
				FSMeshMaterial* MeshMaterial = CacheExist ? nullptr : &Cache.MeshList.AddDefaulted_GetRef();
				if (!CacheExist) {
					MeshMaterial->MeshComponent = MeshComponent;
					MeshMaterial->MeshProxy = MeshProxy;
					MeshMaterial->MaterialInterfaceList.Empty();
				}
				if (MeshProxy) {
					FSMaterialHandle** LocalCache = MaterialMapping.Find(MeshProxy);
					FText txt = Buildable->mDisplayName;
					//std::string str2 = txt.ToString();
					FString txt2 = txt.ToString();
					if (LocalCache) {
						// some temporary fixes here for handleID -1 issues, need to investigate this issue further
						//if (MeshProxy->mInstanceHandle.HandleID != -1) {
						if (!txt2.Contains(TEXT("Assembler")) & !txt2.Contains(TEXT("Manufacturer"))) {
							RemoveInstance(MeshProxy, *LocalCache);
						}
					} else {
						//if (MeshProxy->mInstanceHandle.HandleID != -1) {
						if (!txt2.Contains(TEXT("Assembler")) & !txt2.Contains(TEXT("Manufacturer"))) {
							RemoveInstance(MeshProxy, MeshProxy->mInstanceHandle.ColorIndex);
						}
					}
					//if (MeshProxy->mInstanceHandle.HandleID != -1) {
					if (!txt2.Contains(TEXT("Assembler")) & !txt2.Contains(TEXT("Manufacturer"))) {
						AddInstance(MeshProxy, Material);
					}
				} else {
					for (int i = 0; i < num; i++) {
						if (!CacheExist) {
							UMaterialInterface* Interface = MeshComponent->GetMaterial(i);
							MeshMaterial->MaterialInterfaceList.Add(Interface);

							UMaterialInstanceDynamic* Dynamic = Cast<UMaterialInstanceDynamic>(Interface);
							if (Dynamic) DynamicInstanceSet.Add(Dynamic);
						}
						MeshComponent->SetMaterial(i, Material);
					}
				}
				
			}
		}
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

void UFSSelection::EnableHightLightFocus(AFGBuildable* Buildable)
{
	if (!Buildable) return;
	if (CurrentFocus != Buildable) {
		if (CurrentFocus) DisableHightLightFocus();
		CurrentFocus = Buildable;
	}
	EnableHightLight(Buildable, FocusMaterial);
}

void UFSSelection::DisableHightLightFocus()
{
	if (!CurrentFocus) return;
	if (Design->IsElementSelected(CurrentFocus)) {
		if (CurrentFocus != Design->Anchor) {
			EnableHightLight(CurrentFocus, SelectMaterial);
		}
	}
	else DisableHightLight(CurrentFocus);
	CurrentFocus = nullptr;
}

void UFSSelection::DisableHightLight(AFGBuildable* Buildable)
{
	FSActorMaterial* Cache = ActorMaterialCache.Find(Buildable);

	if (!Cache) return;

	FString FullPathName = Buildable->GetPathName();

	//if (FullPathName.Contains("Build_AssemblerMk1") || FullPathName.Contains("Build_ManufacturerMk1") || FullPathName.Contains("Build_OilRefinery") || FullPathName.Contains("Build_HadronCollider")) {
	//}
	//else {

	FSMaterialHandle** handleHolder = SelectedMap.Find(Buildable);
	if (handleHolder) {

		if ((*handleHolder)->HologramHelper) {
			(*handleHolder)->HologramHelper->Destroy();
			(*handleHolder)->HologramHelper = nullptr;
			for (int i = 0; i < (*handleHolder)->copiedComponents.Num(); i++) {
				(*handleHolder)->copiedComponents[i]->ClearInstances();
			}

			InstanceHandles = Buildable->mInstanceHandles;
			for (int i = 0; i < InstanceHandles.Num(); i++) {
				InstanceHandles[i]->UnHideInstance(true);
			}

		} else {

			FSMaterialHandle handle = **handleHolder;
			//if (!handle.hologramVariation) {
			HideHologram(Buildable, handle);

			if (Buildable->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
				AFGBuildableWidgetSign* BuildableSign = Cast<AFGBuildableWidgetSign>(Buildable);
				BuildableSign->PasteSettings_Implementation(BuildableSign->CopySettings_Implementation());
			}
			//}
			if (handle.hologramVariation) {
				//Buildable->ToggleInstanceVisibility(true);
				if (handle.Hologram) {
					handle.Hologram->SetDisabled(true);
					handle.Hologram->Destroy();
					handle.Hologram = nullptr;
					(*handleHolder)->Hologram = nullptr;
					//FSMaterialHandle* Value = new FSMaterialHandle();
					//Value->Hologram = handle.Hologram;
					//SelectedMap.Add(Buildable, Value);
				}
			}
		}

	}
	SelectedMap.Remove(Buildable);

	ActorList.Remove(Buildable);

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
		
		ActorMaterialCache.Remove(Buildable);
		
	//}
	if (CurrentFocus == Buildable) CurrentFocus = nullptr;
}

void UFSSelection::DisableAll()
{
	for (TPair<AActor*, FSMaterialHandle*>& pair : SelectedMap){
		AActor* actor = pair.Key;
		FSMaterialHandle handle = *pair.Value;

		int num = ActorList.Num();
		AActor* actor2 = nullptr;
		bool exists = false;
		for (int i = 0; i < num; i++) {
			if (ActorList[i] != nullptr) {
				if (actor == ActorList[i]) {
					exists = true;
					break;
				}
			}
		}

		//AActor* actor2 = ActorList.Find(actor);
		if (exists) {
			if (pair.Value->HologramHelper) {
				pair.Value->HologramHelper->Destroy();
				pair.Value->HologramHelper = nullptr;
				for (int i = 0; i < pair.Value->copiedComponents.Num(); i++) {
					pair.Value->copiedComponents[i]->ClearInstances();
				}

				InstanceHandles = Cast<AFGBuildable>(actor)->mInstanceHandles;
				for (int i = 0; i < InstanceHandles.Num(); i++) {
					InstanceHandles[i]->UnHideInstance(true);
				}
				continue;
			}
			HideHologram(actor, handle);
			AFGBuildable* buildableActor = Cast<AFGBuildable>(actor);
			//buildableActor->ToggleInstanceVisibility(true);
			if (buildableActor->GetClass()->IsChildOf<AFGBuildableWidgetSign>()) {
				AFGBuildableWidgetSign* BuildableSign = Cast<AFGBuildableWidgetSign>(buildableActor);
				BuildableSign->PasteSettings_Implementation(BuildableSign->CopySettings_Implementation());
			}
		}
		if (handle.hologramVariation) {
			//Buildable->ToggleInstanceVisibility(true);
			if (handle.Hologram) {
				handle.Hologram->SetDisabled(true);
				handle.Hologram->Destroy();
				FSMaterialHandle* Value = new FSMaterialHandle();
				FSMaterialHandle* handle = pair.Value;
				//pair.Value = nullptr;
				pair.Value->Hologram = nullptr;
				//SelectedMap.Add(actor, Value);
			}
		}
	}
	for (TPair<AFGBuildable*, FSActorMaterial >& Pair : ActorMaterialCache)
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
	Node->ISMComponent->AttachTo(HISMComponent->GetAttachmentRootActor()->GetRootComponent());
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

