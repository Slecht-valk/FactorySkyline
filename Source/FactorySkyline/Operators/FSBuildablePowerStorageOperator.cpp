// ILikeBanas


#include "FSBuildablePowerStorageOperator.h"
#include "Buildables/FGBuildablePowerStorage.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableConveyorLift.h"
#include "Buildables/FGBuildableCircuitSwitch.h"
#include "Hologram/FGConveyorLiftHologram.h"
#include "FactorySkyline/FSkyline.h"
#include "Buildables/FGBuildableLightSource.h"
/*
void UFSBuildableSignOperator::UpdateHologramState(const FHitResult& Hit, AFGHologram* Hologram, bool& ShouldShow, bool& Valid)
{
	
	AFGConveyorLiftHologram* LiftHologram = Cast<AFGConveyorLiftHologram>(Hologram);

	ShouldShow = Valid = false;
	if (!Hologram->IsValidHitResult(Hit)) return;

	Hologram->SetHologramLocationAndRotation(Hit);

	AActor* Actor = Hit.GetActor();
	this->HitConnection = nullptr;

	if (!Actor) return;

	FVector HologramLocation = Hologram->GetTransform().GetLocation();

	for (UActorComponent* Connection : Actor->GetComponentsByClass(UFGFactoryConnectionComponent::StaticClass())) {
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
*/
/*
AFGHologram* UFSBuildableSignOperator::HologramCopy(FTransform& RelativeTransform)
{
	//return SplineHologramFactory->CreateLiftHologram(Cast<AFGBuildableConveyorLift>(Source), RelativeTransform);
	RelativeTransform = Source->GetTransform();
	AFGHologram* Hologram = CreateHologram();
	if (!Hologram) return nullptr;

	AFGBuildableBeam* SourceBuildableBeam = Cast<AFGBuildableBeam>(Source);
	AFGBeamHologram* BuildableBeamHologram = Cast<AFGBeamHologram>(Hologram);

	if (!BuildableBeamHologram) return Hologram;

	BuildableBeamHologram->mSize = SourceBuildableBeam->mSize;
	BuildableBeamHologram->mDefaultLength = SourceBuildableBeam->mDefaultLength;
	BuildableBeamHologram->mMaxLength = SourceBuildableBeam->mMaxLength;
	BuildableBeamHologram->mCurrentLength = SourceBuildableBeam->mLength;
	//BuildableBeamHologram->SetCurrentLength(SourceBuildableBeam->mLength);
	BuildableBeamHologram->OnRep_CurrentLength();
	return Hologram;
}
*/

AFGBuildable* UFSBuildablePowerStorageOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);

	//FTransform Transform;

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}

	AFGBuildable* Buildable = nullptr;
	AFGBuildablePowerStorage* SourceBuildablePowerStorage = nullptr;

	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
		SourceBuildablePowerStorage = Cast<AFGBuildablePowerStorage>(Source.Buildable);
	}

	AFGBuildablePowerStorage* BuildablePowerStorage = Cast<AFGBuildablePowerStorage>(Buildable);

	BuildablePowerStorage->mPowerStore = SourceBuildablePowerStorage->mPowerStore;

	TSubclassOf<UFGRecipe> Recipe;

	if (Source.Buildable) {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
	}

	if (Source.Buildable) {
		if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
	}
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	return Buildable;
}
