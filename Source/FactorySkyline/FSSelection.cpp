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
	
	SelectMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	SelectMaterial->SetScalarParameterValue(FName("MinOpacity"), 0.05f);
	SelectMaterial->SetScalarParameterValue(FName("MaxOpacity"), 0.25f);
	SelectMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.043735f, 0.201556f, 0.496933f));
	SelectMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);

	FocusMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	FocusMaterial->SetScalarParameterValue(FName("MinOpacity"), 0.05f);
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
		DisableHightLight(Buildable);

		//SML::Logging::info(TEXT("Remove "), *Buildable->GetName());
	}
	else {
		Design->AddElement(Buildable);
		Design->BuildableMark.Add(Buildable, 1);
		EnableHightLight(Buildable, SelectMaterial);

		//SML::Logging::info(TEXT("Add "), *Buildable->GetName(), TEXT(" "), *Buildable->GetTransform().ToString());
	}
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

void UFSSelection::EnableHightLight(FSActorMaterial& Cache, AFGBuildable* Buildable, UMaterialInterface* Material)
{
	bool CacheExist = Cache.Init;
	if (CacheExist && !Cache.Buildable.Get()) {
		DisableHightLight(Buildable);
		return;
	}

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
					if (LocalCache) RemoveInstance(MeshProxy, *LocalCache);
					else RemoveInstance(MeshProxy, MeshProxy->mInstanceHandle.ColorIndex);
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
						MeshComponent->SetMaterial(i, Material);
					}
				}
				
			}
		}
	}

	Cache.Init = true;
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
		if (CurrentFocus != Design->Anchor) EnableHightLight(CurrentFocus, SelectMaterial);
	}
	else DisableHightLight(CurrentFocus);
	CurrentFocus = nullptr;
}

void UFSSelection::DisableHightLight(AFGBuildable* Buildable)
{
	FSActorMaterial* Cache = ActorMaterialCache.Find(Buildable);
	if (!Cache) return;

	for (FSMeshMaterial& MeshMaterial : Cache->MeshList) {
		UMeshComponent* MeshComponent = MeshMaterial.MeshComponent.Get();
		UFGColoredInstanceMeshProxy* MeshProxy = MeshMaterial.MeshProxy;
		if (MeshProxy) {
			FSMaterialHandle** LocalCache = MaterialMapping.Find(MeshProxy);
			if (LocalCache) {
				RemoveInstance(MeshProxy, *LocalCache);
				if (MeshComponent && Cache->Buildable.Get()) {
					//SML::Logging::info(MeshProxy->mInstanceHandle.colorIndex);
					if (MeshProxy->mInstanceHandle.ColorIndex < BUILDABLE_COLORS_MAX_SLOTS)
						MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, MeshProxy->mInstanceHandle.ColorIndex);
					else
						MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, 0);
				}
			}
		}
		else if (MeshComponent && Cache->Buildable.Get()) {
			for (int i = 0; i < MeshMaterial.MaterialInterfaceList.Num(); i++)
				MeshComponent->SetMaterial(i, MeshMaterial.MaterialInterfaceList[i]);
		}
	}

	ActorMaterialCache.Remove(Buildable);
	if (CurrentFocus == Buildable) CurrentFocus = nullptr;
}

void UFSSelection::DisableAll()
{
	for (TPair<AFGBuildable*, FSActorMaterial >& Pair : ActorMaterialCache)
		for (FSMeshMaterial& MeshMaterial : Pair.Value.MeshList) {
			UMeshComponent* MeshComponent = MeshMaterial.MeshComponent.Get();
			UFGColoredInstanceMeshProxy* MeshProxy = MeshMaterial.MeshProxy;
			if (MeshProxy) {
				FSMaterialHandle** LocalCache = MaterialMapping.Find(MeshProxy);
				if (LocalCache) {
					RemoveInstance(MeshProxy, *LocalCache);
					if (MeshComponent && Pair.Value.Buildable.Get()) {
						if (MeshProxy->mInstanceHandle.ColorIndex < BUILDABLE_COLORS_MAX_SLOTS)
							MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, MeshProxy->mInstanceHandle.ColorIndex);
						else
							MeshProxy->mInstanceManager->AddInstance(MeshProxy->GetComponentTransform(), MeshProxy->mInstanceHandle, 0);
					}
				}
			}
			else if (MeshComponent && Pair.Value.Buildable.Get()) {
				for (int i = 0; i < MeshMaterial.MaterialInterfaceList.Num(); i++)
					MeshComponent->SetMaterial(i, MeshMaterial.MaterialInterfaceList[i]);
			}
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
	FSISMNode* NodePtr = GetISM(MeshProxy, Material);
	NodePtr->ISMComponent->AddInstance(MeshProxy->GetComponentTransform());
	FSMaterialHandle* Value = new FSMaterialHandle();
	Value->Handle = NodePtr->Handles.Add(Value);
	Value->Material = Material;
	MaterialMapping.Add(MeshProxy, Value);
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
	UHierarchicalInstancedStaticMeshComponent* HISMComponent = MeshProxy->mInstanceManager->mInstanceComponent;
	int Index = MeshProxy->mInstanceHandle.HandleID;
	if (Index < 0) return;

	MeshProxy->mInstanceHandle.HandleID = INDEX_NONE;
	HISMComponent->RemoveInstance(Index);
	TArray <UFGColoredInstanceManager::FInstanceHandle*>& HandlesArray = MeshProxy->mInstanceManager->mHandles;
	HandlesArray.RemoveAtSwap(Index);
	if (Index >= 0 && Index < HandlesArray.Num()) HandlesArray[Index]->HandleID = Index;
}

