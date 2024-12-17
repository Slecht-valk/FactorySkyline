// ILikeBanas


#include "FSJumppadOperator.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableJumppad.h"
#include "Hologram/FGJumpPadLauncherHologram.h"
#include "FGFactoryLegsComponent.h"
#include "FGPipeConnectionComponent.h"


AFGHologram* UFSJumppadOperator::HologramCopy(FTransform& RelativeTransform)
{
	//RelativeTransform = Source->GetTransform();
	if (Source.Buildable) {
		RelativeTransform = Source.Buildable->GetTransform();
	}
	AFGJumpPadLauncherHologram* Hologram = Cast<AFGJumpPadLauncherHologram>(CreateHologram());

	AFGBuildableJumppad* SourceJumppad;
	if (Source.Buildable) {
		SourceJumppad = Cast<AFGBuildableJumppad>(Source.Buildable);
	}
	/*
	Hologram->DoMultiStepPlacement(false);
	Hologram->mLaunchAngle = SourceJumppad->mLaunchAngle;
	Hologram->ScrollRotate(10, 1);
	Hologram->ScrollRotate(-10, 1);
	//Hologram->OnLaunchAngleAdjusted();
	*/
	return Hologram;
}

AFGBuildable* UFSJumppadOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}

	AFGBuildable* Buildable = nullptr;
	AFGBuildableJumppad* SourceJumppad = nullptr;

	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
		SourceJumppad = Cast<AFGBuildableJumppad>(Source.Buildable);
	}

	AFGBuildableJumppad* TargetJumppad = Cast<AFGBuildableJumppad>(Buildable);
	TSubclassOf<UFGRecipe> Recipe;
	if (Source.Buildable) {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
		if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
	}
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);
	//TODO:
	//Buildable->SetBuildingID(Source->GetBuildingID());
	TargetJumppad->SetLaunchAngle(SourceJumppad->mLaunchAngle);
	//SML::Logging::info(SourceJumppad->mLaunchAngle);

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	UFGFactoryLegsComponent* Legs = Buildable->FindComponentByClass<UFGFactoryLegsComponent>();
	if (Legs) {
		TArray< FFeetOffset > feetOffset = Legs->TraceFeetOffsets(Transform, Buildable);
		Legs->SetFeetOffsets(feetOffset);

		// TODO DO WE NEED A ALTERNATIVE TO THIS?
		//Legs->RecreateLegs();
	}

	return Buildable;
}

