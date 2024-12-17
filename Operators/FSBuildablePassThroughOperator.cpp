// ILikeBanas


#include "FSBuildablePassThroughOperator.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableConveyorLift.h"
#include "Buildables/FGBuildablePassthrough.h"
#include "Hologram/FGPassthroughHologram.h"
#include "Hologram/FGConveyorLiftHologram.h"
#include "FactorySkyline/FSkyline.h"

void UFSBuildablePassThroughOperator::UpdateHologramState(const FHitResult& Hit, AFGHologram* Hologram, bool& ShouldShow, bool& Valid)
{
	AFGConveyorLiftHologram* LiftHologram = Cast<AFGConveyorLiftHologram>(Hologram);

	ShouldShow = Valid = false;
	if (!Hologram->IsValidHitResult(Hit)) return;

	Hologram->SetHologramLocationAndRotation(Hit);

	AActor* Actor = Hit.GetActor();
	this->HitConnection = nullptr;

	if (!Actor) return;

	FVector HologramLocation = Hologram->GetTransform().GetLocation();

	for (UActorComponent* Connection : Actor->K2_GetComponentsByClass(UFGFactoryConnectionComponent::StaticClass())) {
		UFGFactoryConnectionComponent* FactoryConnection = Cast<UFGFactoryConnectionComponent>(Connection);
		if (FactoryConnection) {
			if (HitConnection == nullptr) HitConnection = FactoryConnection;
			else {
				FVector HitLocation = HitConnection->GetComponentLocation();
				FVector FactoryLocation = FactoryConnection->GetComponentLocation();
				if ((FactoryLocation - HologramLocation).SizeSquared() < (HitLocation - HologramLocation).SizeSquared())
					HitConnection = FactoryConnection;
			}
		}
	}

	ShouldShow = Valid = true;
}

AFGHologram* UFSBuildablePassThroughOperator::HologramCopy(FTransform& RelativeTransform)
{
	RelativeTransform = Source->GetTransform();
	TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());
	if (!Recipe) return nullptr;
	AFGHologram* Hologram = AFGHologram::SpawnHologramFromRecipe(Recipe, Builder, FVector(0.0f, 0.0f, 0.0f), ((AFSkyline*)Skyline)->FSCtrl->GetPlayer());
	
	if (!Hologram) return nullptr;
	AFGPassthroughHologram* PassthroughHologram = Cast<AFGPassthroughHologram>(Hologram);
	if (!PassthroughHologram) return Hologram;

	AFGBuildablePassthrough* SourceBuildablePassthrough = Cast<AFGBuildablePassthrough>(Source);

	//float num = SourceBuildablePassthrough->mSnappedBuildingThickness;
	//PassthroughHologram.snapped

	FHitResult Hit;
	//Hit.Actor = nullptr;
	Hit.Time = 0.006946;
	Hit.Location = FVector(-11720.067f, 248538.719f, -10141.936f);
	Hit.ImpactPoint = FVector(-11720.066f, 248538.719f, -10141.936f);
	Hit.Normal = FVector(1.0f, 0.0f, 0.0f);
	Hit.ImpactNormal = FVector(1.0f, 0.0f, 0.0f);
	Hit.TraceStart = FVector(-11025.803f, 248538.188f, -10162.381f);
	Hit.TraceEnd = FVector(-110982.445f, 248615.406f, -12781.198f);
	Hit.PenetrationDepth = 0.0f;
	Hit.Item = -1;
	Hit.FaceIndex = -1;

	//PassthroughHologram->SnapHologramLocationToGuidelines(Source->GetTransform().GetTranslation());
	//PassthroughHologram->TrySnapToActor(Hit);

	//PassthroughHologram->BuildMeshes();

	/*
	float num = SourceBuildablePassthrough->mSnappedBuildingThickness;
	PassthroughHologram->mSnappedBuildingThickness = num;
	PassthroughHologram->mConnectionClass = SourceBuildablePassthrough->mConnectionComponentClass;
	PassthroughHologram->mClearanceHeightMin = 100;
	PassthroughHologram->mClearanceThickness = 100;
	PassthroughHologram->mMinHeight = 100;
	PassthroughHologram->OnPendingConstructionHologramCreated_Implementation(PassthroughHologram);
	PassthroughHologram->BuildMeshes();
	PassthroughHologram->UpdateClearance();
	*/

	Hologram->SetActorTransform(Source->GetTransform());

	return Hologram;
}

AFGBuildable* UFSBuildablePassThroughOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);

	FTransform Transform = TransformOperator.Transform(Source->GetTransform());
	AFGBuildable* Buildable = BuildableSubsystem->BeginSpawnBuildable(Source->GetClass(), Transform);
	AFGBuildablePassthrough* SourceBuildablePassthrough = Cast<AFGBuildablePassthrough>(Source);
	AFGBuildablePassthrough* BuildablePassthrough = Cast<AFGBuildablePassthrough>(Buildable);

	float num = SourceBuildablePassthrough->mSnappedBuildingThickness;
	//BuildablePassthrough->mSnappedBuildingThickness = 50;
	BuildablePassthrough->mSnappedBuildingThickness = num;

	BuildablePassthrough->mMidMeshLength = SourceBuildablePassthrough->mMidMeshLength;
	BuildablePassthrough->mGenerateTunnelCollision = SourceBuildablePassthrough->mGenerateTunnelCollision;
	BuildablePassthrough->mEndCapRotation = SourceBuildablePassthrough->mEndCapRotation;
	BuildablePassthrough->mMidMeshRotation = SourceBuildablePassthrough->mMidMeshRotation;
	BuildablePassthrough->mEndCapTranslation = SourceBuildablePassthrough->mEndCapTranslation;
	BuildablePassthrough->mClearanceHeightMin = SourceBuildablePassthrough->mClearanceHeightMin;
	BuildablePassthrough->mClearanceThickness = SourceBuildablePassthrough->mClearanceThickness;

	// NO LONGER VALID
	//BuildablePassthrough->mUseSoftClearance = SourceBuildablePassthrough->mUseSoftClearance;

	//AFGBuildableConveyorLift* SourceConveyorLift = Cast<AFGBuildableConveyorLift>(Source);
	//AFGBuildableConveyorLift* TargetConveyorLift = Cast<AFGBuildableConveyorLift>(Buildable);

	TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());
	if (!Recipe) Recipe = Source->GetBuiltWithRecipe();
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);
	//Buildable->SetBuildingID(Source->GetBuildingID());
	/*
	TargetConveyorLift->mTopTransform = SourceConveyorLift->mTopTransform;
	TargetConveyorLift->mIsReversed = SourceConveyorLift->mIsReversed;
	*/
	//FSkyline->AdaptiveUtil->CopyConveyorLiftAttribute(SourceConveyorLift, TargetConveyorLift);

	Buildable->SetCustomizationData_Implementation(Source->GetCustomizationData_Implementation());
	Buildable->FinishSpawning(Transform);

	//this->BuildableSubsystem->RemoveConveyorFromBucket(TargetConveyorLift);

	return Buildable;
}
