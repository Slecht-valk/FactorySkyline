// ILikeBanas


#include "FactorySkyline/FSBuilder.h"
#include "FactorySkyline/FSSelection.h"
#include "FactorySkyline/FSkyline.h"
#include "FactorySkyline/FSDesign.h"
#include "FactorySkyline/FSMenuItem.h"
#include "FactorySkyline/FSController.h"
#include "FactorySkyline/UI/FSkylineUI.h"
#include "FactorySkyline/UI/FSResourceIcon.h"
#include "FactorySkyline/Operators/FSBuildableOperator.h"
#include "FactorySkyline/Operators/FSConveyorLiftOperator.h"
#include "Equipment/FGBuildGun.h"
#include "FGBuildableSubsystem.h"
#include "FGFluidIntegrantInterface.h"
#include "FGRailroadSubsystem.h"
#include "FGPlayerController.h"
#include "FGPipeConnectionComponent.h"
#include "FGPipeSubsystem.h"
#include "FGRailroadTrackConnectionComponent.h"
#include "FGTrainStationIdentifier.h"
#include "FGGameState.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableManufacturer.h"
#include "Buildables/FGBuildablePipeline.h"
#include "Buildables/FGBuildableConveyorBelt.h"
#include "Buildables/FGBuildableRailroadTrack.h"
#include "Buildables/FGBuildableRailroadSignal.h"
#include "Buildables/FGBuildableRailroadStation.h"
#include "Buildables/FGBuildableWire.h"
#include "Buildables/FGBuildableBlueprintDesigner.h"
#include "Hologram/FGHologram.h"
#include "Hologram/FGBuildableHologram.h"
#include "Hologram/FGFactoryHologram.h"
#include "Hologram/FGFoundationHologram.h"

#include "Hologram/FGRailroadTrackHologram.h"

#include "Buildables/FGBuildablePowerPole.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"

#include "Buildables/FGBuildableRailroadSwitchControl.h"

#include <cmath>

void AFSBuilder::Init()
{
	AFSkyline* FSkyline = AFSkyline::Get(this);
	if (!FSkyline) return;

	this->World = FSkyline->World;
	this->FGController = FSkyline->FGController;

	this->OperatorFactory = FSkyline->OperatorFactory;
	this->SplineHologramFactory = FSkyline->SplineHologramFactory;
	this->BuildableService = FSkyline->BuildableService;
	this->FSCtrl = FSkyline->FSCtrl;
	this->SkylineUI = FSkyline->SkylineUI;
	
	this->SyncBuild = NewObject<UFSSyncBuild>(this);

	this->IsBuilding = false;

	this->Cost.Init(this->SplineHologramFactory);
	this->Inventory = &FSkyline->FSCtrl->Inventory;
}

bool AFSBuilder::CheckAnchor(UFSDesign* DesignParam)
{
	AnchorConnection = AnchorInput = AnchorOutput = nullptr;

	TObjectPtr<UClass> classObject = DesignParam->Anchor.BuildableClass;
	//AFGBuildable* defaultObject = classObject->GetDefaultObject<AFGBuildable>();
	//UAbstractInstanceDataObject* instanceDataObject = nullptr;

	//instanceDataObject = defaultObject ? defaultObject->GetInstanceDataCDO() : nullptr;

	// TODO REWORK THIS
	
	if (!DesignParam->Anchor.Buildable) {
		return true;
	}
	if (Cast<AFGBuildableConveyorBase>(DesignParam->Anchor.Buildable)) {
		AFSkyline* FSkyline = AFSkyline::Get(this);
		AFGBuildableConveyorBase* Conveyor = Cast<AFGBuildableConveyorBase>(DesignParam->Anchor.Buildable);
		//UFGFactoryConnectionComponent* Connection = Conveyor->GetConnection0();
		UFGFactoryConnectionComponent* Connection = FSkyline->AdaptiveUtil->GetConveyorConnection(Conveyor, 0);

		FSBuildable ptr;
		ptr.Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
		if (!Connection->IsConnected() || !DesignParam->IsElementSelected(ptr)) AnchorInput = Connection;
		else {
			FSBuildable ptr;
			ptr.Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			AFGBuildable* Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			if (!Buildable || !DesignParam->IsElementSelected(ptr)) AnchorInput = Connection;
		}

		//Connection = Conveyor->GetConnection1();
		Connection = FSkyline->AdaptiveUtil->GetConveyorConnection(Conveyor, 1);

		FSBuildable ptr1;
		ptr1.Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
		if (!Connection->IsConnected() || !DesignParam->IsElementSelected(ptr1)) AnchorOutput = Connection;
		else {
			AFGBuildable* Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			if (!Buildable || !DesignParam->IsElementSelected(ptr1)) AnchorOutput = Connection;
		}

		return AnchorConnection || AnchorInput || AnchorOutput;
	}
	
	if (Cast<AFGBuildablePipeBase>(DesignParam->Anchor.Buildable)) {

		AFGBuildablePipeBase* Pipe = Cast<AFGBuildablePipeBase>(DesignParam->Anchor.Buildable);
		UFGPipeConnectionComponentBase* Connection = Pipe->GetConnection1();

		if (!Connection->IsConnected()) AnchorConnection = Connection;
		else {
			FSBuildable ptr2;
			ptr2.Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			AFGBuildable* Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			if (!Buildable || !DesignParam->IsElementSelected(ptr2)) AnchorConnection = Connection;
		}

		Connection = Pipe->GetConnection0();
		if (!Connection->IsConnected()) AnchorConnection = Connection;
		else {

			FSBuildable ptr3;
			ptr3.Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			AFGBuildable* Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			if (!Buildable || !DesignParam->IsElementSelected(ptr3)) AnchorConnection = Connection;
		}

		return AnchorConnection != nullptr;
	}

	if (Cast<AFGBuildableRailroadTrack>(DesignParam->Anchor.Buildable)) {
		AFGBuildableRailroadTrack* Track = Cast<AFGBuildableRailroadTrack>(DesignParam->Anchor.Buildable);
		UFGRailroadTrackConnectionComponent* Connection = Track->GetConnection(1);
		
		if (!Connection->IsConnected()) AnchorConnection = Connection;
		else {
			
			FSBuildable ptr;
			ptr.Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			AFGBuildable* Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			if (!Buildable || !DesignParam->IsElementSelected(ptr)) AnchorConnection = Connection;
		}

		Connection = Track->GetConnection(0);
		if (!Connection->IsConnected()) AnchorConnection = Connection;
		else {
			FSBuildable ptr;
			ptr.Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			AFGBuildable* Buildable = Cast<AFGBuildable>(Connection->GetConnection()->GetAttachmentRootActor());
			if (!Buildable || !DesignParam->IsElementSelected(ptr)) AnchorConnection = Connection;
		}

		return AnchorConnection != nullptr;
	}

	
	return true;
}

void AFSBuilder::Load(UFSDesign* DesignParam, bool FullPreview)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);
	FSkyline->SkylineUI->CompletionWidget->SetVisibility(ESlateVisibility::Collapsed);

	/*
	UFGBuildGunStateDismantle* dismantleState = FSkyline->FSCtrl->FGBuildGun->mDismantleStateClass.GetDefaultObject();
	//FSkylin->FSCtrl->FGBuildGun->GotoDismantleState();
	FSkyline->FSCtrl->GetPlayer()->SetOverrideEquipment(FSkyline->FSCtrl->FGBuildGun);
	AFGEquipment* Equipment = FSkyline->FSCtrl->GetPlayer()->GetEquipmentInSlot(EEquipmentSlot::ES_ARMS);
	FSkyline->FSCtrl->GetPlayer()->EquipEquipment(FSkyline->FSCtrl->FGBuildGun);
	FSkyline->FSCtrl->FGBuildGun->Equip(FSkyline->FSCtrl->GetPlayer());
	FSkyline->FSCtrl->FGBuildGun->GotoStateInternal(EBuildGunState::BGS_DISMANTLE);
	*/

	SplineHologramFactory->Load();

	this->Design = DesignParam;
	this->Design->RecheckNullptr();

	FSBuildable* AnchorBuildable = &this->Design->Anchor;

	// TODO REWORK THIS METHOD
	this->AnchorOperator = OperatorFactory->CreateOperator(AnchorBuildable);

	this->Hologram = this->AnchorOperator->CreateHologram();
	if (this->Hologram == nullptr) return;
	//this->Hologram->SetActorHiddenInGame(false);
	//SML::Logging::info(*this->Hologram->GetFullName());

	// TODO REWORK THIS
	
	FVector AnchorLocation;

	if (AnchorBuildable->Buildable) {
		AnchorLocation = AnchorBuildable->Buildable->GetTransform().GetLocation();
	}
	else {
		AnchorLocation = AnchorBuildable->Transform.GetLocation();
	}
	if (AnchorOutput) AnchorLocation = AnchorOutput->GetComponentLocation();
	if (AnchorInput) AnchorLocation = AnchorInput->GetComponentLocation();
	if (AnchorConnection) AnchorLocation = AnchorConnection->GetComponentLocation();
	
	
	// TODO REWORK THIS
	
	for (FSBuildable BuildablePtr : DesignParam->BuildableSet) {
		if (BuildablePtr != FSBuildable()) {
			//AFGBuildable* Buildable = BuildablePtr.Get();
			//float Dist;
			FTransform Transform;
			if (BuildablePtr.Buildable) {
				Transform = BuildablePtr.Buildable->GetTransform();
			}
			else {
				Transform = BuildablePtr.Transform;
			}
			float Dist = (Transform.GetLocation() - AnchorLocation).Size();
			if (BuildablePtr == *AnchorBuildable || Dist < 2500.0f || FullPreview) {
				UFSBuildableOperator* BuildableOperator = OperatorFactory->AcquireOperator(&BuildablePtr);

				FTransform RelativeTransformVar;
				AFGHologram* HologramVar = BuildableOperator->HologramCopy(RelativeTransformVar);

				if (HologramVar) {
					this->HologramList.Add(HologramVar);
					this->RelativeTransform.Add(RelativeTransformVar);
				}
			}
		}
	}
	
	
	Cost.Empty();
	AFGGameState* GameState = Cast<AFGGameState>(World->GetGameState());
	if (!GameState || !(GameState->GetCheatNoCost() == 1)) {
		AFSController* FSCtrlVar = (AFSController*)this->FSCtrl;
		bool NoCost = FSCtrlVar->GetPlayer()->GetInventory()->GetNoBuildCost();
		if (!NoCost) {
			Cost.AddResource(DesignParam);
		}
	}

	LastShow = LastValid = false;

	SplineHologramFactory->Unload();

}

void AFSBuilder::Unload()
{
	for (AFGHologram* HologramVar : HologramList) {
		HologramVar->SetActorHiddenInGame(true);
		HologramVar->SetDisabled(true);
		HologramVar->Destroy();
	}
	HologramList.Empty();
	RelativeTransform.Empty();

	if (this->Hologram) {
		this->Hologram->SetActorHiddenInGame(true);
		this->Hologram->SetDisabled(true);
		this->Hologram->Destroy();
		this->Hologram = nullptr;
	}
	this->AnchorOperator = nullptr;
}

bool AFSBuilder::Test(const FHitResult& Hit)
{
	UE_LOG(LogSkyline, Verbose, TEXT("%s"), (*Hit.GetActor()->GetFullName()));
	UE_LOG(LogSkyline, Verbose, TEXT("%s"), (*Hit.GetActor()->GetTransform().ToString()));
	UE_LOG(LogSkyline, Verbose, TEXT("%s"), (*this->Hologram->GetTransform().ToString()));
	UE_LOG(LogSkyline, Verbose, TEXT("%s"), (*Hit.Location.ToString()));
	UE_LOG(LogSkyline, Verbose, TEXT("%s"), (*Hit.TraceStart.ToString()));
	UE_LOG(LogSkyline, Verbose, TEXT("%s"), (*(FString::Printf(TEXT("bBlockingHit:%s\nbStartPenetrating:%s\nTime:%f\nLocation:%s\nImpactPoint:%s\nNormal:%s\nImpactNormal:%s\nTraceStart:%s\nTraceEnd:%s\nPenetrationDepth:%f\nItem:%d\nFaceIndex:%d"),
		Hit.bBlockingHit == true ? TEXT("True") : TEXT("False"),
		Hit.bStartPenetrating == true ? TEXT("True") : TEXT("False"),
		Hit.Time,
		*Hit.Location.ToString(),
		*Hit.ImpactPoint.ToString(),
		*Hit.Normal.ToString(),
		*Hit.ImpactNormal.ToString(),
		*Hit.TraceStart.ToString(),
		*Hit.TraceEnd.ToString(),
		Hit.PenetrationDepth,
		Hit.Item,
		Hit.FaceIndex))));
	
	//Hologram->DoMultiStepPlacement(false);
	/*
	AFGBuildableConveyorBelt* Belt = Cast<AFGBuildableConveyorBelt>(this->Design->ReferenceItem.Get());
	SML::Logging::info(*Belt->GetFullName());
	SML::Logging::info(*Belt->GetTransform().ToString());
	for (FSplinePointData Data : Belt->GetSplineData()) {
		SML::Logging::info(*Data.ToString());
	}

	SML::Logging::info(*Belt->GetConnection0()->GetFullName());
	SML::Logging::info(*Belt->GetConnection0()->GetConnectorLocation(false).ToString());
	SML::Logging::info(*Belt->GetConnection0()->GetConnectorNormal().ToString());
	SML::Logging::info(*Belt->GetConnection1()->GetFullName());
	SML::Logging::info(*Belt->GetConnection1()->GetConnectorLocation(false).ToString());
	SML::Logging::info(*Belt->GetConnection1()->GetConnectorNormal().ToString());
	*/

	//AFSkyline::Get(this)->ConveyorHologramFactory->CreateBeltHologram(Cast<AFGBuildableConveyorBelt>(this->Design->ReferenceItem.Get()));

	return false;
}

bool AFSBuilder::Build(FSRepeat* Repeat)
{
	if (!(this->LastShow && this->LastValid)) return false;
	if (!CheckCost()) return false;

	if (this->IsBuilding) {
		UE_LOG(LogSkyline, Verbose, TEXT("Warnning: FSBuilder call build while building"));
		return false;
	}

	// TODO REWORK THIS
	if (this->Design->Anchor == FSBuildable()) return false;

	if (this->Hologram == nullptr) return false;

	AFSkyline* FSkyline = AFSkyline::Get(this);
	if (!FSkyline) return false;

	SyncBuild->Load();

	UFSDesignMenu* CurrentDesignMenu = Cast<UFSDesignMenu>(FSkyline->SkylineUI->FindLastDesignItem()->CurrentExpandedItem);
	if (!CurrentDesignMenu) return false;

	for (int i = 0; i < this->HologramList.Num(); i++)
		this->HologramList[i]->SetActorHiddenInGame(true);
	LastShow = false;

	this->IsBuilding = true;
	FSkyline->SkylineUI->CompletionWidget->SetVisibility(ESlateVisibility::Collapsed);

	// TODO REWORK THIS
	
	SyncBuild->Anchor = this->Design->Anchor;
	SyncBuild->BuildableSet = this->Design->BuildableSet;
	
	SyncBuild->CurrentDesignMenu = CurrentDesignMenu;
	SyncBuild->NewDesignMenu = UFSDesignMenu::CopyFrom(CurrentDesignMenu->ParentItem, CurrentDesignMenu, true);
	SyncBuild->NewDesign = SyncBuild->NewDesignMenu->Design;

	bool ValidTarget;
	this->Target = Repeat ? Repeat->Next() : GetFixedTargetTransform(this->Hologram, ValidTarget);
	this->Source = Repeat ? Repeat->Source : GetFixedSourceTransform();
	SyncBuild->FSTransform = FSTransformOperator(this->Source, this->Target);

	OperatorFactory->BuildableMapping = &SyncBuild->BuildableMapping;
	OperatorFactory->InternalConnectionMapping = &SyncBuild->InternalConnectionMapping;
	SyncBuild->OperatorFactory = this->OperatorFactory;

	SyncBuild->PreWork();
	this->Consume();
	//SyncBuild->DoWork(1e6f);
	//SyncBuild->Unload();

	// doing the start of work needlessly delays building for no reason so lets not
	//SyncBuild->StartDoWorkTimer(1.0f);

	// instead call the callback once to do the first building step
	SyncBuild->DoWorkTimerCallback();

	return true;
}

bool AFSBuilder::CheckReady(float TimeLimit)
{
	//SML::Logging::info(SyncBuild->GetCurrent(), TEXT(" "), SyncBuild->GetTotal());
	/*
	if (SyncBuild->DoWork(TimeLimit)) {
		this->IsBuilding = false;
		SyncBuild->Unload();
		return true;
	}
	*/
	return false;
}

void AFSBuilder::Update(const FHitResult& Hit)
{
	if (this->Hologram == nullptr) return;

	bool ShouldShow = false;
	bool Valid = false;
	bool ValidTarget = false;

	AFSkyline* FSkyline = AFSkyline::Get(this);

	
	FSHitResults FSHit = FSkyline->FSCtrl->GetSelectHitResult();
	AFGBuildable* Building = nullptr;
	Building = FSkyline->FSCtrl->AcquireBuildable(Hit);

	FSBuildable Buildable;
	Buildable.Abstract = FSHit.Abstract;
	Buildable.Handle = FSHit.Handle;
	Buildable.RuntimeData = FSHit.RuntimeData;
	Buildable.BuildableClass = FSHit.BuildableClass;
	Buildable.Transform = FSHit.InstanceTransform;
	Buildable.Buildable = Building;

	AFGLightweightBuildableSubsystem* lightweightSubsystem;
	lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(FSkyline->World);
	

	// if no temporary has been spawned and this is a abstract instance
	if (Buildable != FSBuildable() && Buildable.Buildable == nullptr) {

		/*
		AFGLightweightBuildableSubsystem* lightweightSubsystem;

		lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(FSkyline->World);
		lightweightSubsystem->AddInstanceConverterInstigator(10000, nullptr, Buildable.RuntimeData.Transform);

		bool didSpawn = false;
		lightweightSubsystem->FindOrSpawnBuildableForRuntimeData(&Buildable.RuntimeData, Buildable.Handle.GetHandleID(), didSpawn);
		*/

		//AFGLightweightBuildableSubsystem* lightweightSubsystem;
		//lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(FSkyline->World);

		FLightweightBuildableInstanceRef buildableRef;
		AFGLightweightBuildableSubsystem::ResolveLightweightInstance(Buildable.Handle, buildableRef);

		//if (TemporaryBuildable == nullptr) {
			//TemporaryBuildable = buildableRef.SpawnTemporaryBuildable();

		TemporaryBuildable = UFGLightweightBuildableBlueprintLibrary::SpawnTemporaryFromLightweight(buildableRef);

	}

	this->AnchorOperator->UpdateHologramState(Hit, this->Hologram, ShouldShow, Valid);

	lightweightSubsystem->RemoveStaleTemporaryBuildables();

	//this->Hologram->SetActorHiddenInGame(false);
	//return;

	//SML::Logging::info(*this->Hologram->GetActorLocation().ToString());

	if (IsBuilding) ShouldShow = false;

	if (ShouldShow) {
		this->Target = GetFixedTargetTransform(this->Hologram, ValidTarget);
		if (!ValidTarget) ShouldShow = false;
	}

	if (ShouldShow) {
		if (!CheckCost()) Valid = false;
	}

	/*
	FVector translate = this->Target.GetTranslation();


	FVector Vector;
	Vector.X = translate.X;
	Vector.Y = translate.Y;
	Vector.Z = translate.Z;
	*/

	//FQuat Quat1 = Repeat->NextRelative.GetRotation();

	//FQuat Quat;
	//Quat.X = 0;
	//Quat.Y = 0;
	//Quat.Z = 0;

	//float W = FMath::Sqrt(FMath::Max(0.0f, 1.0f - Quat.X * Quat.X - Quat.Y * Quat.Y - Quat.Z * Quat.Z));
	//Quat.W = W;

	//this->Target = FTransform(Quat, Vector);

	//FTransform RotationTransform(Quat, Vector);

	if (LastShow != ShouldShow || ShouldShow) {

		FSTransformOperator FSTransform;
		if (ShouldShow) FSTransform = FSTransformOperator(this->Source = GetFixedSourceTransform(), this->Target);

		for (int i = 0; i < this->HologramList.Num(); i++) {
			if (LastShow != ShouldShow)	this->HologramList[i]->SetActorHiddenInGame(!ShouldShow);

			//FTransform CurrentTransform = this->HologramList[i]->GetActorTransform();

			//FTransform RelativeTransform = CurrentTransform.GetRelativeTransform(RotationTransform);

			//if (ShouldShow) this->HologramList[i]->SetActorTransform(FSTransform.Transform(RelativeTransform));

			//if (ShouldShow) this->HologramList[i]->SetActorTransform(RelativeTransform);

			//FTransform CurrentTransform = this->RelativeTransform[i];
			//FTransform RelativeTransform = CurrentTransform.GetRelativeTransform(RotationTransform);
			//if (ShouldShow) this->HologramList[i]->SetActorTransform(RelativeTransform);


			// doesn't work
			/*
			FTransform PivotTransform = FTransform(this->RelativeTransform[i].GetRotation(), this->AnchorOperator->Source->GetTransform().GetTranslation());
			FTransform DeltaPivotToOriginal = this->RelativeTransform[i] * PivotTransform.Inverse();
			FTransform RotationTransform(Quat);
			FTransform New;
			New = FTransform(Quat.Inverse() * (float)1, FVector(0, 0, 0)) *
				DeltaPivotToOriginal *
				FTransform(RotationTransform.GetRotation(), FVector(0, 0, 0)) *
				PivotTransform;
			this->HologramList[i]->SetActorTransform(New);
			*/

			//this->HologramList[i]->SetActorTransform(this->Target);


			//this->HologramList[i]->SetActorTransform(FSTransform.Transform(this->AnchorOperator->Source->GetTransform()));

			// sets the holograms to the current selected buildable locations
			//if (ShouldShow) this->HologramList[i]->SetActorTransform(this->RelativeTransform[i]);

			if (ShouldShow) this->HologramList[i]->SetActorTransform(FSTransform.Transform(this->RelativeTransform[i]));
			this->HologramList[i]->SetPlacementMaterialState(EHologramMaterialState::HMS_OK);
		}

		LastShow = ShouldShow;
		LastValid = Valid;
	}
}

void AFSBuilder::Update(FSRepeat* Repeat)
{

	FVector translate = Repeat->NextRelative.GetTranslation();


	FVector Vector;
	Vector.X = translate.X;
	Vector.Y = translate.Y;
	Vector.Z = translate.Z;

	FQuat Quat1 = Repeat->NextRelative.GetRotation();

	FQuat Quat;
	Quat.X = Quat1.X;
	Quat.Y = Quat1.Y;
	Quat.Z = Quat1.Z;

	float W = FMath::Sqrt(FMath::Max(0.0f, 1.0f - Quat.X * Quat.X - Quat.Y * Quat.Y - Quat.Z * Quat.Z));
	Quat.W = W;

	Repeat->NextRelative = FTransform(Quat, Vector);

	if (!Repeat) return;
	FSTransformOperator FSTransform = FSTransformOperator(Repeat->Source, Repeat->Next());
	if (!IsBuilding) {
		CheckCost();
	}

	for (int i = 0; i < this->HologramList.Num(); i++) {
		this->HologramList[i]->SetActorHiddenInGame(false);
		this->HologramList[i]->SetPlacementMaterialState(EHologramMaterialState::HMS_OK);
		this->HologramList[i]->SetActorTransform(FSTransform.Transform(this->RelativeTransform[i]));
	}
}

FTransform AFSBuilder::GetFixedTargetTransform(AFGHologram* HologramParam, bool& Valid)
{
	FTransform Transform = HologramParam->GetTransform();
	Valid = true;

	if (Cast<AFGSplineHologram>(this->Hologram)) {
		AFGSplineHologram* SplineHologram = Cast<AFGSplineHologram>(this->Hologram);
		//SML::Logging::info(*SplineHologram->GetTransform().GetRotation().Rotator().ToString());
		
		AFGConveyorBeltHologram* BeltHologram = Cast<AFGConveyorBeltHologram>(this->Hologram);
		if (BeltHologram) {
			FSplinePointData Data;
			SplineHologram->GetLastSplineData(Data);
			Transform.SetRotation(Data.ArriveTangent.ToOrientationQuat());

			AnchorConnection = AnchorInput ? AnchorInput : AnchorOutput;
			UFGFactoryConnectionComponent* Connection = nullptr;
			if (AnchorInput && AnchorOutput) {
				Connection = UFGFactoryConnectionComponent::FindOverlappingConnections(World, Transform.GetLocation(), nullptr, 50.0f, EFactoryConnectionConnector::FCC_CONVEYOR, EFactoryConnectionDirection::FCD_INPUT);
				if (Connection) {
					if (Connection->GetDirection() == EFactoryConnectionDirection::FCD_SNAP_ONLY) AnchorConnection = AnchorInput;
					else AnchorConnection = AnchorOutput;
				}
				else AnchorConnection = AnchorInput;
			}
			else {
				if (AnchorInput) Connection = UFGFactoryConnectionComponent::FindOverlappingConnections(World, Transform.GetLocation(), nullptr, 50.0f, EFactoryConnectionConnector::FCC_CONVEYOR, EFactoryConnectionDirection::FCD_INPUT);
				if (AnchorOutput) Connection = UFGFactoryConnectionComponent::FindOverlappingConnections(World, Transform.GetLocation(), nullptr, 50.0f, EFactoryConnectionConnector::FCC_CONVEYOR, EFactoryConnectionDirection::FCD_OUTPUT);
				if (Connection) {
					Valid = (Connection->GetDirection() == EFactoryConnectionDirection::FCD_SNAP_ONLY);
				}
			}
		}

		UFSConveyorLiftOperator* LiftOperator = Cast<UFSConveyorLiftOperator>(AnchorOperator);
		if (LiftOperator && Cast<AFGConveyorLiftHologram>(this->Hologram)) {
			AnchorConnection = AnchorInput ? AnchorInput : AnchorOutput;
			UFGFactoryConnectionComponent* Connection = nullptr;
			if (LiftOperator->HitConnection) {
				FVector Location = LiftOperator->HitConnection->GetComponentTransform().GetLocation();
				if (AnchorInput && AnchorOutput) {
					if (LiftOperator->HitConnection->GetDirection() == EFactoryConnectionDirection::FCD_INPUT) AnchorConnection = AnchorOutput;
					else if (LiftOperator->HitConnection->GetDirection() == EFactoryConnectionDirection::FCD_OUTPUT ) AnchorConnection = AnchorInput;
					else {
						Connection = UFGFactoryConnectionComponent::FindOverlappingConnections(World, Location, nullptr, 50.0f, EFactoryConnectionConnector::FCC_CONVEYOR, EFactoryConnectionDirection::FCD_INPUT);
						if (Connection) {
							if (Connection->GetDirection() == EFactoryConnectionDirection::FCD_SNAP_ONLY) AnchorConnection = AnchorInput;
							else AnchorConnection = AnchorOutput;
						}
						else AnchorConnection = AnchorInput;
					}
				}
				else {
					if (AnchorInput) {
						if (LiftOperator->HitConnection->GetDirection() == EFactoryConnectionDirection::FCD_INPUT) Connection = LiftOperator->HitConnection;
						else Connection = UFGFactoryConnectionComponent::FindOverlappingConnections(World, Location, nullptr, 50.0f, EFactoryConnectionConnector::FCC_CONVEYOR, EFactoryConnectionDirection::FCD_INPUT);
					}
					if (AnchorOutput) {
						if (LiftOperator->HitConnection->GetDirection() == EFactoryConnectionDirection::FCD_OUTPUT) Connection = LiftOperator->HitConnection;
						else Connection = UFGFactoryConnectionComponent::FindOverlappingConnections(World, Location, nullptr, 50.0f, EFactoryConnectionConnector::FCC_CONVEYOR, EFactoryConnectionDirection::FCD_OUTPUT);
					}
					if (Connection) {
						Valid = (Connection->GetDirection() == EFactoryConnectionDirection::FCD_SNAP_ONLY);
					}
				}
			}
		}

		if (Cast<AFGPipelineHologram>(this->Hologram)) {
			FSplinePointData Data;
			SplineHologram->GetLastSplineData(Data);
			Transform.SetRotation(Data.ArriveTangent.ToOrientationQuat());
		}

		if (Cast<AFGRailroadTrackHologram>(this->Hologram)) {
			FSplinePointData Data;
			SplineHologram->GetLastSplineData(Data);
			Transform.SetRotation(Data.ArriveTangent.ToOrientationQuat());
		}
	}

	return Transform;
}

FTransform AFSBuilder::GetFixedSourceTransform()
{
	FTransform Transform;
	// TODO REWORK THIS
	
	if (this->Design->Anchor.Buildable) {
		Transform = this->Design->Anchor.Buildable->GetTransform();
	}
	else {
		Transform = this->Design->Anchor.Transform;
	}
	if (this->Design->Anchor.Buildable) {
		if (Cast<AFGBuildableConveyorBase>(this->Design->Anchor.Buildable) || Cast<AFGBuildablePipeBase>(this->Design->Anchor.Buildable) || Cast<AFGBuildableRailroadTrack>(this->Design->Anchor.Buildable)) {
			Transform = AnchorConnection->GetComponentTransform();
			if (Cast<AFGBuildableConveyorLift>(this->Design->Anchor.Buildable)) return Transform;
			return FTransform((-Transform.GetRotation().Vector()).ToOrientationQuat(), Transform.GetLocation());
		}
	}
	
	return Transform;
}

bool AFSBuilder::CheckCost()
{
	
	UFSkylineUI* SkylineUIVar = (UFSkylineUI*)this->SkylineUI;
	FSInventory Left = *Inventory;
	
	TMap<TSubclassOf<UFGItemDescriptor>, int> Minus;
	Left.AddResource(&Cost, -1);
	if (Left.Valid(Minus)) {
		SkylineUIVar->ItemBox->SetVisibility(ESlateVisibility::Collapsed);
		return true;
	}

	AFSController* FSCtrlVar = (AFSController*)this->FSCtrl;
	if (FSCtrlVar->GetPlayer()) {
		Left.AddResource(FSCtrlVar->GetPlayer()->GetInventory());
		if (Left.Valid(Minus)) {
			SkylineUIVar->ItemBox->SetVisibility(ESlateVisibility::Collapsed);
			return true;
		}
	}

	SkylineUIVar->ItemBox->ClearChildren();
	int Count = 0;
	for (TPair<TSubclassOf<UFGItemDescriptor>, int>& Pair : Minus) {
		UFSResourceIcon* Icon;
		if (Count < SkylineUIVar->ResourceIcon.Num()) {
			Icon = SkylineUIVar->ResourceIcon[Count];
		}
		else {
			TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(this, TEXT("/FactorySkyline/Widget_ResourceIcon.Widget_ResourceIcon_C"));
			Icon = CreateWidget<UFSResourceIcon>(SkylineUIVar, WidgetClass);
			SkylineUIVar->ResourceIcon.Add(Icon);
		}
		Icon->Load(Pair.Key, Pair.Value);
		SkylineUIVar->ItemBox->AddChild(Icon);
		Count++;
	}
	SkylineUIVar->ItemBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	return false;
}

bool AFSBuilder::Consume()
{
	AFSController* FSCtrlVar = (AFSController*)this->FSCtrl;
	if (FSCtrlVar->GetPlayer())
		return Inventory->Consume(FSCtrlVar->GetPlayer()->GetInventory(), &Cost);
	return Inventory->Consume(nullptr, &Cost);
}

void AFSBuilder::ScrollUp()
{
	if (this->Hologram)
		this->Hologram->Scroll(1);
}

void AFSBuilder::ScrollDown()
{
	if (this->Hologram)
		this->Hologram->Scroll(-1);
}

void AFSBuilder::onBuildFinish()
{
	//CurrentDesignMenu->ParentItem->AddChildAfter(NewDesignMenu, CurrentDesignMenu);
	//this->IsBuilding = false;
}


AFSBuilder::~AFSBuilder()
{
}

void UFSSyncBuild::Load()
{
	BuildableMapping.Empty();
	InternalConnectionMapping.Empty();
	List.Empty();
}

void UFSSyncBuild::Unload()
{
	NewDesignMenu = nullptr;
	BuildableMapping.Empty();
	InternalConnectionMapping.Empty();
	List.Empty();

	UFSBuildableOperatorList.Empty();
	passThroughBuildableList.Empty();
	passThroughNewList.Empty();
	LightsControlPanelNewList.Empty();
	LightSourceNewList.Empty();

}

void UFSSyncBuild::PreWork()
{
	Step = 0;
	Current = 0;
	Index = 0;
	for (FSBuildable BuildablePtr : this->BuildableSet) {
		//if (BuildablePtr.Get()) {
			List.Add(BuildablePtr);
		//}
	}
	Total = List.Num();

	//UFSSelection::SetAutoRebuildTreeAll(false);
}

void UFSSyncBuild::DoWork()
{
	//this->TimeLimit = TimeLimitParam;
	//Time.Start();
	//while (Time.GetTime() < TimeLimitParam) {
	if (Step == 0) {
		StepA();
		
		// start the delayed work after first step so, hopefully given enough time for the buildables to fully initialize?
		AFSkyline* FSkyline = AFSkyline::Get(this);
		FSkyline->Builder->SyncBuild->StartDoWorkTimer(0.1f);

		return;
	}
	if (Step == 1) {
		StepB();
		return;
	}
	if (Step == 2) {
		StepC();
		return;
	}

	//if (Step == 3)
			//return true;
	//}
	//return false;

	AFSkyline* FSkyline = AFSkyline::Get(this);
	FSkyline->Builder->IsBuilding = false;
	FSkyline->GetWorldTimerManager().ClearTimer(TimerHandle_DoWork);
	this->Unload();


}

void UFSSyncBuild::StepA()
{
	//AFSkyline* FSkyline = AFSkyline::Get(this);

	//UFSBuildableOperatorList.Empty();
	//passThroughBuildableList.Empty();
	//passThroughNewList.Empty();

	for (; Index < List.Num(); Index++) {
		//if (Time.GetTime() > TimeLimit) return;

		FSBuildable* Buildable = &List[Index];
		if (Buildable) {
			UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(Buildable);

			// check to see if its a pure abstract because the buildable wont exist
			if (!Buildable->Buildable) continue;

			if (Cast< AFGBuildableRailroadSignal>(Buildable->Buildable)) continue;
			if (Cast<AFGBuildableWire>(Buildable->Buildable)) continue;
			if (Cast<AFGBuildableRailroadSwitchControl>(Buildable->Buildable)) continue;
			
			//check if its a lift
			if (Cast<AFGBuildableConveyorLift>(Buildable->Buildable)) {
				TArray< class AFGBuildablePassthrough* > ThroughsSource = Cast<AFGBuildableConveyorLift>(Buildable->Buildable)->GetSnappedPassthroughs();
				// if its connected to any passthroughs we build these a little different so skip the normal buildig method.
				if (ThroughsSource[0] || ThroughsSource[1]) {
					continue;
				}
			}

			//if (!Operator->firstBuild) {
			//if (Buildable->GetName().Contains("Build_PowerPoleMk")) {
				AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);
				if (NewBuildable != nullptr) {
					BuildableMapping.Add(Buildable->Buildable, NewBuildable);

					if (NewBuildable) {

						Operator->UpdateInternelConnection(NewBuildable);
						//NewBuildable->PlayBuildEffects(FSkyline->FSCtrl->GetPlayer());
						//NewBuildable->ExecutePlayBuildEffects();

						FSBuildable buildable;
						buildable.Buildable = NewBuildable;
						this->NewDesign->BuildableSet.Add(buildable);

						if (Cast<AFGBuildablePassthrough>(Buildable->Buildable)) {
							/*
							if (Buildable->GetName().Contains("Build_FoundationPassthrough_Pipe_C")) {
								continue;
							}
							*/
							if (!Buildable->Buildable->GetName().Contains("Build_FoundationPassthrough_Lift_C")) {
								continue;
							}
							UFSBuildableOperatorList.Add(Operator);
							passThroughBuildableList.Add(Cast<AFGBuildablePassthrough>(Buildable->Buildable));
							passThroughNewList.Add(Cast<AFGBuildablePassthrough>(NewBuildable));


						}

						if (Cast<AFGBuildableLightsControlPanel>(NewBuildable)) {
							LightsControlPanelNewList.Add(Cast<AFGBuildableLightsControlPanel>(NewBuildable));
						}

						if (Cast<AFGBuildableLightSource>(NewBuildable)) {
							LightSourceNewList.Add(Cast<AFGBuildableLightSource>(NewBuildable));
						}

					}
				}
			//}
		}
		Current++;
	}
	Index = 0;
	Step++;

	// conveyor buildablepassthrough logic here
	// first build all of the passthroughs

	//FPlatformProcess::Sleep(10.0f);

	//std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(10.0f * 1000)));

	for (int i = 0; i < passThroughBuildableList.Num(); i++) {
		AFGBuildablePassthrough* passThrough1 = passThroughBuildableList[i];
		UFGConnectionComponent* bottomConnection1 = passThrough1->mBottomSnappedConnection;
		UFGConnectionComponent* TopConnection1 = passThrough1->mTopSnappedConnection;

		// check if conveyor lift is connected to this through at the bottom
		if (bottomConnection1 != nullptr) {
			AFGBuildableConveyorLift* bottomConnectedLift1 = Cast<AFGBuildableConveyorLift>(Cast< UFGFactoryConnectionComponent>(bottomConnection1)->GetOuterBuildable());
			bool foundNoPassThroughConnections = true;
			int foundIndex = 0;
			for (int j = 0; j < passThroughBuildableList.Num(); j++) {
				// because we only want to look at other connections
				if (i == j) {
					continue;
				}

				AFGBuildablePassthrough* passThrough2 = passThroughBuildableList[j];
				UFGConnectionComponent* TopConnection2 = passThrough2->mTopSnappedConnection;

				if (!TopConnection2) {
					continue;
				}

				AFGBuildableConveyorLift* topConnectedLift1 = Cast<AFGBuildableConveyorLift>(Cast< UFGFactoryConnectionComponent>(TopConnection2)->GetOuterBuildable());
				if (bottomConnectedLift1 == topConnectedLift1) {
					foundNoPassThroughConnections = false;
					foundIndex = j;
					break;
				}
			}
			if (foundNoPassThroughConnections) {

				FSBuildable ptr;
				ptr.Buildable = Cast< AFGBuildable>(bottomConnectedLift1);

				UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(&ptr);
				Operator->bottomPassThrough = passThroughNewList[i];
				AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);
				if (NewBuildable != nullptr) {
					BuildableMapping.Add(Cast< AFGBuildable>(bottomConnectedLift1), NewBuildable);
					Operator->UpdateInternelConnection(NewBuildable);


					FSBuildable buildable;
					buildable.Buildable = NewBuildable;
					this->NewDesign->BuildableSet.Add(buildable);
				}
			}
		}
		////

		// check if conveyor lift is connected to this through at the top
		if (TopConnection1 != nullptr) {
			AFGBuildableConveyorLift* topConnectedLift1 = Cast<AFGBuildableConveyorLift>(Cast< UFGFactoryConnectionComponent>(TopConnection1)->GetOuterBuildable());
			bool foundNoPassThroughConnections = true;
			int foundIndex = 0;
			for (int j = 0; j < passThroughBuildableList.Num(); j++) {
				// because we only want to look at other connections
				if (i == j) {
					continue;
				}

				AFGBuildablePassthrough* passThrough2 = passThroughBuildableList[j];
				UFGConnectionComponent* bottomConnection2 = passThrough2->mBottomSnappedConnection;
				UFGConnectionComponent* TopConnection2 = passThrough2->mTopSnappedConnection;

				if (!bottomConnection2) {
					continue;
				}

				AFGBuildableConveyorLift* bottomConnectedLift4 = Cast<AFGBuildableConveyorLift>(Cast< UFGFactoryConnectionComponent>(bottomConnection2)->GetOuterBuildable());
				if (bottomConnectedLift4 == topConnectedLift1) {
					foundNoPassThroughConnections = false;
					foundIndex = j;
					break;
				}

				// its not connected to to anything else so this is the end of a piece
				if (bottomConnection2 == nullptr && TopConnection2 == nullptr) {
				}

			}
			if (foundNoPassThroughConnections) {

				FSBuildable ptr;
				ptr.Buildable = Cast< AFGBuildable>(topConnectedLift1);

				UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(&ptr);
				//Operator->bottomPassThrough = passThroughNewList[i];
				Operator->topPassThrough = passThroughNewList[i];
				AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);
				if (NewBuildable != nullptr) {
					BuildableMapping.Add(Cast< AFGBuildable>(topConnectedLift1), NewBuildable);
					Operator->UpdateInternelConnection(NewBuildable);

					FSBuildable buildable;
					buildable.Buildable = NewBuildable;
					this->NewDesign->BuildableSet.Add(buildable);
				}
			}

			// a top connect is connected to a bottom connection
			if (!foundNoPassThroughConnections) {

				// check if a connection between pass throughs has already been made if so we don't need to make duplicates
				///if (passThroughNewList[foundIndex]->mTopSnappedConnection != nullptr || passThroughNewList[i]->mBottomSnappedConnection != nullptr) {
					//continue;
				//}

				AFGBuildablePassthrough* passThrough2 = passThroughBuildableList[foundIndex];
				UFGConnectionComponent* bottomConnection2 = passThrough2->mBottomSnappedConnection;
				AFGBuildableConveyorLift* bottomConnectedLift = Cast<AFGBuildableConveyorLift>(Cast< UFGFactoryConnectionComponent>(bottomConnection2)->GetOuterBuildable());
				if (bottomConnectedLift == topConnectedLift1) {

					FSBuildable ptr;
					ptr.Buildable = Cast< AFGBuildable>(bottomConnectedLift);

					UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(&ptr);
					Operator->bottomPassThrough = passThroughNewList[i];
					Operator->topPassThrough = passThroughNewList[foundIndex];
					AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);
					if (NewBuildable != nullptr) {
						BuildableMapping.Add(Cast< AFGBuildable>(topConnectedLift1), NewBuildable);
						Operator->UpdateInternelConnection(NewBuildable);

						FSBuildable buildable;
						buildable.Buildable = NewBuildable;
						this->NewDesign->BuildableSet.Add(buildable);
					}
				}

			}
		}

	}

}

void UFSSyncBuild::StepB()
{
	for (; Index < List.Num(); Index++) {
		//if (Time.GetTime() > TimeLimit) return;

		FSBuildable* Buildable = &List[Index];
		if (Buildable) {
			UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(Buildable);

			// check to see if its a pure abstract because the buildable wont exist
			if (!Buildable->Buildable) continue;

			if (Cast<AFGBuildableWire>(Buildable->Buildable)) {
				//if (!Operator->firstBuild) {
					AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);

					if (NewBuildable != nullptr) {
						BuildableMapping.Add(Buildable->Buildable, NewBuildable);

						if (NewBuildable) {

							Operator->UpdateInternelConnection(NewBuildable);
							//NewBuildable->PlayBuildEffects(this->Player);
							//NewBuildable->ExecutePlayBuildEffects();

							FSBuildable buildable;
							buildable.Buildable = NewBuildable;
							this->NewDesign->BuildableSet.Add(buildable);
						}
					}
					Current++;
				//}
			}
			else {
				AFGBuildable** NewBuildable = BuildableMapping.Find(Buildable->Buildable);

				if (NewBuildable && *NewBuildable) {
					Operator->ApplySettingsTo(*NewBuildable);
					Operator->ApplyConnection(*NewBuildable, FSTransform, true);
				}
			}
		}
	}

	Index = 0;
	Step++;
	
	//std::this_thread::sleep_for(std::chrono::nanoseconds(50000000));
	for (int i = 0; i < LightsControlPanelNewList.Num(); i++) {
		AFGBuildableLightsControlPanel* controlPanel = LightsControlPanelNewList[i];
		AFGBuildableControlPanelHost* controlPanelHost = Cast< AFGBuildableControlPanelHost>(controlPanel);
		//controlPanel->PasteSettings_Implementation(controlPanel->CopySettings_Implementation());
		//controlPanel->SetLightControlData(controlPanel->GetLightControlData());
		//controlPanel->SetLightDataOnControlledLights(controlPanel->GetLightControlData());
		//controlPanel->SetLightEnabled(controlPanel->IsLightEnabled());
		//controlPanel->mLightControlData = SourceBuildableLightsControlPanel->mLightControlData;
		//controlPanel->mIsEnabled = SourceBuildableLightsControlPanel->mIsEnabled;
		//controlPanel->OnRep_IsEnabled();
		//controlPanel->PasteSettings_Implementation(controlPanel->CopySettings_Implementation());
		AFGBuildableCircuitBridge* bridge = Cast<AFGBuildableCircuitBridge>(controlPanel);
		
		TArray<UFGCircuitConnectionComponent*> connections = bridge->mConnections;
		UFGCircuitConnectionComponent* downStreamConnection = controlPanelHost->mDownstreamConnection;
		//UFGCircuitConnectionComponent* connection = connections[0];
		
		int32 hostCircuitId = downStreamConnection->GetCircuitID();

		for (int j = 0; j < LightSourceNewList.Num(); j++) {
			//LightSourceNewList[j]->mPowerInfo->mCircuitID;
			AFGBuildableLightSource* lightSource = LightSourceNewList[j];
			UFGPowerInfoComponent* powerInfo = lightSource->mPowerInfo;
			int32 lightSourceId = powerInfo->mCircuitID;

			// check if the host and the light source are on the same circuit
			if (hostCircuitId == lightSourceId) {
				FLightSourceControlData controlData = FLightSourceControlData();

				controlData.ColorSlotIndex = controlPanel->mLightControlData.ColorSlotIndex;
				controlData.Intensity = controlPanel->mLightControlData.Intensity;
				controlData.IsTimeOfDayAware = controlPanel->mLightControlData.IsTimeOfDayAware;

				lightSource->mLightControlData = controlData;

				lightSource->SetLightControlData(controlData);
				lightSource->OnRep_IsEnabled();
				lightSource->OnRep_LightControlData();
				lightSource->OnRep_HasPower();
				lightSource->OnRep_IsDay();
				lightSource->UpdateMeshDataAndHandles();
				lightSource->UpdateCurrentLightColor();
				lightSource->UpdatePowerConsumption();

			}
		}

		/*
		for (int j = 0; j < connections.Num(); j++) {
			UFGCircuitConnectionComponent* connection = connections[i];
			TArray<AFGBuildableWire*> ConnectionWires;
			connection->GetWires(ConnectionWires);

			for (int k = 0; k < ConnectionWires.Num(); k++) {
				AFGBuildableWire* Wire = ConnectionWires[j];
			}
			//r (AFGBuildableWire* TargetWire : ConnectionWires) {
				//AFGBuildable* Wire = Cast<AFGBuildable>(TargetWire);
			//}
		}
		*/

	}
	
	/*
	for (; Index < List.Num(); Index++) {
		//if (Time.GetTime() > TimeLimit) return;

		FSBuildable* Buildable = &List[Index];
		if (Buildable) {
			UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(Buildable);

			UFSBuildableOperator* OperatorSwitch = nullptr;

			// check to see if its a pure abstract because the buildable wont exist
			if (!Buildable->Buildable) continue;

			if (Cast<AFGBuildableRailroadTrack>(Buildable->Buildable)) {
				//if (!Operator->firstBuild) {

				AFGBuildableRailroadTrack* Track = Cast<AFGBuildableRailroadTrack>(Buildable->Buildable);

				TArray< UFGRailroadTrackConnectionComponent* > mConnections = Track->mConnections;

				// cycle through all connections for this piece of track
				for (int i = 0; i < mConnections.Num(); i++) {
					AFGBuildableRailroadSwitchControl* Switch = mConnections[i]->GetSwitchControl();

					// find if this track has a valid switch, may or may not if its a track that has multiple connections
					if (Switch) {

						FSBuildable SwitchBuildable;
						SwitchBuildable.Buildable = Switch;

						OperatorSwitch = OperatorFactory->AcquireOperator(&SwitchBuildable);


						// get the track this switch has control over
						AFGBuildableRailroadTrack* Track2 = mConnections[i]->GetTrack();

						if (Track2) {

							AFGBuildable* NewTrack = *BuildableMapping.Find(Track2);

							// get the new track thats spawning in
							AFGBuildableRailroadTrack* Track3 = Cast<AFGBuildableRailroadTrack>(NewTrack);
							TArray< UFGRailroadTrackConnectionComponent* > mConnections2 = Track3->mConnections;

							// now we have to find the connection component that belongs to this piece of track that needs a new switch created for it

							for (int j = 0; j < mConnections2.Num(); j++) {

								AFGBuildableRailroadTrack* Track4 = mConnections2[j]->GetTrack();
								if (Track4 == Track3) {
									OperatorSwitch->Connection0 = mConnections2[j];
									break;
								}
							}

						}

					}
				}

				//UFGRailroadTrackConnectionComponent* Connection1 = Track->GetForwardConnection();
				//UFGRailroadTrackConnectionComponent* Connection2 = Cast<AFGBuildableRailroadTrack>(Buildable->Buildable)->GetReverseConnection();

				//Operator->Connection0 = Cast<AFGBuildableRailroadSwitchControl>(Buildable->Buildable)->mControlledConnection;

				if (!OperatorSwitch) continue;
				AFGBuildable* NewBuildable = OperatorSwitch->CreateCopy(FSTransform);

				if (NewBuildable != nullptr) {
					BuildableMapping.Add(Buildable->Buildable, NewBuildable);

					if (NewBuildable) {

						Operator->UpdateInternelConnection(NewBuildable);
						//NewBuildable->PlayBuildEffects(this->Player);
						//NewBuildable->ExecutePlayBuildEffects();

						FSBuildable buildable;
						buildable.Buildable = NewBuildable;
						this->NewDesign->BuildableSet.Add(buildable);
					}
				}
				//Current++;
				//}
			}
			else {
			}
		}
	}
	*/

	Index = 0;

}

void UFSSyncBuild::StepC()
{
	AFGBuildableSubsystem* BuildableSubsystem = AFGBuildableSubsystem::Get(this);
	AFGPipeSubsystem* PipeSubsystem = AFGPipeSubsystem::Get(GetWorld());
	AFGRailroadSubsystem* RailroadSubsystem = AFGRailroadSubsystem::Get(this);

	for (; Index < List.Num(); Index++) {

		FSBuildable* Buildable = &List[Index];

		if (Buildable) {
			// check to see if its a pure abstract because the buildable wont exist
			if (!Buildable->Buildable) continue;
		}

		AFGBuildable** Ptr = BuildableMapping.Find(Buildable->Buildable);
		if (Ptr) {
			IFGFluidIntegrantInterface* FluidIntegrant = Cast<IFGFluidIntegrantInterface>(*Ptr);
			if (FluidIntegrant) {
				PipeSubsystem->UnregisterFluidIntegrant(FluidIntegrant);
			}
		}
		
	}
	
	for (Index = 0; Index < List.Num(); Index++) {
		//if (Time.GetTime() > TimeLimit) return;

		FSBuildable* Buildable = &List[Index];

		if (Buildable) {
			// check to see if its a pure abstract because the buildable wont exist
			if (!Buildable->Buildable) continue;
		}

		AFGBuildable** Ptr = BuildableMapping.Find(Buildable->Buildable);
		if (Ptr) {
			AFGBuildableConveyorBase* Conveyor = Cast<AFGBuildableConveyorBase>(*Ptr);
			if (Conveyor) {
				BuildableSubsystem->AddConveyor(Conveyor);
				BuildableSubsystem->mConveyorBucketGroupsDirty = true;
			}
			AFGBuildableRailroadStation* Station = Cast<AFGBuildableRailroadStation>(*Ptr);
			if (Station) {
				RailroadSubsystem->AddTrainStation(Station);
				//SML::Logging::info(Station->GetTrackGraphID()); 
				//SML::Logging::info(Station->mStationIdentifier->GetTrackGraphID());
			}
			IFGFluidIntegrantInterface* FluidIntegrant = Cast<IFGFluidIntegrantInterface>(*Ptr);
			if (FluidIntegrant) {
				PipeSubsystem->RegisterFluidIntegrant(FluidIntegrant);
			}
			/*
			AFGBuildableRailroadTrack* Track = Cast<AFGBuildableRailroadTrack>(*Ptr);
			if (Track) {
				SML::Logging::info(Track->GetTrackGraphID());
				//RailroadSubsystem->AddTrack(Track);
			}*/
		}
	}

	/*
	for (Index = 0; Index < List.Num(); Index++) {
		AFGBuildable* ExistingBuildable = List[Index].Get();
		AFGBuildable** Ptr = BuildableMapping.Find(ExistingBuildable);
		if (Ptr) {
			AFGBuildableRailroadSignal* NewSignal = Cast<AFGBuildableRailroadSignal>(*Ptr);
			// once a signal is found
			if (NewSignal) {
				AFGBuildableRailroadSignal* ExistingSignal = Cast<AFGBuildableRailroadSignal>(ExistingBuildable);
				TArray< UFGRailroadTrackConnectionComponent*> GuardedConnections = ExistingSignal->mGuardedConnections;
				for (int q = 0; q < GuardedConnections.Num(); q++) {
					UFGRailroadTrackConnectionComponent* GuardedConnection = GuardedConnections[q];

					// now find a existing track this connection is connected to
					for (int i = 0; i < List.Num(); i++) {
						AFGBuildable* ExistingBuildable = List[i].Get();
						AFGBuildable** Ptr = BuildableMapping.Find(ExistingBuildable);
						if (Ptr) {
							AFGBuildableRailroadTrack* NewTrack = Cast<AFGBuildableRailroadTrack>(*Ptr);
							if (NewTrack) {
								AFGBuildableRailroadTrack* ExistingTrack = Cast<AFGBuildableRailroadTrack>(ExistingBuildable);
								UFGRailroadTrackConnectionComponent* Connection = ExistingTrack->GetConnection(0);

								if (GuardedConnection == Connection) {
									NewSignal->mObservedConnections.Add(NewTrack->GetConnection(0));
								}

							}
						}
					}
				}
			}
		}
	}
	*/

	// first iterate through all signals
	
	for (Index = 0; Index < List.Num(); Index++) {

		FSBuildable* Buildable = &List[Index];

		if (Buildable) {
			// check to see if its a pure abstract because the buildable wont exist
			if (!Buildable->Buildable) continue;
		}

		AFGBuildable* ExistingBuildable = List[Index].Buildable;
		//AFGBuildable** Ptr = BuildableMapping.Find(ExistingBuildable);
		if (ExistingBuildable) {
			AFGBuildableRailroadSignal* ExistingSignal = Cast<AFGBuildableRailroadSignal>(ExistingBuildable);
			// once a signal is found
			if (ExistingSignal) {

				/*
				UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(Cast< AFGBuildable>(ExistingSignal));

				AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);
				if (NewBuildable != nullptr) {
					BuildableMapping.Add(Cast< AFGBuildable>(ExistingSignal), NewBuildable);
					Operator->UpdateInternelConnection(NewBuildable);
					this->NewDesign->BuildableSet.Add(NewBuildable);
				}
				*/

				TArray< UFGRailroadTrackConnectionComponent*> GuardedConnections = ExistingSignal->mGuardedConnections;
				
				for (int q = 0; q < GuardedConnections.Num(); q++) {
					UFGRailroadTrackConnectionComponent* GuardedConnection = GuardedConnections[q];

					// now find a existing track this connection is connected to
					for (int i = 0; i < List.Num(); i++) {
						AFGBuildable* ExistingBuildable = List[i].Buildable;
						AFGBuildable** Ptr = BuildableMapping.Find(ExistingBuildable);
						if (Ptr) {
							AFGBuildableRailroadTrack* NewTrack = Cast<AFGBuildableRailroadTrack>(*Ptr);
							if (NewTrack) {
								AFGBuildableRailroadTrack* ExistingTrack = Cast<AFGBuildableRailroadTrack>(ExistingBuildable);
								UFGRailroadTrackConnectionComponent* Connection1 = ExistingTrack->GetConnection(1);

								if (GuardedConnection == Connection1) {
									
									FSBuildable ptr;
									ptr.Buildable = Cast< AFGBuildable>(ExistingSignal);

									UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(&ptr);

									Operator->Connection1 = NewTrack->GetConnection(1);

									AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);
									if (NewBuildable != nullptr) {
										BuildableMapping.Add(Cast< AFGBuildable>(ExistingSignal), NewBuildable);
										Operator->UpdateInternelConnection(NewBuildable);

										FSBuildable buildable;
										buildable.Buildable = NewBuildable;
										this->NewDesign->BuildableSet.Add(buildable);
									}
									//NewSignal->mObservedConnections.Add(NewTrack->GetConnection(0));
								}

							}
						}
					}
				}
				
			}
		}
	}

	// not sure what AddSignal actually does?
	for (Index = 0; Index < List.Num(); Index++) {

		FSBuildable* Buildable = &List[Index];

		if (Buildable) {
			// check to see if its a pure abstract because the buildable wont exist
			if (!Buildable->Buildable) continue;
		}

		AFGBuildable** Ptr = BuildableMapping.Find(Buildable->Buildable);
		if (Ptr) {
			AFGBuildableRailroadSignal* Signal = Cast<AFGBuildableRailroadSignal>(*Ptr);
			if (Signal) {
				RailroadSubsystem->AddSignal(Signal);
			}
		}
	}

	for (Index = 0; Index < List.Num(); Index++) {
		//if (Time.GetTime() > TimeLimit) return;

		FSBuildable* Buildable = &List[Index];
		if (Buildable) {
			UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(Buildable);

			// check to see if its not a pure abstract because we only want to do specific logic with them
			if (Buildable->Buildable) continue;

			//if (Cast<AFGBuildableWire>(Buildable->Buildable)) {
				//if (!Operator->firstBuild) {
				AFGBuildable* NewBuildable = Operator->CreateCopy(FSTransform);

				if (NewBuildable != nullptr) {
					//NewBuildable->Destroy();
					//BuildableMapping.Add(Buildable->Buildable, NewBuildable);

					if (NewBuildable) {

						//Operator->UpdateInternelConnection(NewBuildable);
						//NewBuildable->PlayBuildEffects(this->Player);
						//NewBuildable->ExecutePlayBuildEffects();

						FSBuildable buildable;
						buildable.Buildable = NewBuildable;
						this->NewDesign->BuildableSet.Add(buildable);
					}
				}
				Current++;
				//}
			//}
		}
	}
	Index = 0;

	
		for (; Index < List.Num(); Index++) {
			//if (Time.GetTime() > TimeLimit) return;

			FSBuildable* Buildable = &List[Index];
			if (Buildable) {
				UFSBuildableOperator* Operator = OperatorFactory->AcquireOperator(Buildable);

				UFSBuildableOperator* OperatorSwitch = nullptr;

				// check to see if its a pure abstract because the buildable wont exist
				if (!Buildable->Buildable) continue;

				if (Cast<AFGBuildableRailroadTrack>(Buildable->Buildable)) {
					//if (!Operator->firstBuild) {

					AFGBuildableRailroadTrack* Track = Cast<AFGBuildableRailroadTrack>(Buildable->Buildable);

					TArray< UFGRailroadTrackConnectionComponent* > mConnections = Track->mConnections;

					// cycle through all connections for this piece of track
					for (int i = 0; i < mConnections.Num(); i++) {
						AFGBuildableRailroadSwitchControl* Switch = mConnections[i]->GetSwitchControl();

						// find if this track has a valid switch, may or may not if its a track that has multiple connections
						if (Switch) {

							FSBuildable SwitchBuildable;
							SwitchBuildable.Buildable = Switch;

							OperatorSwitch = OperatorFactory->AcquireOperator(&SwitchBuildable);


							// get the track this switch has control over
							AFGBuildableRailroadTrack* Track2 = mConnections[i]->GetTrack();

							//check if its valid
							if (Track2) {
								//find the new track thats being corrected as this needs a new control component for the switch
								AFGBuildable* NewTrack = *BuildableMapping.Find(Track2);
								AFGBuildableRailroadTrack* Track3 = Cast<AFGBuildableRailroadTrack>(NewTrack);
								// this makes and attachs a new component to this new track thats being created, so the switch can use it
								UFGRailroadTrackConnectionComponent* NewConnectionComponent = DuplicateSceneComponent(NewTrack, mConnections[i]);
								// finally store the pointer to this component in the operator, so when we get to that logic the switch can be set to it
								OperatorSwitch->Connection0 = NewConnectionComponent;
							}

							UFGRailroadTrackConnectionComponent* SwitchControlledConnectionComponent = Switch->mControlledConnection;

							TArray< UFGRailroadTrackConnectionComponent* > SwitchConnectedComponents = SwitchControlledConnectionComponent->mConnectedComponents;

							for (int j = 0; j < SwitchConnectedComponents.Num(); j++) {
								UFGRailroadTrackConnectionComponent* ConnectedComponent = SwitchConnectedComponents[j];
								AFGBuildableRailroadTrack* ConnectedTrack = Cast<AFGBuildableRailroadTrack>(ConnectedComponent->GetAttachmentRootActor());
								TArray< UFGRailroadTrackConnectionComponent* > ConnectedTrackConnectedComponent = ConnectedTrack->mConnections;

								// index for the connection components we should get for the new tracks to store in the new connection component
								int TrackConnectionIndex = 0;

								for (int q = 0; q < ConnectedTrackConnectedComponent.Num(); q++) {
									if (ConnectedComponent == ConnectedTrackConnectedComponent[q]) {
										TrackConnectionIndex = q;
										break;
									}
								}

								AFGBuildableRailroadTrack* NewTrack = Cast<AFGBuildableRailroadTrack>(*BuildableMapping.Find(ConnectedTrack));
								TArray< UFGRailroadTrackConnectionComponent* > NewTrackConnectedTrackConnectedComponent = NewTrack->mConnections;
								OperatorSwitch->Connection0->mConnectedComponents.Add(NewTrack->mConnections[TrackConnectionIndex]);
							}

							/*
							if (Track2) {

								AFGBuildable* NewTrack = *BuildableMapping.Find(Track2);

								//DuplicateSceneComponent

								// get the new track thats spawning in
								AFGBuildableRailroadTrack* Track3 = Cast<AFGBuildableRailroadTrack>(NewTrack);
								TArray< UFGRailroadTrackConnectionComponent* > mConnections2 = Track3->mConnections;

								// now we have to find the connection component that belongs to this piece of track that needs a new switch created for it

								for (int j = 0; j < mConnections2.Num(); j++) {

									AFGBuildableRailroadTrack* Track4 = mConnections2[j]->GetTrack();
									if (Track4 == Track3) {
										OperatorSwitch->Connection0 = mConnections2[j];
										break;
									}
								}

							}
							*/
						}
					}

					//UFGRailroadTrackConnectionComponent* Connection1 = Track->GetForwardConnection();
					//UFGRailroadTrackConnectionComponent* Connection2 = Cast<AFGBuildableRailroadTrack>(Buildable->Buildable)->GetReverseConnection();

					//Operator->Connection0 = Cast<AFGBuildableRailroadSwitchControl>(Buildable->Buildable)->mControlledConnection;

					if (!OperatorSwitch) continue;
					AFGBuildable* NewBuildable = OperatorSwitch->CreateCopy(FSTransform);

					if (NewBuildable != nullptr) {
						BuildableMapping.Add(Buildable->Buildable, NewBuildable);

						if (NewBuildable) {

							Operator->UpdateInternelConnection(NewBuildable);
							//NewBuildable->PlayBuildEffects(this->Player);
							//NewBuildable->ExecutePlayBuildEffects();

							FSBuildable buildable;
							buildable.Buildable = NewBuildable;
							this->NewDesign->BuildableSet.Add(buildable);
						}
					}
					//Current++;
					//}
				}
				else {
				}
			}
		}
		

	Index = 0;

	BuildableSubsystem->mFactoryBuildingGroupsDirty = true;

	//AFGBuildable** Result = BuildableMapping.Find(this->Anchor.Get());

	// TODO REWORK THIS
	//NewDesign->Anchor = Result ? *Result : nullptr;

	AFSkyline* Skyline = AFSkyline::Get(this);
	Skyline->SkylineUI->CompletionWidget->Load(CurrentDesignMenu, NewDesignMenu);
	Skyline->SkylineUI->CompletionWidget->StartEnterAnimation();

	//Skyline->FSCtrl->Collectgarbage = true;
	//Skyline->FSCtrl->IsDoWorkComplete = true;

	//UFSSelection::SetAutoRebuildTreeAll(true);

	Step++;
}

int UFSSyncBuild::GetTotal()
{
	return Total;
}

int UFSSyncBuild::GetCurrent()
{
	return Current;
}

void UFSSyncBuild::StartDoWorkTimer(float TimeInterval)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);
	FSkyline->GetWorldTimerManager().SetTimer(TimerHandle_DoWork, this, &UFSSyncBuild::DoWorkTimerCallback, TimeInterval, true);
}
UFUNCTION()
void UFSSyncBuild::DoWorkTimerCallback()
{
	AFSkyline* FSkyline = AFSkyline::Get(this);
	FSkyline->Builder->SyncBuild->DoWork();
}

UFGRailroadTrackConnectionComponent* UFSSyncBuild::DuplicateSceneComponent(UObject* Outer, UFGRailroadTrackConnectionComponent* Original)
{
	if (!Original || !Outer)
	{
		return nullptr;
	}

	// Create a new component of the same class as the original
	UFGRailroadTrackConnectionComponent* NewComponent = NewObject<UFGRailroadTrackConnectionComponent>(Outer, Original->GetClass(), Original->GetFName());

	if (!NewComponent)
	{
		return nullptr;
	}

	// Copy relevant properties
	NewComponent->SetWorldTransform(Original->GetComponentTransform());
	NewComponent->SetVisibility(Original->IsVisible());
	NewComponent->SetRelativeScale3D(Original->GetRelativeScale3D());
	NewComponent->SetMobility(Original->Mobility);

	// Attach to the same parent if applicable
	if (Original->GetAttachParent())
	{
		NewComponent->AttachToComponent(Original->GetAttachParent(), FAttachmentTransformRules::KeepWorldTransform);
	}

	/*
	// Copy tags or other custom properties
	NewComponent->ComponentTags = Original->ComponentTags;

	// Iterate through children if needed
	for (USceneComponent* Child : Original->GetAttachChildren())
	{
		USceneComponent* DuplicatedChild = DuplicateSceneComponent(Outer, Child);
		if (DuplicatedChild)
		{
			DuplicatedChild->AttachToComponent(NewComponent, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
	*/

	// Register the new component if required
	NewComponent->RegisterComponent();

	return NewComponent;
}